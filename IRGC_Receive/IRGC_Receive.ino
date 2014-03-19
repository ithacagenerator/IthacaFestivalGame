#define TIMER_RESET
#define TIMER_ENABLE_PWM     (TCCR2A |= _BV(COM2B1))
#define TIMER_DISABLE_PWM    (TCCR2A &= ~(_BV(COM2B1)))
#define TIMER_ENABLE_INTR    (TIMSK2 = _BV(OCIE2A))
#define TIMER_DISABLE_INTR   (TIMSK2 = 0)
#define TIMER_INTR_NAME      TIMER2_COMPA_vect
#define TIMER_CONFIG_KHZ(val) ({ \
  const uint8_t pwmval = SYSCLOCK / 2000 / (val); \
  TCCR2A = _BV(WGM20); \
  TCCR2B = _BV(WGM22) | _BV(CS20); \
  OCR2A = pwmval; \
  OCR2B = pwmval / 3; \
})
#define TIMER_COUNT_TOP      (SYSCLOCK * USECPERTICK / 1000000)
#if (TIMER_COUNT_TOP < 256)
#define TIMER_CONFIG_NORMAL() ({ \
  TCCR2A = _BV(WGM21); \
  TCCR2B = _BV(CS20); \
  OCR2A = TIMER_COUNT_TOP; \
  TCNT2 = 0; \
})

#define USECPERTICK 50  // microseconds per clock interrupt tick
#define _GAP 5000       // Minimum map between transmissions
#define GAP_TICKS (_GAP/USECPERTICK)

#define IR_RX_STATE_IDLE  0
#define IR_RX_STATE_SPACE 1
#define IR_RX_STATE_MARK  2
#define IR_RX_STATE_STOP  3

int ir_rx_pin = 5;
int input_tick_timer = 0;
int ir_rx_state = IR_RX_STATE_IDLE;

void setup(){
  
}

void loop(){
  
}

ISR(TIMER_INTR_NAME)
{
  TIMER_RESET;

  uint8_t irdata = (uint8_t)digitalRead(ir_rx_pin);

  input_tick_timer++; // One more 50us tick
  if (irparams.rawlen >= RAWBUF) {
    // Buffer overflow
    ir_rx_state = STATE_STOP;
  }
  switch(ir_rx_state) {
  case IR_RX_STATE_IDLE: // In the middle of a gap
    if (irdata == MARK) {
      if (input_tick_timer < GAP_TICKS) {
        // Not big enough to be a gap.
        input_tick_timer = 0;
      } 
      else {
        // gap just ended, record duration and start recording transmission
        irparams.rawlen = 0;
        irparams.rawbuf[irparams.rawlen++] = input_tick_timer;
        input_tick_timer = 0;
        ir_rx_state = STATE_MARK;
      }
    }
    break;
  case IR_RX_STATE_MARK: // timing MARK
    if (irdata == SPACE) {   // MARK ended, record time
      irparams.rawbuf[irparams.rawlen++] = input_tick_timer;
      input_tick_timer = 0;
      ir_rx_state = STATE_SPACE;
    }
    break;
  case IR_RX_STATE_SPACE: // timing SPACE
    if (irdata == MARK) { // SPACE just ended, record it
      irparams.rawbuf[irparams.rawlen++] = input_tick_timer;
      input_tick_timer = 0;
      ir_rx_state = STATE_MARK;
    } 
    else { // SPACE
      if (input_tick_timer > GAP_TICKS) {
        // big SPACE, indicates gap between codes
        // Mark current code as ready for processing
        // Switch to STOP
        // Don't reset timer; keep counting space width
        ir_rx_state = STATE_STOP;
      } 
    }
    break;
  case IR_RX_STATE_STOP: // waiting, measuring gap
    if (irdata == MARK) { // reset gap timer
      input_tick_timer = 0;
    }
    break;
  }

  if (irparams.blinkflag) {
    if (irdata == MARK) {
      BLINKLED_ON();  // turn pin 13 LED on
    } 
    else {
      BLINKLED_OFF();  // turn pin 13 LED off
    }
  }
}
