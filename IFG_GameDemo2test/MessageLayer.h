#ifndef ___MESSAGE_LAYER_H___
#define ___MESSAGE_LAYER_H___

#include "IFG_Utility.h"
#include "IFG_Packet.h"
#include <stdint.h>

#define COUNTER_BYTES_PER_PLAYER  2
#define NUM_ADDRESS_BYTES         1
// payload is an array of {address[1 byte], count_msb[1 byte], count_lsb[1byte]} x NUM_PLAYERS
#define MESSAGE_PAYLOAD_SIZE (NUM_PLAYERS_TOTAL * 3)

IFG_StatusCode attempt_message_receive(void);
IFG_StatusCode attempt_message_transfer(void);
void send_REQ(void);
void send_MSG(void);
void send_ACK(void);
void send_first_half_packet(void);
void send_second_half_packet(void);
void send_packet(void);
IFG_StatusCode wait_for_Packet(uint8_t packet_type, uint32_t * p_first_half, uint32_t * p_second_half);
IFG_StatusCode wait_for_ACK(void);
IFG_StatusCode wait_for_REQ(void);
IFG_StatusCode wait_for_MSG(void);
uint32_t awaiting_ack_duration(void);
uint32_t awaiting_msg_duration(void);
IFG_StatusCode validate_and_decode_Packet(uint8_t expected_packet_type, uint32_t first_half, uint32_t second_half);
IFG_StatusCode validate_and_decode_ACK(uint32_t first_half, uint32_t second_half);
IFG_StatusCode validate_and_decode_REQ(uint32_t first_half, uint32_t second_half);
IFG_StatusCode validate_and_decode_MSG(uint32_t first_half, uint32_t second_half);
uint16_t next_sequence_number(void);

// packet field extraction helpers from half-packets [no fields fragment across half-packet boundaries]
uint8_t extract_packet_type(uint32_t first_half_packet);
uint16_t extract_sequence_number(uint32_t first_half_packet);
uint8_t extract_source_address(uint32_t first_half_packet);
uint8_t extract_destination_address(uint32_t second_half_packet);
uint16_t extract_payload_body(uint32_t second_half_packet);
uint8_t extract_checksum(uint32_t second_half_packet);
uint8_t calculate_checksum(uint32_t first_half_packet, uint32_t second_half_packet);
void push_message_payload(uint8_t value);
#endif
