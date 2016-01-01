/*-
 * Copyright (c) 2012, 2013, 2015 Oliver Lehmann
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer
 *    in this position and unchanged.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <stdbool.h>
#include "wdc_config.h"
#include <avr/io.h>
#include "wdc_avr.h"
#include "wdc_drv_mmc.h"
#include "wdc_types.h"
#include "uart.h"

#define wait_till_send_done() while ( ( SPSR & ( 1 << SPIF ) ) == 0 )
#define wait_till_card_ready() do { send_dummy_byte(); } while ( recv_byte() == 0 )
#define send_dummy_byte() SPDR = 0xFF; wait_till_send_done()
#define recv_byte() SPDR
#define xmit_byte( x ) SPDR = x

#define CMD0  ( 0x40 + 0 )
#define CMD1  ( 0x40 + 1 )
#define CMD8  ( 0x40 + 8 )
#define CMD12 ( 0x40 + 12 )
#define CMD17 ( 0x40 + 17 )
#define CMD18 ( 0x40 + 18 )
#define CMD23 ( 0x40 + 23 )
#define CMD24 ( 0x40 + 24 )
#define CMD25 ( 0x40 + 25 )
#define CMD55 ( 0x40 + 55 )
#define CMD58 ( 0x40 + 58 ) /* READ_OCR */
#define CMD59 ( 0x40 + 59 )

#define SB_START 0xFE
#define MB_START 0xFC
#define MB_STOP  0xFD

static uint8_t mmc_cmd ( uint8_t * );
static uint8_t mmc_do_init ();

static bool is_block_addressing = false;

uint8_t mmc_init ()
{
    uint8_t i;
    uint8_t ret = 1;

    uart_putstring ( PSTR ( "INFO: SDCard init start" ), true );

    for ( i = 0; i < 10; i++ ) {
        ret = mmc_do_init();
        if ( ret == 0 ) {
            break;
        }
    }

    return ret;
}

static uint8_t mmc_do_init ()
{
    uint16_t t16 = 0;
    uint8_t  a;
    uint8_t  cmd[6];
    uint8_t  is_sd2 = 0;


    for ( a = 0; a < 200; a++ ) {
        nop();
    }

    SPCR = ( 1 << SPE ) | ( 1 << MSTR ) | ( 1 << SPR0 ) | ( 1 << SPR1 ); /* Enable SPI, SPI in Master Mode */
    SPSR = ( 0 << SPI2X );

    disable_mmc();

    for ( a = 0; a < 0x0f; a++ ) {
        send_dummy_byte();
    }

    enable_mmc();

    /*
     * Send CMD0 to enter idle state.
     */
    cmd[0] = CMD0;
    cmd[1] = 0x00;
    cmd[2] = 0x00;
    cmd[3] = 0x00;
    cmd[4] = 0x00;
    cmd[5] = 0x95;
    while ( mmc_cmd ( cmd ) != 1 ) {
        if ( t16++ > 1000 ) {
            disable_mmc();
            return 1;
        }
    }

    send_dummy_byte();

    /*
     * Send CMD8 to find out if it is a SD 2.0 or SDSC / MMC card.
     */
    cmd[0] = CMD8;
    cmd[1] = 0x00;
    cmd[2] = 0x00;
    cmd[3] = 0x01;
    cmd[4] = 0xaa;
    cmd[5] = 0x87;
    /* if SD 2.0 Card */
    if ( mmc_cmd ( cmd ) == 1 ) {
        /* skip 5 bytes */
        for ( a = 0; a < 5; a++ ) {
            send_dummy_byte();
        }

        is_sd2 = 1;

        /* prepare next cmd */
        cmd[0] = CMD1;
        cmd[1] = 0x40; /* HCS bit */
        cmd[2] = cmd[3] = cmd[4] = 0x00;
        cmd[5] = 0xFF;
    } else {

        send_dummy_byte();
        is_block_addressing = false;

        /* prepare next cmd */
        cmd[0] = CMD1;
        cmd[1] = cmd[2] = cmd[3] = cmd[4] = 0x00;
        cmd[5] = 0xFF;

        uart_putstring ( PSTR ( "INFO: SDSC disk has been found" ), true );
    }

    /*
     * Send CMD1 to wait until the card leaves idle state.
     */
    t16 = 0;
    while ( mmc_cmd ( cmd ) != 0 ) {
        send_dummy_byte();
        if ( t16++ > 1000 ) {
            disable_mmc();
            return 2;
        }
    }

    send_dummy_byte();

    if ( is_sd2 == 1 ) {
        /*
         * Send CMD58 to find out if the SD 2.0 card wants block or by addressing.
         */
        cmd[0] = CMD58;
        cmd[1] = 0x00;
        if ( mmc_cmd ( cmd ) == 0 ) {
            send_dummy_byte();
            a = recv_byte();
            if ( ( a & 0x40 ) == 0x40 ) {
                is_block_addressing = true;
                uart_putstring ( PSTR ( "INFO: SD2.0 disk with block addressing has been found" ), true );
            } else {
                uart_putstring ( PSTR ( "INFO: SD2.0 disk with byte addressing has been found" ), true );
            }

            /* skip 4 bytes */
            for ( a = 1; a < 5; a++ ) {
                send_dummy_byte();
            }
        }
    }

    send_dummy_byte();

    /* SPI Bus to max. speed */
    SPCR &= ~( ( 1 << SPR0 ) | ( 1 << SPR1 ) );
    SPSR = SPSR | ( 1 << SPI2X );

    disable_mmc();
    return 0;
}

