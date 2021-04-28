#include <Arduino.h>
#include "io.h"
extern void initIO(void) {
	pinMode(dirEnable, OUTPUT);
	pinMode(sens1, INPUT_PULLUP);
	pinMode(sens2, INPUT_PULLUP);
	pinMode(sens3, INPUT_PULLUP);
	pinMode(sens4, INPUT_PULLUP);
	pinMode(greenLed, OUTPUT);
	pinMode(redLed, OUTPUT);
	pinMode(onSwitch, INPUT_PULLUP);
	pinMode(recordSwitch, INPUT_PULLUP);
	pinMode(power, OUTPUT);
	pinMode(dirLeft, OUTPUT);
	pinMode(dirRight, OUTPUT);
	pinMode(channelSelect, INPUT);
	pinMode(speedKnob, INPUT);
	pinMode(currentSens, INPUT);
}