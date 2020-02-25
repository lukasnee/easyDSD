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

/* todo: this is new/delete operators overloading for working with FreeRTOS.
 * Couldn't get it work, deprecated idea... */

/*
void* operator new(size_t size) {
	return pvPortMalloc(size);
}

void* operator new[](size_t size) {
	return pvPortMalloc(size);
}

void operator delete(void *ptr) {
	vPortFree(ptr);
}

void operator delete[](void *ptr) {
	vPortFree(ptr);
}
*/


#ifdef __cplusplus
extern "C" {
#endif

#include <hwg.hpp>			/* hardware glue code for user to fill */

#include <stdlib.h>
#include <string.h>
//#include <string>
#include <stdio.h>

#include <Adafruit_GFX.h>
#include <TFT_ILI9163C.h> 	/* display driver */
#include "JC_Button.h" 		/* library for GPIO hardware buttons (debounce, etc.)*/
#include "fatfs.h"			/* FAT32 file system */
//#include <tag.h>			/* library for ID3v2 decoding from .dsf file */

#include "wucyFont8pt7b.h"

#define TH_STM32
#define WUCY_OS

void th_dsd_start(void); /* start task externally from c source */

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

typedef enum button_map_{

	BTN_UP,
	BTN_OK,
	BTN_DOWN,

BTN_COUNT} button_map;

class openDSD
{

	friend void th_dsd_start(void);

    private:
        openDSD() {}

        static void th_dsd_task(void const * argument);

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

        Button btn[BTN_COUNT] = {
        		{BTN_UP_GPIO_Port, BTN_UP_Pin},
				{BTN_OK_GPIO_Port, BTN_OK_Pin},
				{BTN_DOWN_GPIO_Port, BTN_DOWN_Pin}
        };

};



#endif
#endif  // DSD_HPP_
