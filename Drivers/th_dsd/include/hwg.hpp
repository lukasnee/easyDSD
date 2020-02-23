/*
	Direct Stream Digital (DSD) player - hardware glue.
	Glue code for your specific hardware (initially designed for STM32F407VE).

	License: GPL 3.0
	Copyright (C) 2020 Lukas Neverauskis
	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#ifndef DSD_HWG_H_
#define DSD_HWG_H_

#ifdef __cplusplus
extern "C" {
#endif

// all of your legacy C code here

#ifdef __cplusplus
}
#endif

/* prototypes of API functions here */

#include "main.h"

#define TH_STM32

#include <string>
#include <stdio.h>
typedef std::string String;
typedef char* __FlashStringHelper;
typedef bool boolean;
#define HIGH true;
#define LOW false;

typedef struct tft_pinout_stm32_{

	GPIO_TypeDef* GPIOx;
	uint16_t GPIO_Pin;

}tft_pinout_stm32;

typedef enum tft_pinout_id_{

	TFT_PIN_CS,
	TFT_PIN_RESET,
	TFT_PIN_SDA,
	TFT_PIN_SCK,
	TFT_PIN_A0,

}tft_pinout_id;

class Print {
public:

	virtual size_t write(unsigned char c) = 0;

	void print(const String str);
	void println(const String str = "\0");

};

#define pinMode(pin, in_or_out) __NOP()// ignore, because pin modes already set in stm32cubemx. todo: unless needed to change in runtime.

void digitalWrite(uint8_t pin, bool state);
void delay(uint32_t ms);
void bitClear(volatile uint8_t &byte, uint8_t bit);
void bitSet(volatile uint8_t &byte, uint8_t bit);
#endif  // DSD_HWG_H_
