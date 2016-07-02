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

#ifndef WDC_PAR_H_
#define WDC_PAR_H_

extern void     wdc_read_par_table ( uint8_t *buffer, uint16_t count );
extern void     wdc_write_par_table ( uint8_t *buffer, uint16_t count );

extern void     wdc_read_wdc_par ( uint8_t *buffer, uint16_t count );
extern void     wdc_write_wdc_par ( uint8_t *buffer, uint16_t count );

extern void     wdc_del_wdc_btt ();
extern void     wdc_read_wdc_btt ( uint8_t *buffer, uint16_t count );
extern void     wdc_write_wdc_btt ( uint8_t *buffer, uint16_t count );
extern uint8_t  wdc_add_btt_entry ( uint16_t cylinder, uint8_t head );
extern uint8_t  wdc_get_hdd_sectors ();
extern uint8_t  wdc_get_hdd_heads ();
extern uint16_t wdc_get_hdd_cylinder ();
extern uint32_t wdc_get_hdd_blocks ();
extern void     wdc_set_disk_invalid ();
extern void     wdc_set_disk_valid ();
extern bool     wdc_get_disk_valid ();
extern void     wdc_set_no_disk ();
extern uint8_t  wdc_get_num_of_drvs ();
extern void     wdc_unset_initialized ();
extern void     wdc_set_initialized ();
extern bool     wdc_get_initialized ();
extern bool     wdc_get_btt_cleared ();

extern uint8_t  wdc_validate_blockno ( uint32_t blockno );
extern uint8_t  wdc_validate_cylhead ( uint16_t req_cylinder, uint8_t req_head, uint32_t blockno );


#define POS_PAR_VERSION     0           /* defines at which position in the PAR table the WDC version string starts */
#define POS_PAR_WDC_PAR     8           /* defines at which position in the PAR table the WDC parameter section starts */
#define POS_PAR_WDC_BTT     0x88        /* defines at which position in the PAR table the BTT section starts */

#define SIZE_PAR_VERSION    8           /* defines the length of the WDC version string */
#define SIZE_PAR_WDC_PAR    0x80        /* defines the length of the WDC parameter section */
#define SIZE_PAR_WDC_BTT    125         /* defines the length of the BTT section */

#endif /* WDC_PAR_H_ */