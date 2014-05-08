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
int test_id = 7;
uint32_t temp = 0xff;

void setup(){  
  Serial.begin(115200);
  Serial.println(F("Ithaca Festival Game"));
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
          delay(2000); 
          send_REQ();
	  break;
        case 6: // determine if receiver can ever hear an ACK after send_REQ call 
        case 3:      
          delay(2000);          
          send_ACK();
	  break;
        case 4:
          delay(20);          
          send_MSG();
          // TODO: print message buffer
	  break;
        case 5:          
          if(temp == 0xff){
            Serial.println(F("Attempt Message Transfer"));
            temp = 1;
          }          
          
          status_code = attempt_message_transfer();
          if(IFG_SUCCESS == status_code){
            Serial.println(F("Successful Message Transfer"));             
          }    
          else{
            Serial.print(F("Transfer not sucessful - Status Code "));
            Serial.println(status_code);
          }    
          break;
        case 7:
          temp = 0;
          if(Serial.available()){
            while(Serial.available()) Serial.read();
            Serial.println(F("Keyboard Input"));
            temp = 0xff; 
          }
          
          if(temp == 0xff){
            temp = 0;
            Serial.println(F("Send REQ"));          
            send_REQ();
            Serial.println(F("Waiting for ACK"));
            status_code = wait_for_ACK();
            if(status_code == IFG_SUCCESS){
              Serial.println(F("Got ACK"));
              delay(500);
              Serial.println(F("Send MSG"));
              send_MSG();
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
    case 6: // determine if receiver can ever hear an ACK after send_REQ call  
      if(temp == 0xff){
        Serial.println(F("Sending REQ"));
        send_REQ();
        temp = 0;
      }
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

        if(temp == 0xff){
          Serial.println(F("Attempt Message Receipt"));
          temp = 1;
        }
          
        if(IFG_SUCCESS == attempt_message_receive()){
          Serial.println(F("Successful Message Receipt"));
        }            
        else{
          //Serial.println(F("No Message Received"));
        }          
        
        break; 
      case 7:
        status_code = wait_for_REQ();
        if(status_code == IFG_SUCCESS){
          Serial.println(F("Got REQ"));

          delay(500);
          Serial.println(F("Send ACK"));
          send_ACK();
          
          Serial.println(F("Waiting for MSG"));
          status_code = wait_for_MSG();
          if(status_code == IFG_SUCCESS){
            Serial.println(F("MSG received")); 
          }
          
        }
        break;    
    } 
#endif
}

