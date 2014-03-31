#ifndef ___IFG_UTILITY_H___
#define ___IFG_UTILITY_H___

#define NUM_PLAYERS_TOTAL 4  // up to 255
#define MY_ADDRESS        72 // 8-bit address - zero is not allowed

// only define one of the __USING_ATMEGA328__ or __USING_ATTINY84__
#define __USING_ATMEGA328__
//#define __USING_ATTINY84__

//#define DEBUG_ENABLED 

#ifdef DEBUG_ENABLED  
  #include <HardwareSerial.h>
  extern HardwareSerial Serial;
  
  #define IFG_DEBUG_BEGIN(arg) do{ \
    Serial.begin(arg); \
  }while(0)
  
  #define IFG_DEBUG_PRINT(arg) do{  \
    Serial.print(arg); \
  }while(0)

  #define IFG_DEBUG_PRINTLN(arg) do{  \
    Serial.println(arg); \
  }while(0)  
#else
  // these could possibly be implemented using SoftwareSerial
  #define IFG_DEBUG_BEGIN(arg) do{}while(0)
  #define IFG_DEBUG_PRINT(arg) do{}while(0) 
  #define IFG_DEBUG_PRINTLN(arg) do{}while(0)   
#endif


typedef enum{
  SUCCESS,   
  TIMEOUT,
  ERROR
} IFG_StatusCode;

void spin_forever(void);

#endif
