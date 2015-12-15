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

void init_p8000_ports() {
    disable_p8000com();
	
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
	
    reset_info();
	port_data_set ( DATA_CLEAR );
    
    /* activate P8000 communication */
    enable_p8000com();

}
void wdc_wait_for_reset ()
{
    init_p8000_ports();

    wdc_set_initialized ( 1 );
uart_putstring(PSTR("waiting for reset"),true);
    while ( isset_info_reset() ) {}
uart_putstring(PSTR("reset asserted"),true);
}

uint8_t wdc_read_data_from_p8k ( uint8_t *buffer, uint16_t count, uint8_t wdc_status )
{
    _delay_us ( DELAY_PIO_US );
/*    init_p8000_ports(); */
    set_info ( wdc_status );
    while ( !isset_info_te() ) {
        if ( isset_info_reset() ) {
            return 0;
        }
    }

    while ( !isset_info_wdardy() ) {}

    do {
        while ( isset_info_wdardy() ) {}
        unset_info(INFO_ASTB);
        *buffer++ = port_data_get();
        set_info(INFO_ASTB);
        while ( !isset_info_wdardy() ) {}
        count--;
    } while ( count > 0 );

    reset_info();
    return 1;
}

void wdc_write_data_to_p8k ( uint8_t *buffer, uint16_t count, uint8_t wdc_status )
{
    _delay_us ( DELAY_PIO_US );
/*    init_p8000_ports(); */
	unset_info(INFO_TR);
    set_info ( wdc_status );

    while ( isset_info_te() ) {}

    configure_port_data_write();

    while ( !isset_info_wdardy() ) {}

    do {
        unset_info(INFO_ASTB);
        count--;
        while ( isset_info_wdardy() ) {}
        port_data_set ( *buffer++ );
        set_info(INFO_ASTB);
        while ( !isset_info_wdardy() ) {}
    } while ( count > 0 );

    while ( isset_info_wdardy() ) {}

    /* toggle /ASB once more */
    unset_info(INFO_ASTB);
    nop();
    set_info(INFO_ASTB);

    configure_port_data_read();

    while ( !isset_info_wdardy() ) {}

    reset_info();
}

uint8_t wdc_receive_cmd ( uint8_t *buffer )
{
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