#include <Arduino.h>
#include "MessageLayer.h"
#include "TransportLayer.h"
#include "IFG_Packet.h"
#include "IFG_Utility.h"
#include <stdint.h>

static uint8_t message_payload[MESSAGE_PAYLOAD_SIZE] = {0}; // the game state transmitted/modified from player to player
static uint16_t last_received_sequence_number = 0;          // last received SEQENCE NUMBER
static uint8_t acknowledging_player_address = 0;            // source address associated with last received ACK
static uint8_t requesting_player_address = 0;               // source address associated with last received REQ
static uint32_t last_req_timestamp = 0;                     // the timestamp when the last REQ packet was sent
static uint32_t last_ack_timestamp = 0;                     // the timestamp when the last ACK packet was sent

IFG_StatusCode attempt_message_receive(void){
  IFG_StatusCode status_code = ERROR;
  status_code = wait_for_REQ();
  if(status_code == SUCCESS){
    send_ACK();
    status_code = wait_for_MSG();
  }
  
  
  return status_code;
}

IFG_StatusCode attempt_message_transfer(void){
  IFG_StatusCode status_code = ERROR;
  
  send_REQ();
  status_code = wait_for_ACK();
  if(status_code == SUCCESS){
    send_MSG();
    return SUCCESS;
  }  
  
  return TIMEOUT;  
}

void send_REQ(void){
  Packet_set_type(PACKET_TYPE_REQ);
  Packet_set_sequence_number(last_received_sequence_number+1);  
  Packet_set_source_address(MY_ADDRESS);
  Packet_set_destination_address(0);  
  send_packet();  
  last_req_timestamp = millis();
}

void send_MSG(void){ 
  uint16_t temp = 0;
  uint16_t message_index = 0; 
  Packet_set_type(PACKET_TYPE_MSG);
  Packet_set_sequence_number(last_received_sequence_number+1);  
  Packet_set_source_address(MY_ADDRESS);
  Packet_set_destination_address(acknowledging_player_address);
  Packet_set_payload_length(12);
  
  // send a sequence of packets to transmit the entire message
  // compose the payload 12-bits at a time
  // since it's 3-bytes = 24-bits per player counter entry
  // we know ahead of time that there will be 2 packets sent per player
  // the array is variable length, terminated by an entry with address 0
  // this would be harder if total packet length were arbitrary
  while((message_index < MESSAGE_PAYLOAD_SIZE)){
    // there will be two send packets resulting from this loop
    // and then a possibly a break statement
    temp = message_payload[message_index++];                
    temp <<= 4;                                             // make room for 4 bits
    temp |= ((message_payload[message_index] & 0xf0) >> 4); // stuff in the top four bits
    Packet_set_payload_body(temp);
    send_packet();

    // new packet, notice = operator
    temp = (message_payload[message_index++] & 0x0f);       // start with the bottom four bits
    temp <<= 8;                                             // make room for 8-bits
    temp |= message_payload[message_index++];          
    Packet_set_payload_body(temp);    
    send_packet();  
    
    if(message_payload[message_index - 3] == 0){            // having transmitted a terminator packet 
      break;                                                // break out of the loop - transmit complete
    }
  }
}


void send_ACK(void){
  Packet_set_type(PACKET_TYPE_ACK);  
  Packet_set_sequence_number(last_received_sequence_number+1);  
  Packet_set_source_address(MY_ADDRESS);
  Packet_set_destination_address(requesting_player_address);  
  send_packet();
  last_ack_timestamp = millis();
}

void send_packet0(){
  uint32_t temp = Packet_get_byte(0);
  temp <<= 8;
  temp |= Packet_get_byte(1);
  temp <<= 8;
  temp |= Packet_get_byte(2);
  temp <<= 8;  
  temp |= Packet_get_byte(3);  
  
  Transport_transmit(temp);
}

void send_packet1(){
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
  send_packet0();
  send_packet1();
}

#define ACK_TIMEOUT_DURATION_MS 500

