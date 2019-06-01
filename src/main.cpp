#include <stdio.h>
#include <stdlib.h>
#include "diag/Trace.h"

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

#include "Display.h"
#include "Button.h"
#include "Led.h"


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"



void delay(unsigned int s){
	while(--s > 0) {
		__NOP();
	}
}


int clearNumber(char* number) {
	for (int i = 0; i < 15; i++) {
		number[i] = ' ';
	}
	return 0;
}

int checkNumber(char* number, int numberSize) {
	if ((number[0] == '0') && (number[1] != ' ')) {
		for (int i = 1; i < 15; i++)
			number[i - 1] = number[i];
		number[14] = ' ';
		numberSize -= 1;
	}
	return numberSize;
}


int setNumber(char* number, long long result) {
	if (result == 0) {
		clearNumber(number);
		number[0] = '0';
		return 1;
	}
	bool setMinus = false;
	if (result < 0) {
		setMinus = true;
		result = -result;
	}

	int numberSize = 0;
	char digits[15];
	while (result > 0) {
		digits[numberSize++] = result % 10;
		result /= 10;
	}

	for (int i = 0; i < 15; i++) {
		if (numberSize > i) {
			number[i] = digits[numberSize - i - 1] + 48;
		}
		else {
			number[i] = ' ';
		}
	}
	if (setMinus) {
		for (int i = 14; i >= 1; i--)
			number[i] = number[i-1];
		number[0] = '-';
		numberSize += 1;
	}

	return numberSize;
}


long long calculate(char* firstNumber, char operation, char* secondNumber) {
	long long result = 0;
	long long first = atoll(firstNumber);
	long long second = atoll(secondNumber);

	switch (operation) {
		case 1:
			result = first + second;
			break;
		case 2:
			result = first - second;
			break;
		case 3:
			result = first * second;
			break;
		case 4:
			result = first / second;
			break;
	}

	return result;
}


char buffer[30] = {'\0'};

void usartInit() {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

	// Tx
	GPIO_InitTypeDef initStruct;
	initStruct.GPIO_Pin = GPIO_Pin_9;
	initStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	initStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &initStruct);

	// Rx
	initStruct.GPIO_Pin = GPIO_Pin_10;
	initStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	initStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &initStruct);

	USART_InitTypeDef usartInitStruct;
	usartInitStruct.USART_BaudRate = 115200;
	usartInitStruct.USART_WordLength = USART_WordLength_8b;
	usartInitStruct.USART_StopBits = USART_StopBits_1;
	usartInitStruct.USART_Parity = USART_Parity_No;
	usartInitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	usartInitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &usartInitStruct);

	USART_Cmd(USART1, ENABLE);
}


void send(char* charBuffer, unsigned int count) {
	while (count--) {
		USART_SendData(USART1, *charBuffer++);
		while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}
	}
}



