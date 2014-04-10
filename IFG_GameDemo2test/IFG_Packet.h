#ifndef ___IFG_PACKET_H___
#define ___IFG_PACKET_H___

#include <stdint.h>

#define NUM_PAYLOAD_BITS_PER_PACKET 12

// valid values for PACKET TYPE field
#define PACKET_TYPE_REQ 0x73
#define PACKET_TYPE_ACK 0xAC
#define PACKET_TYPE_MSG 0x43

void Packet_set_type(uint8_t packet_type);
void Packet_set_sequence_number(uint16_t sequence_number);
void Packet_set_source_address(uint8_t address);
void Packet_set_destination_address(uint8_t address);
void Packet_set_payload_length(uint8_t payload_length);
void Packet_set_payload_body(uint16_t body);
void Packet_update_checksum(void);
void Print_packet(void);
uint8_t Packet_get_byte(uint8_t index);



#endif
