/*
 * wdc_types.h
 *
 * Created: 23.12.2015 18:01:01
 *  Author: olivleh1
 */


#ifndef WDC_TYPES_H_
#define WDC_TYPES_H_

typedef union {
    uint32_t value32;
    struct {
        uint8_t ll;
        uint8_t lh;
        uint8_t hl;
        uint8_t hh;
    } value8;
} sint32;

typedef union {
    uint16_t value16;
    struct {
        uint8_t l;
        uint8_t h;
    } value8;
} sint16;

#endif /* WDC_TYPES_H_ */