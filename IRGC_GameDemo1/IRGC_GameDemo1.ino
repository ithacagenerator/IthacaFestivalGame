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
#define ACK_TIMEOUT     500
#define TX_TIMEOUT      250

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
int blue_led_pin = 9;
int green_led_pin = 5;

void led_on();
void led_off();
void led_done();
void led_red();

void setup()
{
  Serial.begin(115200);
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
    
    
    while(millis() - time_received_ball <= SENDING_TIMEOUT){
      
      begin_transmit_timestamp = millis();    
      while(millis() - begin_transmit_timestamp <= TX_TIMEOUT){
        irsend.sendSony(new_ball_number, 12); // Sony TV power code
        delay(10);   
      } 
      
      irrecv.enableIRIn();
      
      begin_transmit_timestamp = millis(); 
      while(millis() - begin_transmit_timestamp <= ACK_TIMEOUT){
        
        if (irrecv.decode(&results)) {
          Serial.println(results.value, HEX);
          irrecv.resume(); // Receive the next value     
          if(results.value == next_ball_number){
            have_ball = 0;
            delay(TX_TIMEOUT);
            return;
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
      Serial.println(results.value, HEX);
      irrecv.resume(); // Receive the next value     
      if(results.value == MY_PLAYER_NUMBER){
        have_ball = 1; 
        time_received_ball = millis();
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
    analogWrite(red_led_pin, 255);
    analogWrite(blue_led_pin, 255);
    analogWrite(green_led_pin, 255); 
}

void led_off(){
    analogWrite(red_led_pin, 0);
    analogWrite(blue_led_pin, 0);
    analogWrite(green_led_pin, 0); 
}

void led_done(){
    analogWrite(red_led_pin, 0);
    analogWrite(blue_led_pin, 0);
    analogWrite(green_led_pin, 255);  
}

void led_red(){
    analogWrite(red_led_pin, 255);
    analogWrite(blue_led_pin, 0);
    analogWrite(green_led_pin, 0);    
}
