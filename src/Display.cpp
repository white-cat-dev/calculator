#include "Display.h"


Display::Display() {
	RCC_APB2PeriphClockCmd(LCD_RCC_GPIO, ENABLE);

	GPIO_InitTypeDef initStruct;
	initStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	initStruct.GPIO_Speed = GPIO_Speed_50MHz;
	initStruct.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_12 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_0 | GPIO_Pin_1;

	GPIO_Init(LCD_PORT, &initStruct);

	int del = 99999;
	GPIO_ResetBits(LCD_PORT, LCD_RS_Pin);  // rs = 0
	delay(del);

	writeData(0b00110000); //init1
	delay(del);

	writeData(0b00110000); //init2
	delay(del);

	writeData(0b00110000); //init3
	delay(del);

	writeData(0b00111000); // function set  8bit 2line 5x8 dots
	delay(del);

	writeData(0b00001100); // display on + cursor underline + blinking
	delay(del);

	writeData(0b00000001); //clear
	delay(del);

	writeData(0b00000110); //entry mode set
	delay(del);

	writeData(0b00000010); // return to home
	delay(del);

	GPIO_SetBits(LCD_PORT,LCD_RS_Pin);  //rs = 1
}

void Display::delay(unsigned int s) {
	while(--s > 0) {
		__NOP();
	}
}

void Display::writeData(u16 data) {
	GPIO_SetBits(LCD_PORT, data | LCD_E_Pin);
	delay(0xFFFF);
	GPIO_ResetBits(LCD_PORT,LCD_E_Pin | data);
}

void Display::printStr(char* str) {
	do {
		writeData(*str);
	} while(*++str);
}

void Display::writeCmd(u16 cmd) {
	GPIO_ResetBits(LCD_PORT,LCD_RS_Pin);
	writeData(cmd);
	GPIO_SetBits(LCD_PORT,LCD_RS_Pin);
}

void Display::setCursor(int line,int pos) {
	pos |= 0b10000000;
	if (line == 1) {
		pos += 0x40;
	}
	writeCmd(pos);
}

void Display::printNumber(char* number) {
	for (int i = 0; i < 15; i++)
		writeData(number[i]);
}

void Display::printFirstNumber(char* number) {
	setCursor(0, 0);
	printNumber(number);
}

void Display::printSecondNumber(char* number) {
	setCursor(1, 0);
	printNumber(number);
}

void Display::printOperation(char operation) {
	setCursor(0, 15);
	writeData(operation);
}

void Display::printError(int error) {
	switch (error) {
		case 0:
			setCursor(1, 0);
			printStr("Err long num   ");
			break;
		case 1:
			setCursor(0, 0);
			printStr("Err long num   ");
			break;
		case 2:
			setCursor(1, 0);
			printStr("Err div by zero");
			break;
		case 3:
			setCursor(0, 0);
			printStr("Err long result");
			break;
		case 4:
			setCursor(1, 0);
			printStr("Err enter num  ");
			break;
		default:
			setCursor(0, 0);
			printStr("Err undefined  ");
	}
	delay(10000000);
}
