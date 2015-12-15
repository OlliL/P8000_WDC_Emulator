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

/*
 * Fuses:
 *  - BODLEVEL 4V3
 *  - SPIEN
 *  - SUT_CKSEL EXTXOSC_8MHZ_XX_16KCK_65MS
 */

#include <avr/io.h>
#include "wdc_config.h"
#include "wdc_avr.h"
#include "wdc_drv_mmc.h"
#include "wdc_drv_pata.h"
#include "wdc_if_disk.h"

#ifdef _OFF
void wdc_init_led ()
{
    configure_pin_led();
}

void wdc_led_on ()
{
    PORT_INFO |= ( 1 << PIN_INFO_LED );
}

void wdc_led_off ()
{
    PORT_INFO &= ~( 1 << PIN_INFO_LED );
}

void wdc_led_toggle ()
{
    PORT_INFO ^= ( 1 << PIN_INFO_LED );
}

#endif

void wdc_config_p8000_ports ()
{
    /* configure the P8000 interface */
    configure_pin_status0();
    configure_pin_status1();
    configure_pin_status2();
    configure_pin_astb();
    configure_pin_tr();
    configure_pin_te();
    configure_pin_wdardy();
    configure_pin_reset();
    configure_port_data_read();
    configure_p8000_com();
}

void wdc_init_avr ()
{
    /* configure the P8000 interface */
    wdc_config_p8000_ports();
    deassert_astb();
    deassert_tr();

    enable_p8000com();

#ifdef _OFF
    /* configure the MMC interface */
    configure_pin_miso();
    configure_pin_sck();
    configure_pin_mosi();
    configure_pin_mmc_cs();
#endif
    /* configure the ATA interface */
    configure_ata_wr();
    configure_ata_rd();
    configure_ata_cs0();
    configure_ata_da0();
    configure_ata_da1();
    configure_ata_da2();

    ata_wr_disable();
    ata_rd_disable();
}

void wdc_get_sysconf ()
{
/*
    enable_sysconf();
    nop();
    nop();

    if ( jumper_pata_set() ) {
 */ drv_init = pata_init;
    drv_read_block = pata_read_block;
    drv_write_block = pata_write_block;
    drv_read_multiblock = pata_read_multiblock;
    drv_write_multiblock = pata_write_multiblock;
/*  } else {
        drv_init = mmc_init;
        drv_read_block = mmc_read_sector;
        drv_write_block = mmc_write_sector;
        drv_read_multiblock = mmc_read_multiblock;
        drv_write_multiblock = mmc_write_multiblock;
    }

    disable_sysconf();
 */
}