IFG_StatusCode wait_for_ACK(){
  IFG_StatusCode status_code = ERROR;
  uint8_t rx_count = 0;
  uint32_t temp1 = 0, temp2 = 0;
  while(awaiting_ack_duration() < ACK_TIMEOUT_DURATION_MS){
    if(rx_count == 0){
      status_code = Transport_receive(&temp1);
      if(status_code == SUCCESS){
        rx_count++;        
      }      
    }
    else{
      status_code = Transport_receive(&temp2);
      if(status_code == SUCCESS){
        rx_count = 0;
        // temp1, temp2 may have an ACK packet, lets find out
        status_code = validate_ACK(temp1, temp2);    
        if(status_code == SUCCESS){
          return SUCCESS; 
        }
      }            
    }
  }
  
  return TIMEOUT;
}

uint32_t awaiting_ack_duration(void){
  return (millis() - last_req_timestamp);
}

IFG_StatusCode validate_ACK(uint32_t packet0, uint32_t packet1){
  IFG_StatusCode status_code = ERROR;
  uint8_t temp[8] = {0};
  uint8_t ii = 0;
  uint16_t sum = 0;
  temp[0] = (packet0 >> 24) & 0xff;
  temp[1] = (packet0 >> 16) & 0xff;
  temp[2] = (packet0 >>  8) & 0xff;
  temp[3] = (packet0 >>  0) & 0xff;  
  temp[4] = (packet1 >> 24) & 0xff;
  temp[5] = (packet1 >> 16) & 0xff;
  temp[6] = (packet1 >>  8) & 0xff;
  temp[7] = (packet1 >>  0) & 0xff;  

  for(ii = 0; ii < 8; ii++){
    sum += temp[ii]; 
  }
  
  if((sum & 0xff) != 0){
    return ERROR; 
  }
  
  // at least the checksum is correct
  // am I the intended recipient?
  if(MY_ADDRESS != temp[4]){ // destination address is index 4 in the packet
    return ERROR; 
  }
  
  // checksum is valid and I am the intended recipient
  if(PACKET_TYPE_ACK != temp[0]){ // packet type is index 0 of the packet
    return ERROR;
  }
  
  // looks good, lets go with it
  acknowledging_player_address  = temp[3];  // source address is index 3 of the packet
  last_received_sequence_number = temp[1];  // msb of sequence number is index 1 of the packet
  last_received_sequence_number <<= 8;
  last_received_sequence_number |= temp[2]; // lsb of sequence number is index 2 of the packet
  
  return SUCCESS;
}

IFG_StatusCode wait_for_REQ(void){
  IFG_StatusCode status_code = ERROR;
  uint8_t rx_count = 0;
  uint32_t temp1 = 0, temp2 = 0;
  if(rx_count == 0){
    status_code = Transport_receive(&temp1);
    if(status_code == SUCCESS){
      rx_count++;        
    }      
  }
  else{
    status_code = Transport_receive(&temp2);
    if(status_code == SUCCESS){
      rx_count = 0;
      // temp1, temp2 may have an REQ packet, lets find out
      status_code = validate_REQ(temp1, temp2);    
      if(status_code == SUCCESS){
        return SUCCESS; 
      }
    }            
  }
  
  return TIMEOUT;
}

IFG_StatusCode validate_REQ(uint32_t packet0, uint32_t packet1){
  IFG_StatusCode status_code = ERROR;
  uint8_t temp[8] = {0};
  uint8_t ii = 0;
  uint16_t sum = 0;
  temp[0] = (packet0 >> 24) & 0xff;
  temp[1] = (packet0 >> 16) & 0xff;
  temp[2] = (packet0 >>  8) & 0xff;
  temp[3] = (packet0 >>  0) & 0xff;  
  temp[4] = (packet1 >> 24) & 0xff;
  temp[5] = (packet1 >> 16) & 0xff;
  temp[6] = (packet1 >>  8) & 0xff;
  temp[7] = (packet1 >>  0) & 0xff;  

  for(ii = 0; ii < 8; ii++){
    sum += temp[ii]; 
  }
  
  if((sum & 0xff) != 0){
    return ERROR; 
  }
  
  // at least the checksum is correct
  // REQ packets are supposed to have a destination address of 0
  if(0 != temp[4]){ // destination address is index 4 in the packet
    return ERROR; 
  }
  
  // checksum is valid and I am the intended recipient
  if(PACKET_TYPE_REQ != temp[0]){ // packet type is index 0 of the packet
    return ERROR;
  }
  
  // looks good, lets go with it
  requesting_player_address     = temp[3];  // source address is index 3 of the packet
  last_received_sequence_number = temp[1];  // msb of sequence number is index 1 of the packet
  last_received_sequence_number <<= 8;
  last_received_sequence_number |= temp[2]; // lsb of sequence number is index 2 of the packet
  
  return SUCCESS;
}

