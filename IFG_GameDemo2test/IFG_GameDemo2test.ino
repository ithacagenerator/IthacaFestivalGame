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
uint32_t temp = 0xff;

void setup(){  
  IFG_DEBUG_BEGIN(115200);
  IFG_DEBUG_PRINTLN(F("Ithaca Festival Game"));
  #ifdef IS_SENDER
  Serial.println("Sender");
  #else 
  Serial.println("Receiver");  
  #endif
  Serial.print("Test running: ");
  Serial.println(test_id);

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
          send_packet();
	  break;
	case 2:
          // it seems as though the time between transmissions is highly relevant
          // to the receiver being able to function at all
          // shorter seems to be better, which leads me to believe not all
          // transmissions are actually getting through
          delay(20); 
          send_REQ();
	  break;
        case 3:       
          delay(20);          
          send_ACK();
	  break;
        case 4:
          delay(20);          
          send_MSG();
          // TODO: print message buffer
	  break;
        case 5:
          if(temp == 0){
            if(IFG_SUCCESS == attempt_message_receive()){
              Serial.println(F("Successful Message Receipt"));
              temp = 1;
              delay(1000);
            }  
            else{
              //Serial.println(F("No Message Received"));
            }            
          }
          else{
            if(temp == 0xff){
              Serial.println(F("Attempt Message Transfer"));
              temp = 1;
            }          
          
            if(IFG_SUCCESS == attempt_message_transfer()){
              Serial.println(F("Successful Message Transfer"));
              temp = 0;             
            }        
          } 
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
      case 5:    
        if(temp == 0){
          if(IFG_SUCCESS == attempt_message_transfer()){
            Serial.println(F("Successful Message Transfer"));
            temp = 1;             
          }
        }
        else{
          if(temp == 0xff){
            Serial.println(F("Attempt Message Receipt"));
            temp = 1;
          }
          
          if(IFG_SUCCESS == attempt_message_receive()){
            Serial.println(F("Successful Message Receipt"));
            temp = 0;
            delay(1000);
          }            
          else{
            //Serial.println(F("No Message Received"));
          }          
        }        
        break;      
    } 
#endif
}

