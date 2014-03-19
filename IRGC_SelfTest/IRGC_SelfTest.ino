
void initLED();
void initIR_Tx();
void initIR_Rx();

#define TEST_LED

int red_pin;
int green_pin;
int blue_pin;
int ir_tx_pin;
int ir_rx_pin;

void setup(){
  initLED();
  initIR_Tx();
  initIR_Rx();
}

void loop(){
  digitalWrite(ir_tx_pin, HIGH);
  delay(1000);
  
  digitalWrite(ir_tx_pin, LOW);  

#ifdef TEST_LED
  analogWrite(red_pin, 255);
  delay(1000);
  
  analogWrite(red_pin, 0);
  analogWrite(green_pin, 255);
  delay(1000);
  
  analogWrite(green_pin, 0);
  analogWrite(blue_pin, 255);
  delay(1000);

  analogWrite(blue_pin, 0);  
#else
  delay(1000);
#endif
}

void initLED(){
  red_pin = 8;
  blue_pin = 6;
  green_pin = 7;
  
  pinMode(red_pin, OUTPUT);
  pinMode(green_pin, OUTPUT);
  pinMode(blue_pin, OUTPUT);
  
  digitalWrite(red_pin, LOW);
  digitalWrite(green_pin, LOW);
  digitalWrite(blue_pin, LOW);
}

void initIR_Tx(){
  ir_tx_pin = 9;
  pinMode(ir_tx_pin, OUTPUT);
  digitalWrite(ir_tx_pin, LOW);
}

void initIR_Rx(){
  ir_rx_pin = 10;
  pinMode(ir_rx_pin, INPUT);  
}