#define MSG_TIMEOUT_DURATION_MS 600

IFG_StatusCode wait_for_MSG(){
  IFG_StatusCode status_code = ERROR;
  uint16_t rx_count = 0;
  uint32_t temp1 = 0, temp2 = 0;
  uint16_t payload1 = 0, payload2 = 0;
  uint8_t  byte1 = 0, byte2 = 0, byte3 = 0;  
  uint8_t message_payload_idx = 0;
  uint8_t field = 0; // toggles between 0 and 1
  
  while(awaiting_msg_duration() < MSG_TIMEOUT_DURATION_MS){
    // need to receive these packets in pairs to extract 3-bytes of payload = 1 player datum    
    
    if((rx_count & 1) == 0){
      status_code = Transport_receive(&temp1);
      if(status_code != SUCCESS){
        return ERROR;  
      }            
      rx_count++;              
    }
    else{
      status_code = Transport_receive(&temp2);
      if(status_code == SUCCESS){        
        // temp1, temp2 may have a MSG packet, lets find out
        status_code = validate_MSG(temp1, temp2);    
        if(status_code != SUCCESS){          
          return ERROR; 
        }
      }     
      
      if(field == 0){
        payload1 = (temp2 >> 8);
        payload1 &= 0x0fff; // 12-bits
      }
      else{ // field == 1
        payload2 = (temp2 >> 8);
        payload2 &= 0x0fff; // 12-bits     
     
        // now we can parse this into bytes
        byte1 = payload1 >> 4;
        byte2 = payload1 & 0x0f;
        byte2 <<= 4;
        byte2 |= ((payload2 & 0xf0) >> 4);
        byte3 = payload2 & 0xff;
        
        // and store them in the payload
        message_payload[message_payload_idx++] = byte1;
        message_payload[message_payload_idx++] = byte2;
        message_payload[message_payload_idx++] = byte3;
        
        if(byte1 == 0){ // this is a terminator packet
          return SUCCESS;
        }
      }
      
      field = 1 - field; // toggle field      
      rx_count++;      
    }
  }
  
  return TIMEOUT;
}

uint32_t awaiting_msg_duration(void){
  return (millis() - last_ack_timestamp);  
}

IFG_StatusCode validate_MSG(uint32_t packet0, uint32_t packet1){
  IFG_StatusCode status_code = ERROR;
  uint8_t temp[8] = {0};
  uint8_t ii = 0;
  uint16_t sum = 0;
  temp[0] = (packet0 >> 24) & 0xff;
  temp[1] = (packet0 >> 16) & 0xff;
  temp[2] = (packet0 >>  8) & 0xff;
  temp[3] = (packet0 >>  0) & 0xff;  
  temp[4] = (packet1 >> 24) & 0xff;
  temp[5] = (packet1 >> 16) & 0xff;
  temp[6] = (packet1 >>  8) & 0xff;
  temp[7] = (packet1 >>  0) & 0xff;  

  for(ii = 0; ii < 8; ii++){
    sum += temp[ii]; 
  }
  
  if((sum & 0xff) != 0){
    return ERROR; 
  }
  
  // at least the checksum is correct
  // am I the intended recipient?
  if(MY_ADDRESS != temp[4]){ // destination address is index 4 in the packet
    return ERROR; 
  }
  
  // is the sender the last person who sent a REQ
  if(requesting_player_address != temp[3]){ // source address is index 3 in the packet
    return ERROR; 
  }  
  
  // checksum is valid and I am the intended recipient
  if(PACKET_TYPE_MSG != temp[0]){ // packet type is index 0 of the packet
    return ERROR;
  }
  
  // looks good, lets go with it
  last_received_sequence_number = temp[1];  // msb of sequence number is index 1 of the packet
  last_received_sequence_number <<= 8;
  last_received_sequence_number |= temp[2]; // lsb of sequence number is index 2 of the packet
  
  return SUCCESS; 
}
