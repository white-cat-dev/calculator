#ifndef DISPLAY_H_
#define DISPLAY_H_

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"


#define LCD_PORT GPIOA
#define LCD_RCC_GPIO RCC_APB2Periph_GPIOA

#define LCD_E_Pin GPIO_Pin_12
#define LCD_RS_Pin GPIO_Pin_8


class Display {
public:
    //  const uint8_t lcd_2x16_decode[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

	Display();
	void delay(unsigned int s);
	void writeData(u16 data);
	void printStr(char* str);
	void writeCmd(u16 cmd);
	void setCursor(int line,int pos);
	void printNumber(char* number);
	void printFirstNumber(char* number);
	void printSecondNumber(char* number);
	void printOperation(char operation);
	void printError(int error);
};

#endif
