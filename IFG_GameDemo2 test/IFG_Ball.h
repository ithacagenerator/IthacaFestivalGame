#ifndef ___IFG_BALL_H___
#define ___IFG_BALL_H___

#include <Arduino.h>
#include <stdint.h>

boolean Ball_in_possession(void);
void Ball_possess(void);
void Ball_release(void);
unsigned long Ball_possession_duration_ms(void);
boolean Ball_dropped(void);

#endif
