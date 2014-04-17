#include <Arduino.h>
#include "TransportLayer.h"
#include "IFG_Utility.h"
#include <IFG_IRremote.h>



#if defined(__USING_ATMEGA328__)
#define RECV_PIN 4
#elif defined(__USING_ATTINY84__)

#endif

static IFG_IRrecv irrecv(RECV_PIN);
static IFG_IRsend irsend;
static decode_results results;

void Transport_enable_receive(void){
  irrecv.enableIRIn(); 
}

void Transport_transmit(uint32_t value){
  irsend.sendSony(value, 32);
  delay(40);
}

// Transport_receive is called when there is a reasonable
// expectation that a transmission is going to be received
// the timeout *must* be at least the inter-transmit intra-
// packet delay + the transmit interval so at least about 65ms
// otherwise second half packets always timeout
#define IR_RX_TIMEOUT_MS 80

IFG_StatusCode Transport_receive(uint32_t * res){
  uint32_t timeout_timestamp = millis() + IR_RX_TIMEOUT_MS; // a time in the future

  while(millis() < timeout_timestamp){
    if (irrecv.decode(&results)) {
      irrecv.resume(); // Receive the next value  
      *res = results.value;
      return IFG_SUCCESS;
    }
  }
  
  return IFG_TIMEOUT;
}