static uint8_t mmc_cmd ( uint8_t *cmd )
{
    uint8_t tmp = 0x80;
    uint8_t i = 10;
    uint8_t cmd0 = cmd[0];
    uint8_t a;

    /* send command */
    for ( a = 0; a < 0x06; a++ ) {
        xmit_byte ( *cmd++ );
        wait_till_send_done();
    }

    /*
     * The received byte immediataly following CMD12 is a stuff byte,
     * it should be discarded before receive the response of the CMD12
     */
    if ( cmd0 == ( CMD12 ) ) {
        tmp = recv_byte();
    }

    do {
        send_dummy_byte();
        tmp = recv_byte();
    } while ( ( tmp & 0x80 ) == 0x80 && --i > 0 );

    return tmp;
}

uint8_t mmc_write_sector ( uint32_t addr, uint8_t *buffer )
{
    sint32   x;
    uint8_t  resp;
    uint8_t  cmd[] = { CMD24, 0x00, 0x00, 0x00, 0x00, 0xFF };
    uint16_t i;

    enable_mmc();

    /* convert blocks to bytes */
    if ( !is_block_addressing ) {
        addr = addr * MMC_BLOCKLEN;
    }
    x.value32 = addr;
    cmd[1] = x.value8.hh;
    cmd[2] = x.value8.hl;
    cmd[3] = x.value8.lh;
    cmd[4] = x.value8.ll;

    wait_till_card_ready();

    /*
     * send CMD24
     */
    resp = mmc_cmd ( cmd );
    if ( resp != 0 ) {
        disable_mmc();
        return resp;
    }

    /* send Startbyte */
    xmit_byte ( SB_START );

    /* write a single block */
    for ( i = MMC_BLOCKLEN; i; i-- ) {
        uint8_t data = *buffer;
        buffer++;
        wait_till_send_done();
        xmit_byte ( data );
    }
    wait_till_send_done();

    /* handle CRC */
    send_dummy_byte();
    send_dummy_byte();

    /* check for errors */
    send_dummy_byte();
    if ( ( recv_byte() & 0x1F ) != 0x05 ) {
        disable_mmc();
        return 2;
    }

    disable_mmc();

    return 0;
}

uint8_t mmc_read_sector ( uint32_t addr, uint8_t *buffer )
{
    sint32   x;
    uint8_t  resp;
    uint16_t i = 1;
    uint8_t  cmd[] = { CMD17, 0x00, 0x00, 0x00, 0x00, 0xFF };

    enable_mmc();

    /* convert blocks to bytes */
    if ( !is_block_addressing ) {
        addr = addr * MMC_BLOCKLEN;
    }
    x.value32 = addr;

    cmd[1] = x.value8.hh;
    cmd[2] = x.value8.hl;
    cmd[3] = x.value8.lh;
    cmd[4] = x.value8.ll;

    wait_till_card_ready();

    /*
     * send CMD17
     */
    resp = mmc_cmd ( cmd );
    if ( resp != 0 ) {
        disable_mmc();
        return resp;
    }

    /* wait for startbyte */
    while ( true ) {
        send_dummy_byte();
        if ( recv_byte() == SB_START ) {
            break;
        }
    }

    /* read first byte */
    send_dummy_byte();
    *buffer = recv_byte();
    xmit_byte ( 0xff );

    /* read the remaining 511 bytes */
    do {
        buffer++;
        i++;
        wait_till_send_done();
        *buffer = recv_byte();
        xmit_byte ( 0xff );
    } while ( i < MMC_BLOCKLEN );

    /* handle CRC */
    wait_till_send_done();
    send_dummy_byte();

    disable_mmc();

    return 0;
}

