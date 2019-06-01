#ifndef LED_H_
#define LED_H_

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

class Led {
private:
	GPIO_TypeDef* port;
	uint16_t pin;
	bool state;

public:
	Led(GPIO_TypeDef* port, uint16_t pin);
	void on();
	void off();
	void invert();
};

#endif
