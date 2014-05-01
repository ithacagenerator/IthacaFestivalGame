#include "IFG_Packet.h"
#include "IFG_Utility.h"

// packet structure is
// -----------------------
// [0]  [1-byte] PACKET TYPE
// [1]  [1-byte] PACKET SEQUENCE NUMBER MSB
// [2]  [1-byte] PACKET SEQUENCE NUMBER LSB
// [3]  [1-byte] SOURCE ADDRESS
// -----------------------
// [4]  [1-byte] DESTINATION ADDRESS
// [5]  [4-bit ] PAYLOAD LENGTH
// [5/6][12-bit] PAYLOAD BODY
// [7]  [1-byte] CHECKSUM

#define PACKET_SIZE_BYTES  8
uint8_t packet_buffer[PACKET_SIZE_BYTES] = {0}; // each packet is 64-bits

void Packet_set_type(uint8_t packet_type){
  packet_buffer[0] = packet_type;
  Packet_update_checksum();
}

void Packet_set_sequence_number(uint16_t sequence_number){
  packet_buffer[1] = (sequence_number >> 8) & 0xff;
  packet_buffer[2] = (sequence_number >> 0) & 0xff;
  Packet_update_checksum();  
}

void Packet_set_source_address(uint8_t address){
  packet_buffer[3] = address;
  Packet_update_checksum();  
}

void Packet_set_destination_address(uint8_t address){
  packet_buffer[4] = address;  
  Packet_update_checksum();  
}

void Packet_set_payload_length(uint8_t payload_length){
  uint8_t temp = packet_buffer[5]; // store the current value
  temp &= 0x0f;                    // clear the first four bits
  payload_length &= 0x0f;          // make sure the length is 4-bits 
  temp |= (payload_length << 4);   // stuff the bits into the field
  packet_buffer[5] = temp;         // write back the results to the buffer
  Packet_update_checksum();  
}

void Packet_set_payload_body(uint16_t body){
  uint8_t temp = packet_buffer[5]; // store the current value
  temp &= 0xf0;                    // clear the lower 4-bits
  temp |= (body >> 8) & 0x0f;      // stuff in the top 4-bits of body 
  packet_buffer[5] = temp;         // write back the results to the buffer
  packet_buffer[6] = (body & 0xff);  
  Packet_update_checksum(); 
}

void Packet_update_checksum(void){  
  uint8_t ii = 0;
  uint16_t sum = 0;
  for(ii = 0; ii < 7; ii++){
    sum += packet_buffer[ii];
  }
  
  // the checksum is the value one must add to the sum
  // such that the resulting sum (mod 256) is zero
  packet_buffer[7] = 0x0100 - (sum & 0xff);
}

uint8_t Packet_get_byte(uint8_t index){
  if(index < PACKET_SIZE_BYTES){
    return packet_buffer[index];
  }
  return 0;
}

void Print_packet (void) {
  for (int i=0; i < PACKET_SIZE_BYTES; i++) {
      IFG_DEBUG_PRINT_HEX(Packet_get_byte(i));
      IFG_DEBUG_PRINT(" ");
  }
  IFG_DEBUG_PRINTLN("");
}  
