#include "Button.h"

Button::Button(GPIO_TypeDef* port, uint16_t pin) {
	this->port = port;
	this->pin = pin;

	GPIO_InitTypeDef initStruct;
	initStruct.GPIO_Pin = pin;
	initStruct.GPIO_Mode = GPIO_Mode_IPU;
	initStruct.GPIO_Speed = GPIO_Speed_10MHz;

	GPIO_Init(port, &initStruct);
}

bool Button::read() {
	return !GPIO_ReadInputDataBit(port, pin);
}
