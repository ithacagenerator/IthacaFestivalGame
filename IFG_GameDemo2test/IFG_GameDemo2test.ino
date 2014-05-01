// Main program for testing

#include <stdint.h>
#include <IFG_IRremote.h>
#include "IFG_Utility.h"
#include "PushButton.h"
#include "IFG_LED.h"
#include "IFG_Ball.h"
#include "MessageLayer.h"
#include "TransportLayer.h"

#define IS_SENDER
int test_id = 4;

void setup(){
  IFG_DEBUG_BEGIN(115200);
  IFG_DEBUG_PRINTLN(F("Ithaca Festival Game"));
  Serial.print("Test running: ");
  Serial.println(test_id);


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
IFG_StatusCode status_code;

#ifdef IS_SENDER
	switch (test_id) {
	case 1:
          // Send a packet with known content
          Packet_set_type(1);
          Packet_set_sequence_number(0x2345);
          Packet_set_source_address(0x67);
          Packet_set_destination_address(0x89);
          Packet_set_payload_length(0xA);
          Packet_set_payload_body(0xBCD);
          Print_packet();
          send_packet();
	  break;
	case 2:
          delay(500);
          send_REQ();
          Print_packet();
	  break;
        case 3:
          delay(500);          
          send_ACK();
          Print_packet();
	  break;
        case 4:
          delay(5000);          
          send_MSG();
          // TODO: print message buffer
	  break;
        }
#else
  switch (test_id) {
    case 1:
      // Receive and print a packet with known content
      if (IFG_SUCCESS == Transport_receive (&temp)) {
        Serial.print("Test 1: ");
        Serial.println(temp, HEX);
      }
      break;
    case 2:
      if (IFG_SUCCESS == wait_for_REQ()) {
        Serial.println("REQ recieved");
      }
      break;
    case 3:
      timestamp_REQ();  
      if (IFG_SUCCESS == wait_for_ACK()) {
        Serial.println("ACK recieved");                
      }
      break;
    case 4:
      timestamp_ACK();   
      status_code = wait_for_MSG();
      if (IFG_SUCCESS == status_code) {
        Serial.println("MSG recieved");                
      }
      else{
        Serial.print("MSG receive did not succeed (");
        Serial.print(status_code);
        Serial.println(")");
        // TODO: This seems to be failing every other time.
      }
      break;      
    } 
#endif
}

