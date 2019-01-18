#include<avr/sleep.h>
#include<avr/power.h>

#define LEDPIN 3

unsigned long secondsOfDay = 21600;
;
unsigned long minutesOfDay = 0;
unsigned int hoursOfDay = 0;
int ledBrightness = 0;


SIGNAL(TIMER1_COMPA_vect){
  secondsOfDay = secondsOfDay >= 86400 ? 0 : secondsOfDay + 1;
  hoursOfDay = secondsOfDay/(60 * 60);
  minutesOfDay = (secondsOfDay - (hoursOfDay * 60 * 60))/60;
}

void setup() {
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = 15624;// = (16*10^6) / (1*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);

  power_adc_disable();
  power_twi_disable();
  power_spi_disable();
  //power_timer0_disable();
  //power_timer2_disable();
  
  pinMode(LEDPIN, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  delay(2000); //just sleep for a long time. Timer interrupts are managing everything
  if (secondsOfDay >= (86400)) secondsOfDay = 0;
  if (hoursOfDay == 6) { //it's the morning, we should be incrementing
    ledBrightness < 255 ? ledBrightness++: 0;
    //Serial.println("It's morning");
  }
  if (hoursOfDay == 7){
    ledBrightness = 0;
  }
  if (hoursOfDay == 21 && minutesOfDay == 59){
    ledBrightness = 255;
  }
  if (hoursOfDay >= 22){
    ledBrightness > 0 ? ledBrightness-- : 0; //decrememnt if we need to
  }
  analogWrite(LEDPIN, ledBrightness);
  Serial.println(ledBrightness, DEC);
}
