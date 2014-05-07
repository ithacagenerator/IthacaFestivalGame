#include <Arduino.h>
#include "MessageLayer.h"
#include "TransportLayer.h"
#include "IFG_Packet.h"
#include "IFG_Utility.h"
#include <stdint.h>

static uint8_t message_payload[MESSAGE_PAYLOAD_SIZE] = {0x01,0x23,0x45,0xFE,0xDC,0xBA}; // the game state transmitted/modified from player to player
static uint16_t message_payload_write_index = 0;            // where the next value should be written to
static uint16_t last_received_sequence_number = 0;          // last received SEQENCE NUMBER
static uint8_t acknowledging_player_address = MY_ADDRESS;   // source address associated with last received ACK
static uint8_t requesting_player_address = MY_ADDRESS;      // source address associated with last received REQ
static uint32_t last_req_timestamp = 0;                     // the timestamp when the last REQ packet was sent
static uint32_t last_ack_timestamp = 0;                     // the timestamp when the last ACK packet was sent


IFG_StatusCode attempt_message_receive(void){
  IFG_StatusCode status_code = IFG_ERROR;
  status_code = wait_for_REQ();
  if(status_code == IFG_SUCCESS){
    send_ACK();
    status_code = wait_for_MSG();
    if(status_code == IFG_SUCCESS){
      //The message has been received!
      //TODO: update the game state        
    }
  }
    
  return status_code;
}

IFG_StatusCode attempt_message_transfer(void){
  IFG_StatusCode status_code = IFG_ERROR;
  
  send_REQ();
  status_code = wait_for_ACK();
  if(status_code == IFG_SUCCESS){
    send_MSG();
    return IFG_SUCCESS;
  }  
  
  return IFG_TIMEOUT;  
}

void send_REQ(void){
  Packet_set_type(PACKET_TYPE_REQ);
  Packet_set_sequence_number(next_sequence_number());  
  Packet_set_source_address(MY_ADDRESS);
  Packet_set_destination_address(0);  
  IFG_DEBUG_PRINT(millis());
  IFG_DEBUG_PRINTLN(" Sending Packet");
  send_packet();  
  IFG_DEBUG_PRINT(millis());
  IFG_DEBUG_PRINTLN(" Packet Sent");  
  timestamp_REQ();
}

void send_MSG(void){ 
  uint16_t temp = 0;
  uint16_t message_index = 0; 
  Packet_set_type(PACKET_TYPE_MSG);
  Packet_set_sequence_number(next_sequence_number());  
  Packet_set_source_address(MY_ADDRESS);
  Packet_set_destination_address(acknowledging_player_address);
  Packet_set_payload_length(12);
  
  // send a sequence of packets to transmit the entire message
  // compose the payload 12-bits at a time
  // since it's 3-bytes = 24-bits per player counter entry
  // we know ahead of time that there will be 2 packets sent per player
  // the array is variable length, terminated by an entry with address 0
  // this would be harder if total packet length were arbitrary
  IFG_DEBUG_PRINT(millis());
  IFG_DEBUG_PRINTLN(" Sending MSG Packet");  
  Print_packet();
  while((message_index < MESSAGE_PAYLOAD_SIZE)){        
    // there will be two send packets resulting from this loop
    // and then a possibly a break statement
    temp = message_payload[message_index++];                
    temp <<= 4;                                             // make room for 4 bits
    temp |= ((message_payload[message_index] & 0xf0) >> 4); // stuff in the top four bits
    Packet_set_payload_body(temp);    
    Print_packet();     
    send_packet();   

    // new packet, notice = operator
    temp = (message_payload[message_index++] & 0x0f);       // start with the bottom four bits
    temp <<= 8;                                             // make room for 8-bits
    temp |= message_payload[message_index++];          
    Packet_set_payload_body(temp);    
    Print_packet();         
    send_packet();
  }
  IFG_DEBUG_PRINT(millis());
  IFG_DEBUG_PRINTLN(" Sent MSG Packet");      
}


void send_ACK(void){
  Packet_set_type(PACKET_TYPE_ACK);  
  Packet_set_sequence_number(next_sequence_number());  
  Packet_set_source_address(MY_ADDRESS);
  Packet_set_destination_address(requesting_player_address);  
  IFG_DEBUG_PRINT(millis());
  IFG_DEBUG_PRINTLN(" Sending Packet");  
  send_packet();
  IFG_DEBUG_PRINT(millis());
  IFG_DEBUG_PRINTLN(" Packet Sent");
  timestamp_ACK();
}

