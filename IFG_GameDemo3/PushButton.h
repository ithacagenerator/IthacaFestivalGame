#ifndef ___IFG_PUSHBUTTON_H___
#define ___IFG_PUSHBUTTON_H___

#include <Arduino.h>
#include <stdint.h>

void PushButton_Init(void);
boolean button_is_pressed(void);
void wait_for_button_released(void);



#endif
