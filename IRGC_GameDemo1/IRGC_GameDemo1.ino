#include <IRremote.h>

#define PLAYER3

#define NUM_PLAYERS 3

#ifdef PLAYER1
#define MY_PLAYER_NUMBER 1
#define START_WITH_BALL  1
#endif
#ifdef PLAYER2
#define MY_PLAYER_NUMBER 2
#define START_WITH_BALL  0
#endif
#ifdef PLAYER3
#define MY_PLAYER_NUMBER 3
#define START_WITH_BALL  0
#endif

#define SENDING_TIMEOUT 10000
#define ACK_TIMEOUT     1000 // i changed this to 500, it was 1000
#define TX_TIMEOUT      250

//The IR transmit pin is defined by what timer is chosen in the library IRRemoteint.h
//Since we are using an ATTINY84 chip we have to use timer 1 which binds to arduino pin 6
//If we were using ATMEGA328 timer 1 binds to pin 9

int have_ball = START_WITH_BALL;
int caught_ball = START_WITH_BALL;
int time_received_ball = 0;
int begin_transmit_timestamp = 0;
int ball_number = MY_PLAYER_NUMBER;


int RECV_PIN = 4;
IRrecv irrecv(RECV_PIN);
IRsend irsend;
decode_results results;

int red_led_pin = 10;
int blue_led_pin = 12;
int green_led_pin = 5;

void led_on();
void led_off();
void led_done();
void led_red();

void setup()
{
  Serial.begin(115200);
  Serial.println(F("Hello World"));
  irrecv.enableIRIn(); // Start the receiver 
  time_received_ball = 0; 
}

void loop() {
  
  if(have_ball){
    led_on();
    
    int new_ball_number = MY_PLAYER_NUMBER + 1;
    if(new_ball_number > NUM_PLAYERS){
      new_ball_number = 1;
    }
    
    int next_ball_number = new_ball_number + 1;
     if(next_ball_number > NUM_PLAYERS){
      next_ball_number = 1;
    }   
    
    
    while((millis() - time_received_ball) <= SENDING_TIMEOUT){
      
      begin_transmit_timestamp = millis(); 
      Serial.println(F("TX: Send BURST"));   
      while(millis() - begin_transmit_timestamp <= TX_TIMEOUT){
        uint32_t timestamp = millis();
        irsend.sendSony(new_ball_number, 12);
        Serial.print(millis() - timestamp);
        Serial.println(F(" ms"));
        delay(100);   
      } 
      
      irrecv.enableIRIn();
      
      begin_transmit_timestamp = millis();       
      Serial.println(F("TX: Awaiting ACK"));         
      while((millis() - begin_transmit_timestamp) <= ACK_TIMEOUT){
        
        if (irrecv.decode(&results)) {
          Serial.println(results.value, HEX);
          irrecv.resume(); // Receive the next value     
          if(results.value == next_ball_number){
            have_ball = 0;
            Serial.println(F("Received ACK"));            
            delay(TX_TIMEOUT);
            Serial.println(F("Going to RX"));            
            return;
          }        
          else{
            Serial.print(F("Received Unexpected Value: "));
            Serial.println(results.value, HEX);
          }
        }
        
      }
    }
    
    for(;;){
      led_red();
    }
 
  }
  else{ //if(!have_ball && !caught_ball){   
    led_off();    

    if (irrecv.decode(&results)) {
      irrecv.resume(); // Receive the next value     
      if(results.value == MY_PLAYER_NUMBER){
        Serial.println(F("RX: Received BURST"));
        have_ball = 1; 
        time_received_ball = millis();
        
        delay(TX_TIMEOUT);
        Serial.println(F("Going to TX"));            
        
      }
      else{
        Serial.print(F("Unexpected Value: "));
        Serial.println(results.value, HEX);        
      }
    }
    
  }
  /*
  else if(!have_ball && caught_ball){
    led_done();    
  }
  */
   
}

void led_on(){
    digitalWrite(red_led_pin, HIGH);
    digitalWrite(blue_led_pin, HIGH);
    digitalWrite(green_led_pin, HIGH); 
}

void led_off(){
    digitalWrite(red_led_pin, LOW);
    digitalWrite(blue_led_pin, LOW);
    digitalWrite(green_led_pin, LOW); 
}

void led_done(){
    digitalWrite(red_led_pin, LOW);
    digitalWrite(blue_led_pin, LOW);
    digitalWrite(green_led_pin, HIGH);  
}

void led_red(){
    digitalWrite(red_led_pin, HIGH);
    digitalWrite(blue_led_pin, LOW);
    digitalWrite(green_led_pin, LOW);    
}
