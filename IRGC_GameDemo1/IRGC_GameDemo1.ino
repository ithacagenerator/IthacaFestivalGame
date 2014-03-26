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

#define TOTAL_SENDING_TIME 10000
#define TX_TIMEOUT      250

int time_received_ball = 0;
boolean I_have_the_ball;

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
  if (MY_PLAYER_NUMBER == 1)
    I_have_the_ball = true;
   else
     I_have_the_ball =false;
   
  Serial.begin(115200);
  Serial.println(F("Hello World"));
  irrecv.enableIRIn(); // Start the receiver 
 // time_received_ball = 0; 
}

void loop() {
  
  if (I_have_the_ball)
  {
    led_on();
    
    int ball_to_send = next_ball(MY_PLAYER_NUMBER);
    int receipt_confirmation_number = next_ball(ball_to_send);
    
    boolean receivedConfirmation=false;
    while(((millis() - time_received_ball) <= TOTAL_SENDING_TIME)&& !receivedConfirmation)
    {
      sendBall(ball_to_send);
      delay(25);   
      
      //listen for 100 miliseconds
      receivedConfirmation=listenForBall(100,receipt_confirmation_number);
    } 
      
    if (!receivedConfirmation)
    {
      //game over
      for(;;){
        led_red();
      }
 
    }
  }
   else  //I don't have the ball
   {  
    led_off();    
    
    while (!I_have_the_ball)
    {
                        //listen for 100 milliseconds
      I_have_the_ball = listenForBall(100,MY_PLAYER_NUMBER);
      if (I_have_the_ball)
      {
        time_received_ball = millis();
      }
    }
    
  }
   
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

int next_ball(int ball_number)
{
    ball_number = MY_PLAYER_NUMBER + 1;
    if(ball_number > NUM_PLAYERS){
      ball_number = 1;
    }
    return ball_number;
}

void sendBall(int ballNum)
{

     irsend.sendSony(ballNum, 12); // Sony TV power code
     delay(10);   
}

boolean listenForBall(int milliSecondsToListen,int ballToListenFor)
{
  
      int startTimeInMillis = millis();
      
      irrecv.enableIRIn();
      
      while((millis() - startTimeInMillis) <= milliSecondsToListen)
      {
        if (irrecv.decode(&results)) 
        {
          Serial.println(results.value, HEX);
          irrecv.resume(); // Receive the next value     
          if(results.value == ballToListenFor){
            delay(TX_TIMEOUT);  //why?
            return  true;
          }   
        }  
       }
       return false;
}
