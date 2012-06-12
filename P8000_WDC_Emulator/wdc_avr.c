/*-
 * Copyright (c) 2012 Oliver Lehmann
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
 * $Id: wdc_avr.c,v 1.1 2012/06/12 17:34:14 olivleh1 Exp $
 */


#include <avr/io.h>
#include "wdc_config.h"
#include "wdc_avr.h"

void wdc_init_avr()
{
    /* configure the P8000 interface */
    configure_pin_status0();
    configure_pin_status1();
    configure_pin_status2();
    configure_pin_te();
    configure_pin_wdardy();
    configure_pin_tr();
    configure_pin_reset();

    configure_port_data_read();

    /* configure the address decoder */
    configure_pin_addr0();
    configure_pin_addr1();
    configure_pin_addr2();

    init_addressdecoder();

    /* configure the MMC interface */
    configure_pin_miso();
    configure_pin_sck();
    configure_pin_mosi();
    configure_pin_mmc_cs();
}