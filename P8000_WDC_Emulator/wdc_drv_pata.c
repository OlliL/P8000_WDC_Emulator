/*-
 * Copyright (c) 2012, 2013, 2105 Oliver Lehmann
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

#include "wdc_config.h"
#include <avr/io.h>
#include <util/delay.h>
#include "wdc_avr.h"
#include "uart.h"
#include "wdc_drv_pata.h"

void deactivate_p8000com();
void activate_p8000com();
uint8_t pata_bsy ();
uint8_t pata_rdy ();
uint8_t pata_drq ();
uint8_t pata_err ();
uint8_t ata_identify ();
void    set_io_register ( uint8_t ioreg );
uint8_t read_io_register ( uint8_t ioreg );
void    write_io_register ( uint8_t ioreg, uint8_t byte );
void    pata_read_bytes ( uint8_t *buffer, uint8_t numblocks );
void    pata_write_bytes ( uint8_t *buffer, uint8_t numblocks );
void    pata_rw_command( uint32_t addr, uint8_t numblocks, uint8_t cmd );

/*
 *                                                  +---- DA2
 *                                                  |+--- DA1
 *                                                  ||+-- DA0
 *                                                  |||+- CS0
 * Command Block registers                          |||| */
#define PATA_RW_DATA_REGISTER            0x00  /* 0b0000 */
#define PATA_R_ERROR_REGISTER            0x02  /* 0b0010 */
#define PATA_W_FEATURE_REGISTER          0x02  /* 0b0010 */
#define PATA_RW_SECTOR_COUNT_REGISTER    0x04  /* 0b0100 */
#define PATA_RW_SECTOR_NUMBER_REGISTER   0x06  /* 0b0110 */
#define PATA_RW_CYLINDER_LOW_REGISTER    0x08  /* 0b1000 */
#define PATA_RW_CYLINDER_HIGH_REGISTER   0x0A  /* 0b1010 */
#define PATA_RW_DEVICE_HEAD_REGISTER     0x0C  /* 0b1100 */
#define PATA_R_STATUS_REGISTER           0x0E  /* 0b1110 */
#define PATA_W_COMMAND_REGISTER          0x0E  /* 0b1110 */

/* Control Block Registers */
#define PATA_W_DEVICE_CONTROL_REGISTER   0x0D  /* 0b1101 */
#define PATA_R_ALTERNATE_STATUS_REGISTER 0x0D  /* 0b1101 */

typeDriveInfo ataDriveInfo;
uint8_t p8000_info_restore;

/*
 * Private functions
 */

void deactivate_p8000com()
{
	disable_p8000com();
	p8000_info_restore = PORT_INFO;
/*
	configure_ata_data_read();
	port_ata_data_8l_set(0x00);
	port_ata_data_8h_set(0x00);
*/
}

void activate_p8000com()
{
	ata_wr_disable();
	ata_rd_disable();
	wdc_config_p8000_ports();
	PORT_INFO = p8000_info_restore;
	enable_p8000com();
}

uint8_t pata_rdy ()
{
    return ( read_io_register ( PATA_R_STATUS_REGISTER ) & ATA_STAT_RDY ) ? 1 : 0;
}

uint8_t pata_bsy ()
{
    return ( read_io_register ( PATA_R_STATUS_REGISTER ) & ATA_STAT_BSY ) ? 1 : 0;
}

uint8_t pata_drq ()
{
    return ( read_io_register ( PATA_R_STATUS_REGISTER ) & ATA_STAT_DRQ ) ? 1 : 0;
}

uint8_t pata_err ()
{
    if ( read_io_register ( PATA_R_STATUS_REGISTER ) & ATA_STAT_ERR ) {
        return read_io_register ( PATA_R_ERROR_REGISTER );
    }
    return 0;
}

