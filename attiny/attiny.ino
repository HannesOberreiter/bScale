#include <avr/sleep.h>
#include <avr/wdt.h>

//Used for Sleep Mode
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

int const pinGate = 0; //PIN for MOSFET GATE
int const readGate = 1; //PIN for MOSFET GATE

long const waitTime = 90000; //How long the mosfet is open

volatile boolean f_wdt=1; //Flag to open MOSFET 1 open 0 closed
volatile boolean f_done=0; //Flag when open state is done
volatile boolean f_setup=0; //Flag when open pins and set starting time

unsigned long startMillis = 0; //holds our starting time
volatile int count; //count the circles
int const circles = 900; // Amount of circle till gate will be opened, Watchdog = 8 sec per circle

void setup(){
  pinMode(pinGate,INPUT); // set all used port to intput to save power
  pinMode(readGate,INPUT); // set all used port to intput to save power

  count = circles - 1; //Init first circle on power up, so we dont need to wait for full circle to see if it works

  // CPU Sleep Modes
  // SM2 SM1 SM0 Sleep Mode
  // 0    0  0 Idle
  // 0    0  1 ADC Noise Reduction
  // 0    1  0 Power-down
  // 0    1  1 Power-save
  // 1    0  0 Reserved
  // 1    0  1 Reserved
  // 1    1  0 Standby(1)

  /*cbi( SMCR,SE );      // sleep enable, power down mode
  cbi( SMCR,SM0 );     // power down mode
  sbi( SMCR,SM1 );     // power down mode
  cbi( SMCR,SM2 );     // power down mode*/

  setup_watchdog(9); //9 = 8 Seconds, see Setup Watchdog for other values.
}


void loop(){
  if (f_wdt==1) {  // wait for timed out watchdog / flag is set when a watchdog timeout occurs
    if(circles <= count){
      //Count reached, lets start our MOSFET
      
      //Start with Setup our PINS and the millis to compare
      if(f_start==0){
        pinMode(pinGate,OUTPUT); //Set Gate as Output
        pinMode(readGate,INPUT_PULLUP); //Set Read Gate to Pullup will LOW when closed and HIGH when open
        unsigned long startMillis = millis(); //Snapshot of current time
        digitalWrite(pinGate, HIGH); //Output High to MOSFET to open the gate
        f_start = 1; //set setup flag as finished
      }

      //Setup is finished, lets see if we should stop open the MOSFET
      if(f_start==1){
        if(digitalRead(readGate) == LOW) f_done = 1; //We got a signal, lets start new sleep circle
        unsigned long currentMillis = millis(); //Our current Millis
        if ((unsigned long)(currentMillis - startMillis) >= waitTime) f_done = 1;  //Timeout time is reached, lets start new sleep circle
      }
      
      if(f_done==1){
        digitalWrite(pinGate, LOW);
        // set all used port to intput to save power
        pinMode(pinGate,INPUT); 
        pinMode(readGate,INPUT);
        f_start = 0;    // reset setup flag
        f_done = 0;     // reset open circle flag
        count = 0;      // reset sleep cycle count
        f_wdt=0;       // reset watchdog flag
        system_sleep(); // back to sleep little tiny
      }
    } else {
      //Count has not reached Circles yet, just go back to sleep and count up
      count++;
      f_wdt=0;       // reset watchdog flag
      system_sleep(); // back to sleep little tiny
    }
  }
}


// set system into the sleep state
// system wakes up when watchdog is timed out
void system_sleep() {

  cbi(ADCSRA,ADEN);                    // switch Analog to Digitalconverter OFF

  set_sleep_mode(SLEEP_MODE_PWR_DOWN); // sleep mode is set here
  sleep_enable();

  sleep_mode();                        // System sleeps here
  sleep_disable();                     // System continues execution here when watchdog timed out
  sbi(ADCSRA,ADEN);                    // switch Analog to Digitalconverter ON

}

// 0=16ms, 1=32ms,2=64ms,3=128ms,4=250ms,5=500ms
// 6=1 sec,7=2 sec, 8=4 sec, 9= 8sec
void setup_watchdog(int ii) {
  byte bb;
  int ww;
  if (ii > 9 ) ii=9;
  bb=ii & 7;
  if (ii > 7) bb|= (1<<5);
  bb|= (1<<WDCE);
  ww=bb;
  MCUSR &= ~(1<<WDRF);
  // start timed sequence
  WDTCR  |= (1<<WDCE) | (1<<WDE);
  // set new watchdog timeout value
  WDTCR  = bb;
  WDTCR  |= _BV(WDIE);
}

// Watchdog Interrupt Service / is executed when  watchdog timed out
ISR(WDT_vect) {
  f_wdt=1;  // set global flag
}
