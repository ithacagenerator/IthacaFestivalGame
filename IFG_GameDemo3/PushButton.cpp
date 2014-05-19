#include <Arduino.h>
#include "PushButton.h"
#include "IFG_Utility.h"

#if defined(__USING_ATMEGA328__)
#define PUSHBUTTON_PIN 8
#elif defined(__USING_ATTINY84__)
#define PUSHBUTTON_PIN 4
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

void wait_for_button_released(void){
   while(digitalRead(PUSHBUTTON_PIN) == 0){
     // spin
   }   
}