void send_first_half_packet(){
  uint32_t temp = Packet_get_byte(0);
  temp <<= 8;
  temp |= Packet_get_byte(1);
  temp <<= 8;
  temp |= Packet_get_byte(2);
  temp <<= 8;  
  temp |= Packet_get_byte(3);  
  
  Transport_transmit(temp);
}

void send_second_half_packet(){
  uint32_t temp = Packet_get_byte(4);
  temp <<= 8;
  temp |= Packet_get_byte(5);
  temp <<= 8;
  temp |= Packet_get_byte(6);
  temp <<= 8;  
  temp |= Packet_get_byte(7);  
  
  Transport_transmit(temp);
}

void send_packet(){
  send_first_half_packet();
  send_second_half_packet();
}

#define ACK_TIMEOUT_DURATION_MS 500

IFG_StatusCode wait_for_ACK(){
  IFG_StatusCode status_code = IFG_ERROR;
  uint32_t first_half_packet = 0, second_half_packet = 0;
  while(awaiting_ack_duration() < ACK_TIMEOUT_DURATION_MS){
    status_code = wait_for_Packet(PACKET_TYPE_ACK, &first_half_packet, &second_half_packet);
    if(status_code == IFG_SUCCESS){
       status_code = validate_and_decode_ACK(first_half_packet, second_half_packet);    
       if(status_code == IFG_SUCCESS){
          IFG_DEBUG_PRINTLN(F("Successfully Recieved ACK Packet "));     
          return IFG_SUCCESS; 
       }
    }
  }

  IFG_DEBUG_PRINTLN(F("Error due to Timeout "));
  return IFG_TIMEOUT;
}

uint32_t awaiting_ack_duration(void){
  return (millis() - last_req_timestamp);
}

IFG_StatusCode validate_and_decode_ACK(uint32_t first_half_packet, uint32_t second_half_packet){
  IFG_StatusCode status_code = validate_and_decode_Packet(PACKET_TYPE_ACK, first_half_packet, second_half_packet);
  if(status_code == IFG_ERROR){
    return IFG_ERROR; 
  }
  
  // looks good, lets go with it
  acknowledging_player_address  = extract_source_address(first_half_packet);
  last_received_sequence_number = extract_sequence_number(first_half_packet); 
  
  return IFG_SUCCESS;
}

// return SUCCESS if and only if a *valid* REQ packet is received
// otherwise return ERROR on packet received but not validated
// or TIMEOUT if no data is received
// 
// This function times out as quickly as possible
IFG_StatusCode wait_for_REQ(void){
  IFG_StatusCode status_code = IFG_ERROR;
  uint32_t first_half_packet = 0, second_half_packet = 0;

  status_code = wait_for_Packet(PACKET_TYPE_REQ, &first_half_packet, &second_half_packet);
  if(status_code != IFG_SUCCESS){
    return status_code; 
  }
  
  // At this point temp1, temp2 *may* hold a valid REQ packet
  status_code = validate_and_decode_REQ(first_half_packet, second_half_packet);    
  if(status_code != IFG_SUCCESS){
    IFG_DEBUG_PRINT(F("Error due to decode - status code =  "));
    IFG_DEBUG_PRINTLN(status_code);
    return IFG_ERROR; 
  }

  // all set, the requesting_player_address and last_received_sequence_number
  // state variables were populated by validate_and_decode_REQ 
  IFG_DEBUG_PRINTLN(F("Returning Success")); 
  return IFG_SUCCESS;
}

IFG_StatusCode validate_and_decode_REQ(uint32_t first_half_packet, uint32_t second_half_packet){
  IFG_StatusCode status_code = validate_and_decode_Packet(PACKET_TYPE_REQ, first_half_packet, second_half_packet);
  if(status_code == IFG_ERROR){
    return IFG_ERROR; 
  }
  
  // REQ packets are supposed to have a destination address of 0
  if(0 != extract_destination_address(second_half_packet)){ 
    IFG_DEBUG_PRINTLN(F("Error: REQ packet with non-zero destination"));
    return IFG_ERROR; 
  }
  
  // looks good, lets go with it
  requesting_player_address     = extract_source_address(first_half_packet);
  last_received_sequence_number = extract_sequence_number(first_half_packet);

  IFG_DEBUG_PRINT(F("Requesting Player Address: "));
  IFG_DEBUG_PRINTLN(requesting_player_address);
  IFG_DEBUG_PRINT(F("Last Sequence Number: "));
  IFG_DEBUG_PRINTLN(last_received_sequence_number);  
  
  return IFG_SUCCESS;
}

