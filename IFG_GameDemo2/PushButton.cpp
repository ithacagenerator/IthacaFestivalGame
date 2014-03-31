#include <Arduino.h>
#include "PushButton.h"
#include "IFG_Utility.h"

#if defined(__USING_ATMEGA328__)
#define PUSHBUTTON_PIN 9
#elif defined(__USING_ATTINY84__)

#endif

void PushButton_Init(void){
  pinMode(PUSHBUTTON_PIN, INPUT_PULLUP);  
}

boolean button_is_pressed(void){
  if(digitalRead(PUSHBUTTON_PIN) == 0){
    return true; 
  }
  return false; 
}
