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

#define STATE_IDLE            1
#define STATE_TXREQ_AND_RXACK 2
#define STATE_TXMSG           3
#define STATE_RXREQ           4
#define STATE_TXACK           5
#define STATE_RXMSG           6

uint8_t state = STATE_IDLE;
long previousMillis = 0; 
long interval = 300;

void setup(){
  IFG_DEBUG_BEGIN(115200);
  IFG_DEBUG_PRINTLN(F("Ithaca Festival Game"));

  Transport_enable_receive(); // start the receiver
  PushButton_Init();
  LED_Init();
  
  //TODO: If fou are player one, you should star with the ball
  if(0){
    Ball_possess();
    // TODO: Should the first player start with a point automatically?
    // set_player_score(MY_ADDRESS, 1);
    LED_White();
  }
}

void loop(){
  if(Ball_in_possession()){
    do_action_with_ball();
  }
  else if(!Ball_in_possession()){
    do_action_without_ball();
  }
}

void do_action_with_ball(void){
  unsigned long currentMillis = millis();
  IFG_StatusCode status_code = IFG_ERROR;
  switch(state){
    case STATE_IDLE:
      if(button_is_pressed()){
        state = STATE_TXREQ_AND_RXACK;
        IFG_DEBUG_PRINTLN(F("Going to state STATE_TXREQ_AND_RXACK"));
      }
      break;
    case STATE_TXREQ_AND_RXACK:
      if(!button_is_pressed()){
        state = STATE_IDLE;   
        IFG_DEBUG_PRINTLN(F("Going to state STATE_IDLE"));    
      }
      else{
        interval = 300;
        send_REQ();
        
        if(currentMillis - previousMillis > interval) {
          previousMillis = currentMillis;
          status_code = wait_for_ACK();
          if(status_code == IFG_SUCCESS){
            LED_Red();
            state = STATE_TXMSG;
            IFG_DEBUG_PRINTLN(F("Going to state STATE_TXMSG"));
          }
        }
      }                      
      break;
    case STATE_TXMSG:
      if(!button_is_pressed()){
        LED_Off();
        Ball_release();
        state = STATE_IDLE;
        IFG_DEBUG_PRINTLN(F("Going to state STATE_IDLE"));
      }
      else{
        send_MSG();
      }
      break; 
  }
}

void do_action_without_ball(void){
  IFG_StatusCode status_code = IFG_ERROR;
  
  switch(state){
    case STATE_IDLE:
      if(button_is_pressed()){
        state = STATE_RXREQ; 
        IFG_DEBUG_PRINTLN(F("Going to state STATE_RXREQ"));
      }
      break;
    case STATE_RXREQ:
      if(!button_is_pressed()){
        state = STATE_IDLE; 
        IFG_DEBUG_PRINTLN(F("Going to state STATE_IDLE"));
      }
      else{
        status_code = wait_for_REQ();
        if(status_code == IFG_SUCCESS){
          state = STATE_TXACK;
          IFG_DEBUG_PRINTLN(F("Going to state STATE_TXACK"));
          LED_Red();
        }
      }
      
      break;
    case STATE_TXACK:
      send_ACK();
      if(!button_is_pressed()){
        state = STATE_RXMSG; 
        IFG_DEBUG_PRINTLN(F("Going to state STATE_RXMSG"));
      }
      break; 
    case STATE_RXMSG:
      status_code = wait_for_MSG();
      if(status_code == IFG_SUCCESS){
        LED_White();
        state = STATE_IDLE;
        IFG_DEBUG_PRINTLN(F("Going to state STATE_IDLE"));
        // modify my own score now that I possess the ball
        set_player_score(MY_ADDRESS, get_player_score(MY_ADDRESS) + 1);
        Ball_possess(); 
      }
      break; 
  }
}