#define MSG_TIMEOUT_DURATION_MS 1000L

IFG_StatusCode wait_for_MSG(){
  IFG_StatusCode status_code = IFG_ERROR;
  uint16_t packet_counter = 0;
  uint32_t first_half_packet = 0, second_half_packet = 0;
  
  uint16_t payload_body = 0;
  uint8_t  byte1 = 0, byte2 = 0, byte3 = 0;  

  message_payload_write_index = 0;
  
  // the following loop can exit because
  // (1) you receive the whole message successfully and return IFG_SUCCESS
  // (2) you experience a validation error at some point and return IFG_ERROR
  // (3) you run out of time waiting for MSG packets and return IFG_TIME
  while(awaiting_msg_duration() < MSG_TIMEOUT_DURATION_MS){
    
    status_code = wait_for_Packet(PACKET_TYPE_MSG, &first_half_packet, &second_half_packet);

    // packets come in pairs for MSG sequences
    if(status_code == IFG_SUCCESS){
      timestamp_ACK();
      
      status_code = validate_and_decode_MSG(first_half_packet, second_half_packet);
      if(status_code == IFG_ERROR){
        return IFG_ERROR; 
      }
      
      payload_body = extract_payload_body(second_half_packet);
      
      if(packet_counter == 0){
        
        packet_counter = 1;    
        
        byte1 = (payload_body >> 4) & 0xff;  // byte1 is the top 8-bits of the payload
        byte2 = (payload_body & 0x0f) << 4;  // the top half of byte2 is the bottom 4-bits of the payload
      }
      else if(packet_counter == 1){
        
        packet_counter = 0;   
        
        byte2 |= (payload_body >> 8) & 0x0f; // the bottom half of byte2 is the top 4-bits of the payload
        byte3 = (payload_body & 0xff);       // byte3 is the bottom 8-bits of the payload          
        
        push_message_payload(byte1);         // add the bytes to the message payload buffer
        push_message_payload(byte2);         // add the bytes to the message payload buffer
        push_message_payload(byte3);         // add the bytes to the message payload buffer       
              
        IFG_DEBUG_PRINT(F("Bytes: "));
        IFG_DEBUG_PRINT_HEX(byte1);
        IFG_DEBUG_PRINT(F(", "));
        IFG_DEBUG_PRINT_HEX(byte2);
        IFG_DEBUG_PRINT(F(", "));
        IFG_DEBUG_PRINTLN_HEX(byte3);
        
        timestamp_ACK();
        
        IFG_DEBUG_PRINT(F("message_payload_write_index = "));
        IFG_DEBUG_PRINTLN(message_payload_write_index);
        if(message_payload_write_index == MESSAGE_PAYLOAD_SIZE){
          IFG_DEBUG_PRINTLN(F("Info: End of MSG Packet"));  
          return IFG_SUCCESS;
        }
      }      
    } 
  }
  
  return IFG_TIMEOUT;
}

uint32_t awaiting_msg_duration(void){
  return (millis() - last_ack_timestamp);  
}

IFG_StatusCode validate_and_decode_MSG(uint32_t first_half_packet, uint32_t second_half_packet){
  IFG_StatusCode status_code = validate_and_decode_Packet(PACKET_TYPE_MSG, first_half_packet, second_half_packet);
  if(status_code == IFG_ERROR){
    return IFG_ERROR; 
  }
  
  // is the sender the last person who sent a REQ
  if(requesting_player_address != extract_source_address(first_half_packet)){ 
    IFG_DEBUG_PRINT(F("Error: Unexpected Source Address: "));
    IFG_DEBUG_PRINTLN(extract_source_address(first_half_packet));
    return IFG_ERROR; 
  }  
  
  // looks good, lets go with it
  last_received_sequence_number = extract_sequence_number(first_half_packet);
  
  return IFG_SUCCESS; 
}

