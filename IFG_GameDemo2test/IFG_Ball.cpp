#include <Arduino.h>
#include "IFG_Ball.h"

#define MAXIMUM_POSSESSION_DURATION_MS 10000

static boolean I_have_the_ball = false;
static unsigned long possession_start_time = 0;

boolean Ball_in_possession(void){
  return I_have_the_ball;
}

void Ball_possess(void){  
  I_have_the_ball = true;
  possession_start_time = millis();
}

void Ball_release(void){
  I_have_the_ball = false;
}

unsigned long Ball_possession_duration_ms(void){
  return (millis() - possession_start_time);
}

boolean Ball_dropped(void){  
  if(Ball_possession_duration_ms() > MAXIMUM_POSSESSION_DURATION_MS){
    return true;
  }
  return false;
}
