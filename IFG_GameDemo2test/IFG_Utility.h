#ifndef ___IFG_UTILITY_H___
#define ___IFG_UTILITY_H___

#define NUM_PLAYERS_TOTAL 2  // up to 255
#define MY_ADDRESS        0x39 // 8-bit address - zero is not allowed
// TODO: Factor the address out as an initialization step through button press or something

// only define one of the __USING_ATMEGA328__ or __USING_ATTINY84__
#define __USING_ATMEGA328__
//#define __USING_ATTINY84__

#define DEBUG_ENABLED 

#ifdef DEBUG_ENABLED
  #include <HardwareSerial.h>
  extern HardwareSerial Serial;
  
  #define IFG_DEBUG_BEGIN(arg) do{ \
    Serial.begin(arg); \
  }while(0)
  
  #define IFG_DEBUG_PRINT(arg) do{  \
    Serial.print(arg); \
  }while(0)
  
 #define IFG_DEBUG_PRINT_HEX(arg) do{  \
    Serial.print(arg,HEX); \
  }while(0)

  #define IFG_DEBUG_PRINTLN(arg) do{  \
    Serial.println(arg); \
  }while(0)  
  
  #define IFG_DEBUG_PRINTLN_HEX(arg) do{  \
    Serial.println(arg, HEX); \
  }while(0)  
 
  #define IFG_DEBUG_ZERO_PAD8(arg){ \
    if((arg & 0xf0000000) == 0){ \
       IFG_DEBUG_PRINT(0); \
    } \
    else if((arg & 0xff000000) == 0){ \
       IFG_DEBUG_PRINT(0); \
       IFG_DEBUG_PRINT(0); \
    } \
    else if((arg & 0xfff00000) == 0){ \
       IFG_DEBUG_PRINT(0); \
       IFG_DEBUG_PRINT(0); \
       IFG_DEBUG_PRINT(0); \
    } \
    else if((arg & 0xffff0000) == 0){ \
       IFG_DEBUG_PRINT(0); \
       IFG_DEBUG_PRINT(0); \
       IFG_DEBUG_PRINT(0); \
       IFG_DEBUG_PRINT(0); \
    } \
    else if((arg & 0xfffff000) == 0){ \
       IFG_DEBUG_PRINT(0); \
       IFG_DEBUG_PRINT(0); \
       IFG_DEBUG_PRINT(0); \
       IFG_DEBUG_PRINT(0); \
       IFG_DEBUG_PRINT(0); \
    } \
    else if((arg & 0xffffff00) == 0){ \
       IFG_DEBUG_PRINT(0); \
       IFG_DEBUG_PRINT(0); \
       IFG_DEBUG_PRINT(0); \
       IFG_DEBUG_PRINT(0); \
       IFG_DEBUG_PRINT(0); \
       IFG_DEBUG_PRINT(0); \
    } \
    else if((arg & 0xfffffff0) == 0){ \
       IFG_DEBUG_PRINT(0); \
       IFG_DEBUG_PRINT(0); \
       IFG_DEBUG_PRINT(0); \
       IFG_DEBUG_PRINT(0); \
       IFG_DEBUG_PRINT(0); \
       IFG_DEBUG_PRINT(0); \
       IFG_DEBUG_PRINT(0); \
    } \
    IFG_DEBUG_PRINT_HEX(arg);    \
  }while(0)
#else
  // these could possibly be implemented using SoftwareSerial
  #define IFG_DEBUG_BEGIN(arg) do{}while(0)
  #define IFG_DEBUG_PRINT(arg) do{}while(0) 
  #define IFG_DEBUG_PRINT_HEX(arg) do{} while(0)
  #define IFG_DEBUG_PRINTLN(arg) do{}while(0)   
  #define IFG_DEBUG_PRINTLN_HEX(arg) do{}while(0)  
  #define IFG_DEBUG_ZERO_PAD8(arg) do{} while(0)
#endif


typedef enum{
  IFG_SUCCESS = 0,   
  IFG_TIMEOUT = 1,
  IFG_ERROR = 2
} IFG_StatusCode;

void spin_forever(void);

#endif
