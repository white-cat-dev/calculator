#include "Led.h"

Led::Led(GPIO_TypeDef* port, uint16_t pin) {
	this->port = port;
	this->pin = pin;

	GPIO_InitTypeDef initStruct;
	initStruct.GPIO_Pin = pin;
	initStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	initStruct.GPIO_Speed = GPIO_Speed_10MHz;

	GPIO_Init(port, &initStruct);
}

void Led::off() {
	GPIO_WriteBit(port, pin, Bit_SET);
}

void Led::on() {
	GPIO_WriteBit(port, pin, Bit_RESET);
}

void Led::invert() {
	port->ODR ^= pin;
}
