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
  irsend.sendSony(value, 12);
  delay(20);
}

//TODO: add timeout argument, don't insta-fail
IFG_StatusCode Transport_receive(uint32_t * res){
  IFG_StatusCode status_code = ERROR;
  if (irrecv.decode(&results)) {
    irrecv.resume(); // Receive the next value     
    *res = results.value;
    status_code = SUCCESS;
  }
  else{
    status_code = TIMEOUT; 
  }
  return status_code;
}



