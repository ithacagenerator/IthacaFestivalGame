#include <stdint.h>
#include <IFG_IRremote.h>
#include "IFG_Utility.h"
#include "PushButton.h"
#include "IFG_LED.h"
#include "IFG_Ball.h"
#include "MessageLayer.h"
#include "TransportLayer.h"
#if defined(IS_SCORE_BOARD)
#include <Adafruit_NeoPixal.h>
#endif

void do_action_with_ball(void);
void do_action_without_ball(void);

#define STATE_IDLE            1
#define STATE_TXREQ_AND_RXACK 2
#define STATE_TXMSG           3
#define STATE_RXREQ           4
#define STATE_TXACK           5
#define STATE_RXMSG           6
#define STATE_SPIN            7

#define SCORE_COUNT           8 // Can have up to 8 scores

uint8_t state = STATE_IDLE;
long previousMillis = 0; 
long interval = 300;

void display_score ( void );

void setup(){
  IFG_DEBUG_BEGIN(115200);
  IFG_DEBUG_PRINTLN(F("Ithaca Festival Game"));

  #if defined(IS_SCORE_BOARD)
  IFG_DEBUG_PRINTLN(F("Scoreboard!"));
  #endif

  Transport_enable_receive(); // start the receiver
  PushButton_Init();
  LED_Init();

  //TODO: If you are player one, you should start with the ball
  #if defined(STARTS_WITH_BALL)
  Ball_possess();
  // TODO: Should the first player start with a point automatically?
  // set_player_score(MY_ADDRESS, 1);
  LED_White();
  #endif
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
          //LED_Red();
          LED_Off();
          state = STATE_TXMSG;
          IFG_DEBUG_PRINTLN(F("Going to state STATE_TXMSG"));
        }
      }
    }                      
    break;
  case STATE_TXMSG:
    if(!button_is_pressed()){
      //LED_Off();
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
  unsigned long currentMillis = millis();

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
        //LED_Red();
      }
    }
    
    break;
  case STATE_TXACK:
    send_ACK();
    if(!button_is_pressed()){
      state = STATE_RXMSG; 
      previousMillis = currentMillis;
      IFG_DEBUG_PRINTLN(F("Going to state STATE_RXMSG"));
    }
    break; 
  case STATE_RXMSG:
    status_code = wait_for_MSG();
    interval = 10000; // 10 seconds timeout
    if(status_code == IFG_SUCCESS){
      #ifndef IS_SCORE_BOARD 
      LED_White();
      state = STATE_IDLE;
      IFG_DEBUG_PRINTLN(F("Going to state STATE_IDLE"));
      // modify my own score now that I possess the ball
      set_player_score(MY_ADDRESS, get_player_score(MY_ADDRESS) + 1);
      Ball_possess(); 
      message_pretty_print();
      #else
      LED_White();
      state = STATE_IDLE;
      IFG_DEBUG_PRINTLN(F("Posting Score"));
      IFG_DEBUG_PRINTLN(F("Going to state STATE_IDLE"));
      message_pretty_print();
      display_score();
      delay(5000); // leave the light on for 5 seconds
      LED_Off();
      #endif
    }
    else if(currentMillis - previousMillis > interval) { // timeout!
      LED_Red();
      state = STATE_SPIN;
      IFG_DEBUG_PRINTLN(F("Timeout waiting for MSG, going to STATE_SPIN"));
    }
    break; 
  case STATE_SPIN:
    // do nothing...
    break;
  }
}

#if defined(IS_SCORE_BOARD)

uint8_t score[SCORE_COUNT] = {0};
uint8_t num_scores = 0;

void display_score ( void ){
  score[num_scores++] = compute_score(); // rtns 0, 1, 2
  for(uint8_t ii = 0; ii < num_scores; ii++){
    IFG_DEBUG_PRINT(F("ROUND "));
    IFG_DEBUG_PRINT(ii);
    IFG_DEBUG_PRINT(F(": "));
    IFG_DEBUG_PRINTLN(score[ii]);
  }
}

#endif