void set_io_register ( uint8_t ioreg )
{
    /* set the requested signals */
 	PORT_ATADA = PORT_ATADA & 0xf0; 		    /* Clear previous signals (lower 4 bits of PORT_ATADA) */
  	PORT_ATADA = PORT_ATADA | (ioreg & 0x0f); 	/* Assert the signal Lines (lower 4 bits of PORT_ATADA) */
}

uint8_t read_io_register ( uint8_t ioreg )
{
    uint8_t byte;

    configure_ata_data_read();

    set_io_register ( ioreg );
    ata_rd_enable();
    nop();
    nop();
    byte = port_ata_data_8l_get();
    ata_rd_disable();

    return byte;
}

void write_io_register ( uint8_t ioreg, uint8_t byte )
{
    configure_ata_data_write();

    set_io_register ( ioreg );
    port_ata_data_8l_set ( byte );
    ata_wr_enable();
    nop();
    nop();
    ata_wr_disable();}

void pata_read_bytes ( uint8_t *buffer, uint8_t numblocks )
{
    uint8_t i = 0;

    configure_ata_data_read();

    do {
        while ( pata_bsy() ) {}
        if ( pata_err() ) {
            return;
        }
        while ( !pata_drq() ) {}

        set_io_register ( PATA_RW_DATA_REGISTER );

         do {
			ata_rd_enable();
			nop();
			nop();
            *buffer++ = port_ata_data_8l_get();
            *buffer++ = port_ata_data_8h_get();
		    ata_rd_disable();
       } while ( ++i != 0 );  /* overflow, 512 bytes read */
    } while ( --numblocks );
}

void pata_write_bytes ( uint8_t *buffer, uint8_t numblocks )
{
    uint8_t i = 0;

    do {
        while ( pata_bsy() ) {}
        if ( pata_err() ) {
            return;
        }
        while ( !pata_drq() ) {}

        configure_ata_data_write();

        set_io_register ( PATA_RW_DATA_REGISTER );

        do {
            port_ata_data_8l_set ( *buffer++ );
			port_ata_data_8h_set ( *buffer++ );
		    ata_wr_enable();
			nop();
			nop();
			ata_wr_disable();
        } while ( ++i != 0 );  /* overflow, 512 bytes written */
    } while ( --numblocks );
}

