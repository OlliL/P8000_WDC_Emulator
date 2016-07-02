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

static inline bool is_rst_asserted ()
{
    return info_rst_is_high() && info_rst_is_high();
}

static inline bool is_ardy_asserted ()
{
    return !info_wardy_is_high() && !info_wardy_is_high();
}

static inline bool is_ardy_deasserted ()
{
    return info_wardy_is_high() && info_wardy_is_high();
}

static inline bool is_te_asserted ()
{
    return !info_te_is_high() && !info_te_is_high();
}

static inline bool is_te_deasserted ()
{
    return info_te_is_high() && info_te_is_high();
}

static inline void wait_until_ardy_asserted ()
{
    do {
        if ( is_ardy_asserted() ) {
            return;
        }
    } while ( true );
}

static inline void wait_until_ardy_deasserted ()
{
    do {
        if ( is_ardy_deasserted() ) {
            return;
        }
    } while ( true );
}

static bool wdc_read_data_from_p8k ( uint8_t *buffer, uint16_t count, uint8_t wdc_status )
{
    if ( is_rst_asserted() ) {
        goto reset;
    }

    set_status ( wdc_status );

    configure_port_data_read();

    /* TE and ARDY have to be checked sepperatly to detect a small RST flank savely. */
    /* If both are checked together, the RST check could miss small RST flanks. */
    while ( is_te_asserted() ) {
        if ( is_rst_asserted() ) {
            goto reset;
        }
    }

    while ( is_ardy_deasserted() ) {
        if ( is_rst_asserted() ) {
            goto reset;
        }
    }

    do {
        wait_until_ardy_asserted();
        assert_astb();
        *buffer++ = port_data_get();
        deassert_astb();
        wait_until_ardy_deasserted();
    } while ( --count > 0 );

    reset_status();
    return true;

reset:
    wdc_set_initialized();
    reset_status();
    return false;
}

static void wdc_write_data_to_p8k ( uint8_t *buffer, uint16_t count, uint8_t wdc_status )
{
    assert_tr();
    set_status ( wdc_status );

    while ( is_te_deasserted() ) {}

    configure_port_data_write();

    wait_until_ardy_deasserted();

    do {
        port_data_set ( *buffer++ );
        wait_until_ardy_asserted();
        assert_astb();
        nop();
        nop();
        nop();
        deassert_astb();
        wait_until_ardy_deasserted();
    } while ( --count > 0 );

    /* toggle /ASB once more */
    wait_until_ardy_asserted();
    assert_astb();
    nop();
    nop();
    nop();
    deassert_astb();
    wait_until_ardy_deasserted();

    deassert_tr();
    reset_status();
}

bool wdc_receive_cmd ( uint8_t *buffer )
{
    /* needed for at least sa.format when issuing command 0x28, 0x58 */
    _delay_us ( 200 );

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

    /* It is important to wait some time until the error is sent back because otherwise some errors */
    /* would be send back to the Host too fast after the cmd was received. This would result in freezing*/
    /* the communication (detected with sa.diags with an unformatted disk attached) */
    _delay_ms ( 70 );

    wdc_write_data_to_p8k ( buffer
                          , 1
                          , INFO_STAT_ERROR
                          );
}