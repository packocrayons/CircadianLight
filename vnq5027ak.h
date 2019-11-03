//1/2700 times the drive current through a 22k resistor to ground, paralleled with a 10uF (105) capacitor to stabilize. Protected with a 330k resistor, but since current doesn't flow into an ADC in this spherical cow, we don't take the protection resistor into account 
#define current_sense_multiplier 0.0006
#define P_GAIN 60 

#define REDLED_MAX_CURRENT 0.25
#define WHITELED_MAX_CURRENT 0.25

struct l{
	uint8_t pin;
	uint8_t sensePin;
	float brightnessTarget;
	float max_current;
	int16_t PWMvalue;
	float inc;
};

typedef struct l led;

void vnq_int(led* led, uint8_t pin, uint8_t sensePin);

void PIControl(led* led);
