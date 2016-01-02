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
#include <stdbool.h>
#include <avr/io.h>
#include <util/delay.h>
#include "wdc_avr.h"
#include "wdc_if_p8000.h"
#include "wdc_par.h"

void wdc_wait_for_reset ()
{
    wdc_set_initialized();

    while ( info_rst_is_high() ) {}
}

static bool wdc_read_data_from_p8k ( uint8_t *buffer, uint16_t count, uint8_t wdc_status )
{
    set_status ( wdc_status );

    configure_port_data_read();

    while ( !info_te_is_high() ) {
        if ( info_rst_is_high() ) {
            return false;
        }
    }

    while ( !info_wardy_is_high() ) {}

    do {
        while ( info_wardy_is_high() ) {}
        assert_astb();
        *buffer++ = port_data_get();
        deassert_astb();
        while ( !info_wardy_is_high() ) {}
    } while ( --count > 0 );

    reset_status();

    return true;
}

static void wdc_write_data_to_p8k ( uint8_t *buffer, uint16_t count, uint8_t wdc_status )
{
    assert_tr();
    set_status ( wdc_status );

    while ( info_te_is_high() ) {}

    configure_port_data_write();

    while ( !info_wardy_is_high() ) {}

    do {
        port_data_set ( *buffer++ );
        while ( info_wardy_is_high() ) {}
        assert_astb();
        nop();
        nop();
        nop();
        deassert_astb();
        while ( !info_wardy_is_high() ) {}
    } while ( --count > 0 );

    /* toggle /ASB once more */
    while ( info_wardy_is_high() ) {}
    assert_astb();
    nop();
    nop();
    nop();
    deassert_astb();
    while ( !info_wardy_is_high() ) {}

    deassert_tr();
    reset_status();
}

bool wdc_receive_cmd ( uint8_t *buffer )
{
    /* needed for at least sa.format when issuing command 0x28, 0x58 */
    _delay_us ( 100 );

    return wdc_read_data_from_p8k ( buffer
                                  , 9
                                  , INFO_STAT_GCMD
                                  );
}

void wdc_receive_data ( uint8_t *buffer, uint16_t count )
{
    wdc_read_data_from_p8k ( buffer
                           , count
                           , INFO_STAT_RDATA
                           );
}

void wdc_send_data ( uint8_t *buffer, uint16_t count )
{
    wdc_write_data_to_p8k ( buffer
                          , count
                          , INFO_STAT_WDATA
                          );
}

void wdc_send_error ()
{
    wdc_send_errorcode ( 0x01 );
}

void wdc_send_errorcode ( uint8_t error )
{
    uint8_t buffer[1];

    buffer[0] = error;

    wdc_write_data_to_p8k ( buffer
                          , 1
                          , INFO_STAT_ERROR
                          );
}