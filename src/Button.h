#ifndef BUTTON_H_
#define BUTTON_H_

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

class Button {
private:
	GPIO_TypeDef* port;
	uint16_t pin;

public:
	Button(GPIO_TypeDef* port, uint16_t pin);
	bool read();
};

#endif
