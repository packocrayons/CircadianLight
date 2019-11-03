#include<avr/sleep.h>
#include<avr/power.h>
#include"vnq5027ak.h"

#define OCR1A_val 1562

led r, l;

led* redLed = &r;
led* whiteLed = &l;
unsigned long secondsOfDay = 79140;
unsigned long minutesOfDay = 0;
unsigned int hoursOfDay = 0;
uint8_t interrupted=0;
uint8_t runPID;
int pin13=0;

void setup() {
  cli();
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = OCR1A_val;// = (16*10^6) / (1*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
//
//  power_adc_disable();
//  power_twi_disable();
//  power_spi_disable(); //turning down power usage

  pinMode(13,OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);

  sei();

  vnq_init(redLed, 3, 0);
  vnq_init(whiteLed, 11, 1);
  
  Serial.begin(9600);

}

void loop() {
  if (secondsOfDay >= (86400)) secondsOfDay = 0;
  if (hoursOfDay == 6) { //it's the morning, we should be incrementing
    redLed->inc = 0.1;
    whiteLed->inc = 0.1;
  }
  if (hoursOfDay == 7){ //only for a bit, lights off for the day
    redLed->inc = 0;
    whiteLed->inc = 0;
    whiteLed->brightnessTarget = 0;
    redLed->brightnessTarget = 0;
  }
  if (hoursOfDay == 21 && minutesOfDay == 59){ //night time, start up
    whiteLed->brightnessTarget = whiteLed->max_current;
    whiteLed->inc = -0.1; //white fades out, red fades in
    redLed->inc=0.1;  
  }
  if (hoursOfDay >= 22){
    redLed->inc = -0.1;
    whiteLed->inc = 0;
  }
  if (runPID){
    PIControl(redLed);
    PIControl(whiteLed);
    runPID=0;
  }
  Serial.print("redLed->inc: "); Serial.println(redLed->inc);
  //delay(100);

}


SIGNAL(TIMER1_COMPA_vect){ //1 second timer fire
  if(++interrupted == 10){
	  secondsOfDay = secondsOfDay >= 86400 ? 0 : secondsOfDay + 1;
	  hoursOfDay = secondsOfDay/(60 * 60);
	  minutesOfDay = (secondsOfDay/60) % 60;
  
  	redLed->brightnessTarget += redLed->inc; //I can't imagine increments, loads and stores aren't atomic on avr architecture
   	whiteLed->brightnessTarget += whiteLed->inc;
  
  	if (redLed->brightnessTarget >= redLed->max_current - 0.01 || redLed->brightnessTarget <= 0.01){ //note this is POST increment, this means we're done the sequence
		  redLed->inc = 0;
  	}
  
	  if (whiteLed->brightnessTarget >= whiteLed->max_current - 0.01 || whiteLed->brightnessTarget <= 0.01){ //note this is POST increment, this means we're done the sequence
  		whiteLed->inc = 0;
  	}
    interrupted = 0;
  }
  runPID = 1;
}
