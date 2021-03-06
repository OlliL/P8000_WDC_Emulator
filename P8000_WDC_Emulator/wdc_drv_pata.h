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

#ifndef WDC_DRV_PATA_H_
#define WDC_DRV_PATA_H_

extern uint8_t pata_init ();

extern uint8_t pata_read_block ( uint32_t addr, uint8_t *buffer );
extern uint8_t pata_write_block ( uint32_t addr, uint8_t *buffer );

extern uint8_t pata_read_multiblock ( uint32_t addr, uint8_t *buffer, uint8_t numblocks );
extern uint8_t pata_write_multiblock ( uint32_t addr, uint8_t *buffer, uint8_t numblocks );

#define configure_ata_data_read()   DDR_ATADATA8L = 0x00; DDR_ATADATA8H = 0x00
#define configure_ata_data_write()  DDR_ATADATA8L = 0xff; DDR_ATADATA8H = 0xff
#define port_ata_data_8l_set( x )   ( PORT_ATADATA8L = ( x ) )
#define port_ata_data_8l_get()      PIN_ATADATA8L
#define port_ata_data_8h_set( x )   ( PORT_ATADATA8H = ( x ) )
#define port_ata_data_8h_get()      PIN_ATADATA8H

#define ATA_STAT_BSY  0x80  /* ATA busy         */
#define ATA_STAT_RDY  0x40  /* ATA ready        */
#define ATA_STAT_DRQ  0x08  /* ATA data request */
#define ATA_STAT_ERR  0x01  /* ATA error        */

#define ATA_LBA_DRIVE_0 0xE0
#define ATA_CHS_DRIVE_0 0xA0

/* implemented ATA commands */
#define CMD_RECALIBRATE                  0x10
#define CMD_READ_SECTORS                 0x20
#define CMD_WRITE_SECTORS                0x30
#define CMD_INITIALIZE_DRIVE             0x91
#define CMD_IDENTIFY_DEVICE              0xEC

#define PATA_BLOCKLEN 512

#endif /* WDC_DRV_PATA_H_ */