uint8_t ata_identify ()
{
    uint8_t  buffer[512], i;
    uint16_t word[256], n;

    write_io_register ( PATA_W_COMMAND_REGISTER, CMD_IDENTIFY_DEVICE );

    pata_read_bytes ( buffer, 1 );

    i = 0;
    for ( n = 0; n < 512; n += 2 ) {
        word[i] = ( ( buffer[n + 1] << 8 ) | buffer[n] );
        i++;
    }

    /* check if data is nonsense */
    if ( word[1] == word[3] ) {
        return 1;
    }

 
    ataDriveInfo.cylinders = word[1];
    ataDriveInfo.heads = word[3];
    ataDriveInfo.sectors = word[6];
    ataDriveInfo.LBAsupport = word[53] & ( 1 << 0 );
    ataDriveInfo.sizeinsectors = word[60] | (uint32_t)word[61] << 16;

    if ( !ataDriveInfo.LBAsupport ) {
        ataDriveInfo.sizeinsectors = ataDriveInfo.cylinders * ataDriveInfo.heads * ataDriveInfo.sectors;
    }

    uart_putstring ( PSTR ( "INFO: PATA disk has been found" ), true );
    uart_putstring ( PSTR ( "INFO: Number of logical cylinders: " ), false );
    uart_putw_dec ( word[1] );
    uart_put_nl();
    uart_putstring ( PSTR ( "INFO: Number of logical heads: " ), false );
    uart_putw_dec ( word[3] );
    uart_put_nl();
    uart_putstring ( PSTR ( "INFO: Number of logical sectors per logical track: " ), false );
    uart_putw_dec ( word[6] );
    uart_put_nl();
    uart_putstring ( PSTR ( "INFO: Serial number: " ), false );
    for ( i = 10; i <= 19; i++ ) {
        uart_putc ( word[i] >> 8 );
        uart_putc ( word[i] & 0x00FF );
    }
    uart_put_nl();
    uart_putstring ( PSTR ( "INFO: Firmware revision: " ), false );
    for ( i = 23; i <= 26; i++ ) {
        uart_putc ( word[i] >> 8 );
        uart_putc ( word[i] & 0x00FF );
    }
    uart_put_nl();
    uart_putstring ( PSTR ( "INFO: Model number: " ), false );
    for ( i = 27; i <= 46; i++ ) {
        uart_putc ( word[i] >> 8 );
        uart_putc ( word[i] & 0x00FF );
    }
    uart_put_nl();
    uart_putstring ( PSTR ( "INFO: Capabilities: " ), false );
    if ( word[49] & ( 1 << 8 )) {
        uart_putstring ( PSTR ( "DMA, " ), false );
    }
    if ( word[49] & ( 1 << 9 )) {
        uart_putstring ( PSTR ( "LBA, " ), false );
    }
    if ( word[49] & ( 1 << 10 )) {
        uart_putstring ( PSTR ( "IORDY may be disabled, " ), false );
    }
    if ( word[49] & ( 1 << 11 )) {
        uart_putstring ( PSTR ( "IORDY, " ), false );
    }
    if ( word[49] & ( 1 << 13 )) {
        uart_putstring ( PSTR ( "Standard standby timer values, " ), false );
    }
    uart_put_nl();

    if ( word[53] & ( 1 << 0 )) {
        uart_putstring ( PSTR ( "INFO: Number of current logical cylinders: " ), false );
        uart_putw_dec ( word[54] );
        uart_put_nl();
        uart_putstring ( PSTR ( "INFO: Number of current logical heads: " ), false );
        uart_putw_dec ( word[55] );
        uart_put_nl();
        uart_putstring ( PSTR ( "INFO: Number of current logical sectors per track: " ), false );
        uart_putw_dec ( word[56] );
        uart_put_nl();
        uart_putstring ( PSTR ( "INFO: Current capacity in sectors: " ), false );
        uart_putdw_dec ( word[57] | (uint32_t)word[58] << 16 );
        uart_put_nl();
    }
    if ( word[53] & ( 1 << 1 )) {
        uart_putstring ( PSTR ( "INFO: User addressable sectors for 28-bit commands: " ), false );
        uart_putdw_dec ( word[60] | (uint32_t)word[61] << 16 );
        uart_put_nl();
        uart_putstring ( PSTR ( "Single Word DMA modes: " ), false );
        uart_putw_dec ( word[62] );
        uart_put_nl();
        uart_putstring ( PSTR ( "Multiword Word DMA modes: " ), false );
        uart_putw_dec ( word[63] );
        uart_put_nl();
        uart_putstring ( PSTR ( "PIO modes: " ), false );
        uart_putw_dec ( word[64] );
        uart_put_nl();
        uart_putstring ( PSTR ( "INFO: Minimum Multiword DMA cycle time per word: " ), false );
        uart_putw_dec ( word[65] );
        uart_putstring ( PSTR ( "ns" ), true );
        uart_putstring ( PSTR ( "INFO: Recommended Multiword DMA cycle time: " ), false );
        uart_putw_dec ( word[66] );
        uart_putstring ( PSTR ( "ns" ), true );
        uart_putstring ( PSTR ( "INFO: Minimum PIO transfer cycle time without flow control: " ), false );
        uart_putw_dec ( word[67] );
        uart_putstring ( PSTR ( "ns" ), true );
        uart_putstring ( PSTR ( "INFO: Minimum PIO cycle time with IORDY flow control: " ), false );
        uart_putw_dec ( word[68] );
        uart_putstring ( PSTR ( "ns" ), true );
        uart_putstring ( PSTR ( "INFO: Additional support: " ), false );
        uart_putw_dec ( word[69] );
        uart_put_nl();
    }
    if ( word[53] & ( 1 << 2 )) {
        uart_putstring ( PSTR ( "INFO: Ultra DMA modes: " ), false );
        uart_putw_dec ( word[88] );
        uart_put_nl();
    }

    return 0;
}

