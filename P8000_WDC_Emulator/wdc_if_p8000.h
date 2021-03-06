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

#ifndef WDC_IF_P8000_H_
#define WDC_IF_P8000_H_

/* input pin handling */
#define info_rst_is_high()          ((( PIN_INFO ) & ( 1 << PIN_INFO_RST )) != 0 )
#define info_te_is_high()             ((( PIN_INFO ) & ( 1 << PIN_INFO_TE )) != 0 )
#define info_wardy_is_high()         ((( PIN_INFO ) & ( 1 << PIN_INFO_WDARDY )) != 0 )

#define port_data_set( x )          ( PORT_DATA = ( x ) )
#define port_data_get()             PIN_DATA

/* output pin handling */

#define INFO_STAT_GCMD      (uint8_t)(( 1 << PIN_INFO_STATUS0 )                                                        )  /* 0x01 */
#define INFO_STAT_RDATA     (uint8_t)(                            ( 1 << PIN_INFO_STATUS1 )                            )  /* 0x02 */
#define INFO_STAT_WDATA     (uint8_t)(( 1 << PIN_INFO_STATUS0 ) | ( 1 << PIN_INFO_STATUS1 )                            )  /* 0x03 */
#define _INFO_STAT_CRCERR   (uint8_t)(                            ( 1 << PIN_INFO_STATUS1 ) | ( 1 << PIN_INFO_STATUS2 ))  /* 0x06 */
#define INFO_STAT_ERROR     (uint8_t)(( 1 << PIN_INFO_STATUS0 ) | ( 1 << PIN_INFO_STATUS1 ) | ( 1 << PIN_INFO_STATUS2 ))  /* 0x07 */

#define set_status( x )             ( PORT_INFO |= ( x ) )
#define reset_status()              PORT_INFO &= ~(( 1 << PIN_INFO_STATUS0 ) | ( 1 << PIN_INFO_STATUS1 ) | ( 1 << PIN_INFO_STATUS2 ))

extern bool wdc_receive_cmd ( uint8_t *buffer );
extern void wdc_receive_data ( uint8_t *buffer, uint16_t count );
extern void wdc_send_data ( uint8_t *buffer, uint16_t count );
extern void wdc_send_error ();
extern void wdc_send_errorcode ( uint8_t error );

#endif /* WDC_IF_P8000_H_ */