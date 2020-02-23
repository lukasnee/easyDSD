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

#include <hwg.hpp>

#ifdef __cplusplus
extern "C" {
#endif

// all of your legacy C code here

#ifdef __cplusplus
}
#endif


/* Glue code template for user to fill */

const tft_pinout_stm32 tft_stm32_pinmap[] = {
	{TFT_CS_GPIO_Port, TFT_CS_Pin},
	{TFT_RESET_GPIO_Port, TFT_RESET_Pin},
	{TFT_SDA_GPIO_Port, TFT_SDA_Pin},
	{TFT_SCK_GPIO_Port, TFT_SCK_Pin},
	{TFT_A0_GPIO_Port, TFT_A0_Pin}
};
void digitalWrite(uint8_t pin, bool state) {

	HAL_GPIO_WritePin(tft_stm32_pinmap[pin].GPIOx, tft_stm32_pinmap[pin].GPIO_Pin, (GPIO_PinState)state);

}

inline void delay(uint32_t ms) {

	HAL_Delay(ms);

}

void bitClear(uint8_t &byte, uint8_t bit) {

	byte &= ~(1 << bit);

}
void bitSet(uint8_t &byte, uint8_t bit){

	byte |= (1 << bit);

}
