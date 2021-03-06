/*-
 * Copyright (c) 2012, 2013, 2015, 2016 Oliver Lehmann
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
#include <stdint.h>
#include <stdbool.h>
#include <avr/pgmspace.h>
#include "wdc_if_disk.h"
#include "wdc_drv_mmc.h"
#include "wdc_drv_pata.h"
#include "uart.h"
#include "wdc_par.h"

uint8_t wdc_init_disk ()
{
    return ( *drv_init )();
}

uint32_t wdc_sector2diskblock ( uint16_t req_cylinder, uint8_t req_head, uint8_t req_sector )
{
    uint8_t hdd_sectors = wdc_get_hdd_sectors();

    return ( req_cylinder * wdc_get_hdd_heads() * hdd_sectors )
           + ( req_head * hdd_sectors )
           + ( req_sector - 1 );
}

/************************************************************************/
/* Always ignore the PAR+BTT on the disk (first head * sectors).        */
/************************************************************************/
uint32_t wdc_p8kblock2diskblock ( uint32_t blockno )
{
    return blockno
           + ( wdc_get_hdd_heads() * wdc_get_hdd_sectors() );
}

uint8_t wdc_write_sector ( uint32_t addr, uint8_t *sector )
{
    uint8_t errorcode;

    errorcode = drv_write_block ( addr, sector );

    if ( errorcode > 0 ) {
        uart_putstring ( PSTR ( "ERROR: write error at address: " ), false );
        uart_putdw_dec ( addr );
        uart_putstring ( PSTR ( " / errorcode is: " ), false );
        uart_putc_hex ( errorcode );
        uart_put_nl();
        return errorcode;
    } else {
        return 0;
    }
}

uint8_t wdc_read_sector ( uint32_t addr, uint8_t *sector )
{
    uint8_t errorcode;

    errorcode = drv_read_block ( addr, sector );

    if ( errorcode > 0 ) {
        uart_putstring ( PSTR ( "ERROR: read error at address: " ), false );
        uart_putdw_dec ( addr );
        uart_putstring ( PSTR ( " / errorcode is: " ), false );
        uart_putc_hex ( errorcode );
        uart_put_nl();
        return errorcode;
    } else {
        return 0;
    }
}

uint8_t wdc_read_multiblock ( uint32_t addr, uint8_t *sector, uint8_t numblocks )
{
    uint8_t errorcode;

    errorcode = drv_read_multiblock ( addr, sector, numblocks );

    if ( errorcode > 0 ) {
        uart_putstring ( PSTR ( "ERROR: read error at address: " ), false );
        uart_putdw_dec ( addr );
        uart_putstring ( PSTR ( " / errorcode is: " ), false );
        uart_putc_hex ( errorcode );
        uart_put_nl();
        return errorcode;
    } else {
        return 0;
    }
}

uint8_t wdc_write_multiblock ( uint32_t addr, uint8_t *sector, uint8_t numblocks )
{
    uint8_t errorcode;

    errorcode = drv_write_multiblock ( addr, sector, numblocks );

    if ( errorcode > 0 ) {
        uart_putstring ( PSTR ( "ERROR: write error at address: " ), false );
        uart_putdw_dec ( addr );
        uart_putstring ( PSTR ( " / errorcode is: " ), false );
        uart_putc_hex ( errorcode );
        uart_put_nl();
        return errorcode;
    } else {
        return 0;
    }
}