/*
	Direct Stream Digital (DSD) player core module.

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

#ifndef DSD_HPP_
#define DSD_HPP_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <string.h>
//#include <string>
#include <stdio.h>

#include "fatfs.h"			/* FAT32 file system */
//#include <tag.h>			/* library for ID3v2 decoding from .dsf file */

#include "sound.h"

#ifdef __cplusplus
}
#endif


#define WUCY_OS

#include "openDSD.h"
#include <hwg.hpp>			/* hardware glue code for user to fill */
#include <Adafruit_GFX.h>
#include <TFT_ILI9163C.h> 	/* display driver */
#include "JC_Button.h" 		/* library for GPIO hardware buttons (debounce, etc.)*/
#include <log.hpp>
#include "storage.hpp"

extern const GFXfont wucyFont8pt7b;

typedef enum button_map_{

	BTN_UP,
	BTN_OK,
	BTN_DOWN,

BTN_COUNT} button_map;

class openDSD :
		public TFT_ILI9163C,
		public Storage
{
	public:
	openDSD() : TFT_ILI9163C(TFT_PIN_CS, TFT_PIN_A0, TFT_PIN_RESET) {
		TFT_ILI9163C::begin();
		Storage::sd_mount();
	}

	FRESULT scanFiles(char* path);
	//void list(const char * pattern);

	static void th_dsd_task(void const * argument);
//todo: button abstractions should go to button clas maybe ?
	void buttonsUpdate(void)
	{
		for(uint8_t i = 0; i < BTN_COUNT; i++)
			btn[i].read();
	}

	void buttonsBegin(void)
	{
		for(uint8_t i = 0; i < BTN_COUNT; i++)
			btn[i].begin();
	}

	button_map buttonWasPressed(void)
	{
		buttonsUpdate();
		for(uint8_t i = 0; i < BTN_COUNT; i++) {
			if (btn[i].wasPressed())
				return static_cast<button_map>(i); // wasPressed
		}
		return BTN_COUNT; // none presssed
	}

	Button btn[BTN_COUNT] = {
			{BTN_UP_GPIO_Port, BTN_UP_Pin},
			{BTN_OK_GPIO_Port, BTN_OK_Pin},
			{BTN_DOWN_GPIO_Port, BTN_DOWN_Pin}
	};

	friend void th_dsd_start(void);

};


extern TFT_ILI9163C tft;
extern openDSD dsd;


#endif  // DSD_HPP_
