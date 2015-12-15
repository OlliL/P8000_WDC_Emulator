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
#include "wdc_if_p8000.h"
#include "wdc_par.h"
#include "uart.h"

void wdc_wait_for_reset ()
{
    wdc_set_initialized ( 1 );

    while ( isset_info_reset() ) {}
}

uint8_t wdc_read_data_from_p8k ( uint8_t *buffer, uint16_t count, uint8_t wdc_status )
{
    set_status ( wdc_status );

    while ( !isset_info_te() ) {
        if ( isset_info_reset() ) {
            return 0;
        }
    }

    configure_port_data_read();

    while ( !isset_info_wdardy() ) {}

    do {
		assert_astb();
        count--;
        while ( isset_info_wdardy() ) {}
        *buffer++ = port_data_get();
		deassert_astb();
        while ( !isset_info_wdardy() ) {}
    } while ( count > 0 );

    reset_status();
    return 1;
}

void wdc_write_data_to_p8k ( uint8_t *buffer, uint16_t count, uint8_t wdc_status )
{
	assert_tr();
    set_status ( wdc_status );

    while ( isset_info_te() ) {}

    configure_port_data_write();

    while ( !isset_info_wdardy() ) {}

    do {
        assert_astb();
        count--;
        while ( isset_info_wdardy() ) {}
        port_data_set ( *buffer++ );
        deassert_astb();
        while ( !isset_info_wdardy() ) {}
    } while ( count > 0 );

    while ( isset_info_wdardy() ) {}

    /* toggle /ASB once more */
    assert_astb();
    nop();
    deassert_astb();

    while ( !isset_info_wdardy() ) {}

	deassert_tr();
    reset_status();
}

uint8_t wdc_receive_cmd ( uint8_t *buffer )
{
    _delay_us ( DELAY_PIO_US );

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