int main(int argc, char* argv[]) {
	usartInit();

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitTypeDef initStruct;
	initStruct.GPIO_Pin = GPIO_Pin_4;
	initStruct.GPIO_Mode = GPIO_Mode_IPD;
	initStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &initStruct);

	initStruct.GPIO_Pin = GPIO_Pin_3;
	initStruct.GPIO_Mode = GPIO_Mode_IPD;
	initStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &initStruct);

	bool enc1 = 0;
	bool enc2 = 0;
	bool enc1Prev = 0;
	bool enc2Prev = 0;


	char firstNumber[15];
	int firstNumberSize = clearNumber(firstNumber);

	char secondNumber[15];
	int secondNumberSize = clearNumber(secondNumber);

	char operations[5] = {' ', '+', '-', '*', '/'};
	int operation = 0;

	Display display;

	Led rows[] = {Led(GPIOB, GPIO_Pin_12),
				  Led(GPIOB, GPIO_Pin_5),
				  Led(GPIOB, GPIO_Pin_8),
				  Led(GPIOB, GPIO_Pin_9)};

	Button cols[] = {Button(GPIOB, GPIO_Pin_14),
					 Button(GPIOB, GPIO_Pin_13),
					 Button(GPIOB, GPIO_Pin_15)};
	int i;
	int currentCol = 0;
	int currentRow = 0;
	char currentButton = 0;

	firstNumberSize = setNumber(firstNumber, 0);
	display.printFirstNumber(firstNumber);

	bool buttonPressed = false;

	while (1) {
		enc1 = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4);
		enc2 = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_3);

		if ((enc1 != enc1Prev) || (enc2 != enc2Prev)) {
			if ((!enc1Prev && !enc2Prev) && (enc1 && !enc2)) {
				if (secondNumberSize > 0) {
					secondNumber[--secondNumberSize] = ' ';

					if ((secondNumberSize == 1) && (secondNumber[0] == '-'))
						secondNumber[--secondNumberSize] = ' ';

					display.printSecondNumber(secondNumber);
				}
				else if (operation > 0) {
					operation = 0;
					display.printOperation(operations[operation]);
				}
				else if (firstNumberSize > 0) {
					firstNumber[--firstNumberSize] = ' ';

					if ((firstNumberSize == 1) && (firstNumber[0] == '-'))
						firstNumber[--firstNumberSize] = ' ';

					if (firstNumberSize == 0)
						firstNumber[firstNumberSize++] = '0';

					display.printFirstNumber(firstNumber);
				}
			}
		}
		enc1Prev = enc1;
		enc2Prev = enc2;



		for (i = 0; i < 4; i++)
			rows[i].on();

		currentCol = -1;
		for (i = 0; i < 3; i++)
			if (cols[i].read())
				currentCol = i;

		if (currentCol >= 0) {
			if (buttonPressed)
				continue;
			buttonPressed = true;

			for (i = 0; i < 4; i++)
				rows[i].off();

			for (i = 0; i < 4; i++) {
				rows[i].on();
				if (cols[currentCol].read()) {
					currentRow = i;
					break;
				}
			}

			currentButton = currentRow * 3 + (currentCol + 1);

			if (currentButton == 11) {
				currentButton = '0';
				if (operation > 0) {
					if (secondNumberSize < 10)
						secondNumber[secondNumberSize++] = currentButton;
					else
						display.printError(0);
					secondNumberSize = checkNumber(secondNumber, secondNumberSize);
					display.printSecondNumber(secondNumber);
				}
				else {
					if (firstNumberSize < 10)
						firstNumber[firstNumberSize++] = currentButton;
					else
						display.printError(1);
					firstNumberSize = checkNumber(firstNumber, firstNumberSize);
					display.printFirstNumber(firstNumber);
				}
			}

			else if (currentButton == 10) {
				currentButton = '*';
				if (secondNumberSize > 0) {
					if (secondNumber[0] == ' ') {
						display.printError(4);
						display.printSecondNumber(secondNumber);
						continue;
					}

					if ((secondNumber[0] == '0') && (operation == 4)) {
						display.printError(2);
						display.printSecondNumber(secondNumber);
						continue;
					}

					long long result = calculate(firstNumber, operation, secondNumber);

					if (result > 999999999999999) {
						secondNumberSize = clearNumber(secondNumber);
						display.printSecondNumber(secondNumber);

						operation = 0;
						display.printOperation(operations[operation]);

						display.printError(3);

						firstNumberSize = setNumber(firstNumber, 0);
						display.printFirstNumber(firstNumber);
						continue;
					}

					firstNumberSize = setNumber(firstNumber, result);
					display.printFirstNumber(firstNumber);

					secondNumberSize = clearNumber(secondNumber);
					display.printSecondNumber(secondNumber);

					operation = 1;
				}
				else {
					operation += 1;
					if (operation > 4)
						operation = 1;
				}
				display.printOperation(operations[operation]);
			}

			else if (currentButton == 12) {
				currentButton = '#';
				if (operation == 0) {
					firstNumberSize = setNumber(firstNumber, 0);
					display.printFirstNumber(firstNumber);
				}
				else {
					if (secondNumber[0] == ' ') {
						display.printError(4);
						display.printSecondNumber(secondNumber);
						continue;
					}

					if ((secondNumber[0] == '0') && (operation == 4)) {
						display.printError(2);
						display.printSecondNumber(secondNumber);
						continue;
					}
				}

				long long result = calculate(firstNumber, operation, secondNumber);

				if (result > 999999999999999) {
					secondNumberSize = clearNumber(secondNumber);
					display.printSecondNumber(secondNumber);

					operation = 0;
					display.printOperation(operations[operation]);

					display.printError(3);

					firstNumberSize = setNumber(firstNumber, 0);
					display.printFirstNumber(firstNumber);
					continue;
				}

				firstNumberSize = setNumber(firstNumber, result);
				display.printFirstNumber(firstNumber);

				secondNumberSize = clearNumber(secondNumber);
				display.printSecondNumber(secondNumber);

				operation = 0;
				display.printOperation(operations[operation]);
			}

			else {
				currentButton += 48;
				if (operation > 0) {
					if (secondNumberSize < 10)
						secondNumber[secondNumberSize++] = currentButton;
					else
						display.printError(0);
					secondNumberSize = checkNumber(secondNumber, secondNumberSize);
					display.printSecondNumber(secondNumber);
				}
				else {
					if (firstNumberSize < 10)
						firstNumber[firstNumberSize++] = currentButton;
					else
						display.printError(1);
					firstNumberSize = checkNumber(firstNumber, firstNumberSize);
					display.printFirstNumber(firstNumber);
				}
	        }

			sprintf(buffer, "%c\n", currentButton);
			send(buffer, sizeof(buffer));
		}
		else {
			buttonPressed = false;
		}

		delay(1000);
	}
}

#pragma GCC diagnostic pop
