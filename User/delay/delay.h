//#include "stm8s_type.h"
#ifndef _DELAY_H
#define _DELAY_H

//#define nop()                {_asm("nop\n");}    /* No Operation */
extern void Delay_MS(u16 time);
extern void Delay_US(void);
extern void Delay_nUS(u16 time);
void Delay(u16 j);
#endif