uint8_t mmc_read_multiblock ( uint32_t addr, uint8_t *buffer, uint8_t numblocks )
{
    sint32  x;
    uint8_t resp;
    uint8_t cmd[] = { CMD18, 0x00, 0x00, 0x00, 0x00, 0xFF };

    enable_mmc();

    /* convert blocks to bytes */
    if ( !is_block_addressing ) {
        addr = addr * MMC_BLOCKLEN;
    }
    x.value32 = addr;

    cmd[1] = x.value8.hh;
    cmd[2] = x.value8.hl;
    cmd[3] = x.value8.lh;
    cmd[4] = x.value8.ll;

    wait_till_card_ready();

    /*
     * send CMD18
     */
    resp = mmc_cmd ( cmd );
    if ( resp != 0 ) {
        disable_mmc();
        return resp;
    }

    /* read the number of requested blocks */
    do {
        uint16_t i = 1;

        numblocks--;

        /* wait for startbyte */
        while ( true ) {
            send_dummy_byte();
            if ( recv_byte() == SB_START ) {
                break;
            }
        }

        send_dummy_byte();
        *buffer = recv_byte();
        xmit_byte ( 0xff );

        /* receive the block */
        do {
            buffer++;
            i++;
            wait_till_send_done();
            *buffer = recv_byte();
            xmit_byte ( 0xff );
        } while ( i < MMC_BLOCKLEN );

        /* handle CRC */
        wait_till_send_done();
        send_dummy_byte();

        buffer++;

    } while ( numblocks > 0 );

    wait_till_card_ready();

    /*
     * send CMD12
     */
    cmd[0] = CMD12;
    cmd[1] = cmd[2] = cmd[3] = cmd[4] = 0;

    /*
     * My Apacer SD-Card sends sometimes 0x7f as response to the CMD12 even if the write was
     * successfull. Because of that, no checking of the response to CMD12 is done.
     */
    mmc_cmd ( cmd );

    /* Pad 8 */
    send_dummy_byte();

    disable_mmc();

    return 0;
}

uint8_t mmc_write_multiblock ( uint32_t addr, uint8_t *buffer, uint8_t numblocks )
{
    sint32  x;
    uint8_t resp;
    uint8_t cmd[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF };

    enable_mmc();
    wait_till_card_ready();

    /* convert blocks to bytes */
    if ( !is_block_addressing ) {
        addr = addr * MMC_BLOCKLEN;
    }
    x.value32 = addr;
    cmd[0] = CMD25;
    cmd[1] = x.value8.hh;
    cmd[2] = x.value8.hl;
    cmd[3] = x.value8.lh;
    cmd[4] = x.value8.ll;

    /*
     * send CMD25
     */
    resp = mmc_cmd ( cmd );
    if ( resp > 1 ) {
        disable_mmc();
        return 1;
    }

    do {
        uint16_t i;

        wait_till_card_ready();

        /* Send multi-block start-token */
        xmit_byte ( MB_START );

        /* actually transfer the data */
        for ( i = MMC_BLOCKLEN; i > 0; i-- ) {
            uint8_t data = *buffer;
            buffer++;
            wait_till_send_done();
            xmit_byte ( data );
        }
        numblocks--;
        wait_till_send_done();
        /* handle CRC */
        send_dummy_byte();
        send_dummy_byte();

        /* check for errors */
        send_dummy_byte();
        if ( ( recv_byte() & 0x1F ) != 0x05 ) {
            disable_mmc();
            return 2;
        }

    } while ( numblocks > 0 );

    wait_till_card_ready();

    /* Send "transfer stop byte" */
    xmit_byte ( MB_STOP );
    wait_till_send_done();

    /* Pad 8 */
    send_dummy_byte();

    disable_mmc();

    return 0;
}