/*-
 * Copyright (c) 2012, 2013, 2015 Oliver Lehmann
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

#ifndef WDC_AVR_H_
#define WDC_AVR_H_

#define configure_port_data_read()  DDR_DATA = 0x00                /* programs the DATA-Port as Input-only for reading from the P8000*/
#define configure_port_data_write() DDR_DATA = 0xff                /* programs the DATA-Port as Out-only for writing to the P8000*/

#define configure_pin_status0()     DDR_INFO |= ( 1 << PIN_INFO_STATUS0 )
#define configure_pin_status1()     DDR_INFO |= ( 1 << PIN_INFO_STATUS1 )
#define configure_pin_status2()     DDR_INFO |= ( 1 << PIN_INFO_STATUS2 )
#define configure_pin_tr()          DDR_INFO |= ( 1 << PIN_INFO_TR )
#define configure_pin_astb()        DDR_INFO |= ( 1 << PIN_INFO_ASTB )
#define configure_pin_te()          DDR_INFO &= ~( 1 << PIN_INFO_TE )
#define configure_pin_wdardy()      DDR_INFO &= ~( 1 << PIN_INFO_WDARDY )
#define configure_pin_reset()       DDR_INFO &= ~( 1 << PIN_INFO_RST ); PORT_INFO |= ( 1 << PIN_INFO_RST )
#define configure_p8000_com()       DDR_P8000_ACT |= ( 1 << PIN_P8000_COM )

#define assert_astb()               PORT_INFO &= ~( 1 << PIN_INFO_ASTB )
#define deassert_astb()             PORT_INFO |= ( 1 << PIN_INFO_ASTB )
#define assert_tr()                 PORT_INFO &= ~( 1 << PIN_INFO_TR )
#define deassert_tr()               PORT_INFO |= ( 1 << PIN_INFO_TR )

#define enable_p8000com()           PORT_P8000_ACT &= ~( 1 << PIN_P8000_COM )
#define disable_p8000com()          PORT_P8000_ACT |= ( 1 << PIN_P8000_COM )

/* jumper */
#define configure_jp_mmc_pata()     DDR_JUMPER &= ~( 1 << PIN_JP_MMC_PATA )
#define jp_mmc_pata_set()           (( PIN_JUMPER & ( 1 << PIN_JP_MMC_PATA )) > 0 )

/* led */
#define configure_pin_led1()        DDR_LED |= ( 1 << PIN_INFO_LED1 )
#define configure_pin_led2()        DDR_LED |= ( 1 << PIN_INFO_LED2 )

/* functions dealing with the MMC interface */
#define configure_pin_miso()        DDR_MMC &= ~( 1 << PIN_MMC_MISO )
#define configure_pin_sck()         DDR_MMC |= ( 1 << PIN_MMC_SCK )
#define configure_pin_mosi()        DDR_MMC |= ( 1 << PIN_MMC_MOSI )
#define configure_pin_mmc_cs()      DDR_MMC |= ( 1 << PIN_MMC_CS )

#define disable_mmc()               PORT_MMC |= ( 1 << PIN_MMC_CS );
#define enable_mmc()                PORT_MMC &= ~( 1 << PIN_MMC_CS );

/* functions dealing with the ATA interface */
#define configure_ata_wr()          DDR_ATARDWR |= ( 1 << PIN_ATARDWR_WR )
#define configure_ata_rd()          DDR_ATARDWR |= ( 1 << PIN_ATARDWR_RD )
#define configure_ata_cs0()         DDR_ATACS |= ( 1 << PIN_ATACS_CS0 )
#define configure_ata_da0()         DDR_ATADA |= ( 1 << PIN_ATADA_DA0 )
#define configure_ata_da1()         DDR_ATADA |= ( 1 << PIN_ATADA_DA1 )
#define configure_ata_da2()         DDR_ATADA |= ( 1 << PIN_ATADA_DA2 )

#define ata_wr_disable()            PORT_ATARDWR |= ( 1 << PIN_ATARDWR_WR )
#define ata_wr_enable()             PORT_ATARDWR &= ~( 1 << PIN_ATARDWR_WR )
#define ata_rd_disable()            PORT_ATARDWR |= ( 1 << PIN_ATARDWR_RD )
#define ata_rd_enable()             PORT_ATARDWR &= ~( 1 << PIN_ATARDWR_RD )

extern void wdc_init_avr ();
extern void wdc_get_sysconf ();
extern void wdc_config_p8000_ports ();

extern void wdc_led_bootup ();
extern void wdc_led_no_disk ();
extern void wdc_led_invalid_disk ();
extern void wdc_led_all_ok ();

#endif /* WDC_AVR_H_ */