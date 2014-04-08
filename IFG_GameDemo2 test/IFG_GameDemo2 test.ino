// Main program for testing

#include <stdint.h>
#include <IFG_IRremote.h>
#include "IFG_Utility.h"
#include "PushButton.h"
#include "IFG_LED.h"
#include "IFG_Ball.h"
#include "MessageLayer.h"
#include "TransportLayer.h"

#define IS_SENDER;

#ifdef IS_SENDER
  #define MAX_TEST_ID = 3; // 4 message types to test
#else
  #define MAX_TEST_ID = 0;
#endif

int test_id = 0;

void setup(){
  IFG_DEBUG_BEGIN(115200);
  IFG_DEBUG_PRINTLN(F("Ithaca Festival Game"));

  Transport_enable_receive(); // start the receiver
  PushButton_Init();
  LED_Init();
  
  //TODO: If fou are player one, you should start with the ball
}

void loop(){
  if (button_is_pressed() ) {
  	wait_for_button_released;
  	if (MAX_TEST_ID < ++test_id) {
  		test_id = MAX_TEST_ID
  	}
  }
  run_test(test_id);
/*
  if(Ball_in_possession() == true){
    do_action_with_ball();
  }
  else if(Ball_in_possession() == false){
    do_action_without_ball();
  }
*/
}

void run_test(int test_id) {
#ifdef IS_SENDER
	switch (test_id)
	case 1:
		send_REQ();
		break;
	case 2:
		//---
		break;
#else
	// receive and report code
#endif
}
/*
void do_action_with_ball(void){
  IFG_StatusCode status = IFG_ERROR;
  if(button_is_pressed()){
    status = attempt_message_transfer();
  }
  
  if(status == IFG_SUCCESS){
    Ball_release();
    wait_for_button_released();
  }
  else if(status == IFG_TIMEOUT){
    if(Ball_dropped()){
      LED_Red();
      spin_forever();
    }
  }
}

void do_action_without_ball(void){
  IFG_StatusCode status = IFG_ERROR;  
  if(button_is_pressed()){
    status = attempt_message_receive();
  }  
  
  if(status == IFG_SUCCESS){
    Ball_possess();
    LED_White();
    wait_for_button_released();    
  }
}
*/
