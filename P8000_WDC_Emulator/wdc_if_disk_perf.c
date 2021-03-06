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
#include <avr/io.h>
#include <avr/interrupt.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>
#include "wdc_if_disk.h"
#include "wdc_ram.h"
#include "uart.h"

#ifdef MEASURE_DISK_PERFORMANCE
#define BLOCKNO 409600

static uint8_t    data_buffer[4096];

volatile uint32_t overflow = 0;
ISR ( TIMER0_OVF_vect ) {
    overflow++;
}

/* switched from local to global for keeping an eye on memory usage */
extern uint8_t data_buffer[];
extern uint8_t cmd_buffer[];


void print_speed ( uint16_t size_in_kb, uint32_t overflow_count )
{
    uint32_t total_timer_ticks;
    float    new_frequency;
    float    real_time;
    uint16_t timer_resolution = 256; /* 8 Bit Timer */
    float    prescaler_freq = F_CPU / 8;
    char     speed[100];

    total_timer_ticks = timer_resolution * overflow_count;
    new_frequency = prescaler_freq / total_timer_ticks;
    real_time = 1 / new_frequency;

    sprintf ( speed, "%f KiB/sec", size_in_kb / real_time );
    uart_puts ( speed );
    uart_put_nl();
}

void test_write4k ( uint8_t numblocks, uint8_t nr_of_tests )
{
    uint32_t starttime;
    uint32_t blockno = 0;
    uint8_t  i8, errorcode;

    uart_putstring ( PSTR ( "Test Write 5000 512B-Blocks with Multiblock-Write (4KB Blocksize):" ), true );
    memset ( &data_buffer[0], 0xCC, WDC_BLOCKLEN * numblocks );

    for ( i8 = 0; i8 < nr_of_tests; i8++ ) {
        blockno = BLOCKNO;

        for ( starttime = overflow; blockno < ( (uint32_t)BLOCKNO + 5000 ); blockno += numblocks ) {
            errorcode = wdc_write_multiblock ( blockno, data_buffer, numblocks );

            if ( errorcode ) {
                break;
            }
        }

        if ( errorcode ) {
            break;
        }

        print_speed ( 2500, overflow - starttime );
    }
}

void test_read4k ( uint8_t numblocks, uint8_t nr_of_tests )
{
    uint32_t starttime;
    uint32_t blockno = 0;
    uint8_t  i8, errorcode;

    uart_putstring ( PSTR ( "Test Read 5000 512B-Blocks with Multiblock-Read (4KB Blocksize):" ), true );

    for ( i8 = 0; i8 < nr_of_tests; i8++ ) {
        blockno = BLOCKNO;

        for ( starttime = overflow; blockno < ( (uint32_t)BLOCKNO + 5000 ); blockno += numblocks ) {
            errorcode = wdc_read_multiblock ( blockno, data_buffer, numblocks );

            if ( errorcode ) {
                break;
            }
        }

        if ( errorcode ) {
            break;
        }

        print_speed ( 2500, overflow - starttime );
    }
}

void test_write512 ( uint8_t numblocks, uint8_t nr_of_tests )
{
    uint32_t starttime;
    uint32_t blockno = 0;
    uint8_t  i8, errorcode;

    uart_putstring ( PSTR ( "Test Write 5000 512B-Blocks with Singleblock-Write (512B Blocksize):" ), true );
    memset ( &data_buffer[0], 0xEF, WDC_BLOCKLEN * numblocks );

    for ( i8 = 0; i8 < nr_of_tests; i8++ ) {
        blockno = BLOCKNO;

        for ( starttime = overflow; blockno < ( (uint32_t)BLOCKNO + 5000 ); blockno++ ) {
            errorcode = wdc_write_sector ( blockno, data_buffer );

            if ( errorcode ) {
                break;
            }
        }

        if ( errorcode ) {
            break;
        }

        print_speed ( 2500, overflow - starttime );
    }
}

void test_read512 ( uint8_t numblocks, uint8_t nr_of_tests )
{
    uint32_t starttime;
    uint32_t blockno = 0;
    uint8_t  i8, errorcode;

    uart_putstring ( PSTR ( "Test Read 5000 512B-Blocks with Singleblock-Read (512B Blocksize):" ), true );

    for ( i8 = 0; i8 < nr_of_tests; i8++ ) {
        blockno = BLOCKNO;

        for ( starttime = overflow; blockno < ( (uint32_t)BLOCKNO + 5000 ); blockno++ ) {
            errorcode = wdc_read_sector ( blockno, data_buffer );

            if ( errorcode ) {
                break;
            }
        }

        if ( errorcode ) {
            break;
        }

        print_speed ( 2500, overflow - starttime );
    }
}

void measure_performance ()
{
    uint8_t numblocks = 4;
    uint8_t nr_of_tests = 2;

    sei();
    TIMSK0 |= ( 0x01 << TOIE0 );
    TCCR0B = ( 1 << CS01 ); /* Prescaler 8 */
    TCNT0 = 0x00;
    uart_putstring ( PSTR ( "INFO: Performance Measurement." ), true );
    test_read4k ( numblocks, nr_of_tests );
    test_write512 ( numblocks, nr_of_tests );
    test_read512 ( numblocks, nr_of_tests );
    test_write512 ( numblocks, nr_of_tests );
    test_read4k ( numblocks, nr_of_tests );
    test_write4k ( numblocks, nr_of_tests );
    test_read4k ( numblocks, nr_of_tests );
    test_write512 ( numblocks, nr_of_tests );
    test_write4k ( numblocks, nr_of_tests );
    test_read512 ( numblocks, nr_of_tests );
    test_read4k ( numblocks, nr_of_tests );
    test_read512 ( numblocks, nr_of_tests );
    test_write4k ( numblocks, nr_of_tests );
    test_write512 ( numblocks, nr_of_tests );
    uart_putstring ( PSTR ( "End" ), true );

    while ( 1 ) {}
}

#endif