#include <stdint.h>
#include <IFG_IRremote.h>
#include "IFG_Utility.h"
#include "PushButton.h"
#include "IFG_LED.h"
#include "IFG_Ball.h"
#include "MessageLayer.h"
#include "TransportLayer.h"

void do_action_with_ball(void);
void do_action_without_ball(void);

void setup(){
  IFG_DEBUG_BEGIN(115200);
  IFG_DEBUG_PRINT(F("Ithaca Festival Game"));

  Transport_enable_receive(); // start the receiver
  PushButton_Init();
  LED_Init();
  
  //TODO: If fou are player one, you should star with the ball
}

void loop(){
  if(Ball_in_possession() == true){
    do_action_with_ball();
  }
  else if(Ball_in_possession() == false){
    do_action_without_ball();
  }
}

void do_action_with_ball(void){
  IFG_StatusCode status = ERROR;
  if(button_is_pressed()){
    status = attempt_message_transfer();
  }
  
  if(status == SUCCESS){
    Ball_release();
    wait_for_button_released();
  }
  else if(status == TIMEOUT){
    if(Ball_dropped()){
      LED_Red();
      spin_forever();
    }
  }
}

void do_action_without_ball(void){
  IFG_StatusCode status = ERROR;  
  if(button_is_pressed()){
    status = attempt_message_receive();
  }  
  
  if(status == SUCCESS){
    Ball_possess();
    LED_White();
    wait_for_button_released();    
  }
}
