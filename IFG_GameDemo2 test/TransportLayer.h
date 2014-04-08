#ifndef ___TRANSPORT_LAYER_H___
#define ___TRANSPORT_LAYER_H___

#include "IFG_Utility.h"
#include <stdint.h>

void Transport_enable_receive(void);
void Transport_transmit(uint32_t value);
IFG_StatusCode Transport_receive(uint32_t * res); 

#endif