void pata_rw_command ( uint32_t addr, uint8_t numblocks, uint8_t cmd )
{
    typedef union {
        uint32_t value32;
        struct {
            uint8_t ll;
            uint8_t lh;
            uint8_t hl;
            uint8_t hh;
        } value8;
    } sint32;
   typedef union {
	   uint16_t value16;
	   struct {
		   uint8_t l;
		   uint8_t h;
	   } value8;
   } sint16;

    sint32  startblock;
    uint8_t sector, cyllow, cylhigh, head;
    sint16  cylinder;

    if ( ataDriveInfo.LBAsupport ) {
        startblock.value32 = addr;
        sector = startblock.value8.ll;
        cyllow = startblock.value8.lh;
        cylhigh = startblock.value8.hl;
        head = ( startblock.value8.hh & 0x0F ) | ATA_LBA_DRIVE_0;
    } else {
        sector = ( addr % ataDriveInfo.sectors ) + 1;
        addr = addr / ataDriveInfo.sectors;
        head = ATA_CHS_DRIVE_0 + ( addr % ataDriveInfo.heads );
        addr = addr / ataDriveInfo.heads;
        cylinder.value16 = addr;
        cylhigh = cylinder.value8.h;
        cyllow = cylinder.value8.l;
    }

    while ( pata_bsy() ) {}
    while ( pata_drq() ) {}

    write_io_register ( PATA_RW_DEVICE_HEAD_REGISTER, head );
    write_io_register ( PATA_RW_SECTOR_COUNT_REGISTER, numblocks );
    write_io_register ( PATA_RW_SECTOR_NUMBER_REGISTER, sector );
    write_io_register ( PATA_RW_CYLINDER_LOW_REGISTER, cyllow );
    write_io_register ( PATA_RW_CYLINDER_HIGH_REGISTER, cylhigh );

    write_io_register ( PATA_W_COMMAND_REGISTER, cmd );

}

/*
 * Public functions
 */

uint8_t pata_init ()
{
    uint8_t wait_for_drive = 50;
	uint8_t ret;

	deactivate_p8000com();

    uart_putstring ( PSTR ( "INFO: PATA init start" ), true );
    while ( !pata_rdy() ) {
        if ( --wait_for_drive == 0 ) {
            ret=1;
			goto out;
        }
        _delay_ms ( 500 );
    }
    while ( pata_bsy() ) {}

    write_io_register ( PATA_RW_DEVICE_HEAD_REGISTER, ATA_CHS_DRIVE_0 );

    ret = ata_identify();
	
out:
	activate_p8000com();
	return ret;
}

uint8_t pata_read_block ( uint32_t addr, uint8_t *buffer )
{
    return pata_read_multiblock ( addr, buffer, 1 );
}

uint8_t pata_write_block ( uint32_t addr, uint8_t *buffer )
{
    return pata_write_multiblock ( addr, buffer, 1 );
}

uint8_t pata_read_multiblock ( uint32_t addr, uint8_t *buffer, uint8_t numblocks )
{
	uint8_t ret;

	deactivate_p8000com();
	
    pata_rw_command ( addr, numblocks, CMD_READ_SECTORS );

    pata_read_bytes ( buffer, numblocks );

	ret = pata_err();

	activate_p8000com();

	return ret;
}

uint8_t pata_write_multiblock ( uint32_t addr, uint8_t *buffer, uint8_t numblocks )
{
	uint8_t ret;

	deactivate_p8000com();

    pata_rw_command ( addr, numblocks, CMD_WRITE_SECTORS );

    pata_write_bytes ( buffer, numblocks );

    /* wait until drive completed write */
    while ( pata_bsy() ) {}

	ret = pata_err();

	activate_p8000com();

    return ret;
}