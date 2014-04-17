// Main program for testing

#include <stdint.h>
#include <IFG_IRremote.h>
#include "IFG_Utility.h"
#include "PushButton.h"
#include "IFG_LED.h"
#include "IFG_Ball.h"
#include "MessageLayer.h"
#include "TransportLayer.h"

//#define IS_SENDER

int test_id = 2;

void setup(){
  IFG_DEBUG_BEGIN(115200);
  IFG_DEBUG_PRINTLN(F("Ithaca Festival Game"));

  Transport_enable_receive(); // start the receiver
  PushButton_Init();
  LED_Init();
  
  //TODO: If fou are player one, you should start with the ball
}

void loop(){
  /* if (button_is_pressed() ) {
  	wait_for_button_released();
  	if (++test_id > MAX_TEST_ID) {
  		test_id = MAX_TEST_ID;
  	}
  }*/
  Run_test(test_id);
  
}

void Run_test(int test_id) {

uint32_t temp;

#ifdef IS_SENDER
	switch (test_id) {
	case 1:
          Packet_set_type(1);
          Packet_set_sequence_number(0x2345);
          Packet_set_source_address(6);
          Packet_set_destination_address(7);
          Packet_set_payload_length(8);
          Packet_set_payload_body(0x90AB);
          Print_packet();
          send_packet();
	  break;
	case 2:
          delay (500);
          send_REQ();
          Print_packet();
	  break;
        }
#else
  switch (test_id) {
    case 1:
      if (IFG_SUCCESS == Transport_receive (&temp)) {
        Serial.print("Test 1: ");
        Serial.println(temp, HEX);
      }
      break;
    case 2:
      delay (20);
      if (IFG_SUCCESS == wait_for_REQ ()) {
        Serial.println("REQ recieved");
      }
      break;
    } 
#endif
}

