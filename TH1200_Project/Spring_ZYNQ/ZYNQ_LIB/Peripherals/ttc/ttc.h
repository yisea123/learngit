#ifndef __TTC_H__
#define __TTC_H__

#ifdef __cplusplus
extern "C" {
#endif


#include "adt_datatype.h"



int ttc_init(int ttc, int part);
int ttc_reset(int ttc, int part);
int ttc_enable(int ttc, int part);
int ttc_disable(int ttc, int part);
int ttc_prescale_enable(int ttc, int part);
int ttc_prescale_disable(int ttc, int part);
int ttc_prescale_value(int ttc, int part, u8 value);
int ttc_match_enable(int ttc, int part);
int ttc_match_disable(int ttc, int part);
int ttc_set_match(int ttc, int part, int match, u16 count);
int ttc_set_interval(int ttc, int part, u16 count);
int ttc_set_mode(int ttc, int part, int mode);
int ttc_intr_enable(int ttc, int part, u8 mask);
int ttc_intr_disable(int ttc, int part);
int ttc_irq_init(int ttc, int part, void (*irq_handle)(void *p_arg, u32 cpu_id));
int ttc_irq_clear(int ttc, int part);


#ifdef __cplusplus
}
#endif


#endif
