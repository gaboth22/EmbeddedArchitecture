#ifndef UTILS_H
#define UTILS_H

#define DisableWatchdog() WDTCTL = WDTPW | WDTHOLD
#define SetClockTo16Mhz() \
    BCSCTL2 = SELM_0 + DIVM_0 + DIVS_0; \
    DCOCTL = 0x00; \
    BCSCTL1 = CALBC1_16MHZ; \
    DCOCTL = CALDCO_16MHZ; \
    BCSCTL1 |= XT2OFF + DIVA_0; \
    BCSCTL3 = XT2S_0 + LFXT1S_2 + XCAP_0; \
    __delay_cycles(250000) \

#define SetClockTo1Mhz() \
    BCSCTL2 = SELM_0 + DIVM_0 + DIVS_0; \
    DCOCTL = 0x00; \
    BCSCTL1 = CALBC1_1MHZ; \
    DCOCTL = CALDCO_1MHZ; \
    BCSCTL1 |= XT2OFF + DIVA_0; \
    BCSCTL3 = XT2S_0 + LFXT1S_2 + XCAP_0; \
    __delay_cycles(250000)\

#define BETWEEN(min, val, max) ((val >= min) && (val <= max))
#define TRUNCATE_UP(val, max) (val > max ? max : val)
#define TRUNCATE(min, val, max) TRUNCATE_UP(val < min ? min : val, max)
#define TRUNCATE_U16_SUBTRACTION(val1, val2) (val1 > val2 ? val1 - val2 : UINT16_MAX - (val2 - val1))
#define RECAST(new_name, cast_me, new_type) new_type new_name = (new_type) cast_me
#define IGNORE(something) (void)something
#define EnableInterrupts() __enable_interrupt()

#endif