IFG_StatusCode validate_and_decode_Packet(uint8_t expected_packet_type, uint32_t first_half_packet, uint32_t second_half_packet){
  IFG_StatusCode status_code = IFG_ERROR;
  
  uint8_t checksum = calculate_checksum(first_half_packet, second_half_packet);
  
  // checksum has to be zero, by definition / construction
  if(checksum != 0){
    IFG_DEBUG_PRINT(F("Error: Checksum = "));
    IFG_DEBUG_PRINTLN(checksum);
    return IFG_ERROR; 
  }
  
  // at least the checksum is correct
  // am I the intended recipient?
  if(expected_packet_type != PACKET_TYPE_REQ){ // REQ packets are not addressed
    if(MY_ADDRESS != extract_destination_address(second_half_packet)){
      IFG_DEBUG_PRINT(F("Error: Unexpected destination address - "));
      IFG_DEBUG_PRINTLN(extract_destination_address(second_half_packet));
      return IFG_ERROR; 
    }
  }  
  
  return IFG_SUCCESS; 
}

uint16_t next_sequence_number(void){
  return (last_received_sequence_number + 1);
}

IFG_StatusCode wait_for_Packet(uint8_t packet_type, uint32_t * p_first_half, uint32_t * p_second_half){
  IFG_StatusCode status_code = IFG_ERROR;

  status_code = Transport_receive(p_first_half);
  if(status_code != IFG_SUCCESS){            
    return IFG_TIMEOUT; // no packet, no problem
  }
  
  // We might have just got the first half of a packet  
  // it had better be the expected type, otherwise bail out.
  // One way this can happen is if you are out of sync with
  // the transmitter and received the second half of a packet.
  if(extract_packet_type(*p_first_half) != packet_type){
    //IFG_DEBUG_PRINT(F("Error: Unexpected Packet Type - Expected: ")); 
    //IFG_DEBUG_PRINT_HEX(packet_type); 
    //IFG_DEBUG_PRINT(F(" Got: ")); 
    //IFG_DEBUG_PRINTLN_HEX(extract_packet_type(*p_first_half)); 
    return IFG_ERROR; 
  }
    
  // We are on the right track, lets try and receive 
  // the second half of the packet
  // give it some time to have a chance though
  for(int ii = 0; ii < 1000; ii++){
    status_code = Transport_receive(p_second_half);
    if(status_code == IFG_SUCCESS){
      break;
    }  
  }
  
  if(status_code != IFG_SUCCESS){
    return status_code; 
  }
  
  IFG_DEBUG_PRINT(F("Packet Rx: "));
  IFG_DEBUG_PRINT_HEX((uint32_t) (*p_first_half));  
  IFG_DEBUG_PRINTLN_HEX((uint32_t) (*p_second_half));             

  return IFG_SUCCESS;  
}

uint8_t extract_packet_type(uint32_t first_half_packet){
  return ((first_half_packet >> 24) & 0xff);
}

uint16_t extract_sequence_number(uint32_t first_half_packet){
  uint16_t sequence_number = 0;
  sequence_number = (first_half_packet >> 16) & 0xff; // msb of sequence number is index 1 of the packet
  sequence_number <<= 8;
  sequence_number |= (first_half_packet >> 8) & 0xff; // lsb of sequence number is index 2 of the packet  
  return sequence_number;
}

uint8_t extract_source_address(uint32_t first_half_packet){
  return (first_half_packet & 0xff);
}

uint8_t extract_destination_address(uint32_t second_half_packet){
  return ((second_half_packet >> 24) & 0xff);
}

uint16_t extract_payload_body(uint32_t second_half_packet){
  uint16_t payload_body = 0;
  payload_body = (second_half_packet >> 16) & 0xff; // msb of payload body is index 1 of the packet
  payload_body <<= 8;
  payload_body |= (second_half_packet >> 8) & 0xff; // lsb of payload body is index 2 of the packet  
  return (payload_body & 0x0fff);  
}

uint8_t extract_checksum(uint32_t second_half_packet){
  return (second_half_packet & 0xff);  
}

uint8_t calculate_checksum(uint32_t first_half_packet, uint32_t second_half_packet){
  uint8_t ii = 0;
  uint16_t sum = 0;
  
  for(ii = 0; ii < 4; ii++){ // first four bytes
    sum += (first_half_packet >> (8 * (3 - ii))) & 0xff;
  }
  
  for(ii = 0; ii < 4; ii++){ // next four bytes
    sum += (second_half_packet >> (8 * (3 - ii))) & 0xff;
  }
  
  return (sum & 0xff);
}

void push_message_payload(uint8_t value){
  if(message_payload_write_index < MESSAGE_PAYLOAD_SIZE){
    message_payload[message_payload_write_index++] = value;
  }
}

void timestamp_REQ(void){
  last_req_timestamp = millis();
}

void timestamp_ACK(void){
  last_ack_timestamp = millis();
}

