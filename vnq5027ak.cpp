//We put a driver for your driver so you can drive your driver with a driver.
//VNQ5027ak driver

#include <Arduino.h>
#include <stdint.h>
#include "vnq5027ak.h"

void vnq_init(led* led, uint8_t pin, uint8_t sensePin){
	uint8_t i;
	led->pin = pin;
	led->sensePin = sensePin;
	pinMode(pin, OUTPUT); //turn the LED on to measure current
	for (i = 0; i < 255; ++i){ //wait for current to stabilize
		led->max_current += ((float)analogRead(sensePin)/255);
	}	
}


void PIControl(led* led){
  int analogValue = analogRead(led->sensePin);
  float discrepancy = led->brightnessTarget - (float)( analogValue * current_sense_multiplier);
  led->PWMvalue += discrepancy * P_GAIN;
  if (led->PWMvalue > 255) led->PWMvalue = 255; //the reason it's an int16 is because it's an easy way to detect over/underflow
  if (led->PWMvalue < 0) led->PWMvalue = 0;
  analogWrite(led->pin, led->PWMvalue);
}
