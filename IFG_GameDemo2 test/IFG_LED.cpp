#include <Arduino.h>
#include "IFG_LED.h"
#include "IFG_Utility.h"

#if defined(__USING_ATMEGA328__)
#define RED_PIN   10
#define GREEN_PIN  5
#define BLUE_PIN  12
#elif defined(__USING_ATTINY84__)

#endif

void LED_Init(void){
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT); 
 
  LED_Off();
}

void LED_Red(void){
  digitalWrite(RED_PIN, HIGH);
  digitalWrite(GREEN_PIN, LOW);
  digitalWrite(BLUE_PIN, LOW);  
}

void LED_White(void){
  digitalWrite(RED_PIN, HIGH);
  digitalWrite(GREEN_PIN, HIGH);
  digitalWrite(BLUE_PIN, HIGH);    
}

void LED_Off(void){
  digitalWrite(RED_PIN, LOW);
  digitalWrite(GREEN_PIN, LOW);
  digitalWrite(BLUE_PIN, LOW);    
}
