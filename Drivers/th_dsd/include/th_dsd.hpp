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
#ifndef TH_DSD_HPP_
#define TH_DSD_HPP_

 #ifdef __cplusplus
 #define EXTERNC extern "C"
 #else
 #define EXTERNC
 #endif

#ifdef __cplusplus
extern "C" {
#endif

// all of your legacy C code here

#ifdef __cplusplus
}
#endif

#include "main.h"
#include "JC_Button.h" 		/* library for GPIO hardware buttons (debounce, etc.)*/
#include "cmsis_os.h"		/* cmsis freeRTOS API */
#include "fatfs.h"			/* FAT32 file system */
#include "th_dsd_hwg.hpp" 	/* hardware glue code for user to fill */

//#include <tag.h>			/* library for ID3v2 decoding from .dsf file */

EXTERNC void th_dsd_start(void); /* start task externally from c source */
typedef enum button_map_{

		BTN_UP,
		BTN_OK,
		BTN_DOWN,

	BTN_COUNT} button_map;

class openDSD
{
    public:
	/* singleton */
        static openDSD& getInstance()
        {
            static openDSD instance;
            return instance;
        }
        openDSD(openDSD const&) = delete;
        void operator = (openDSD const&) = delete;

        static void th_dsd_task(void const * argument);

    private:

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

        openDSD() {}                    // Constructor? (the {} brackets) are needed here.



        Button btn[BTN_COUNT] = {
        		{BTN_UP_GPIO_Port, BTN_UP_Pin},
				{BTN_OK_GPIO_Port, BTN_OK_Pin},
				{BTN_DOWN_GPIO_Port, BTN_DOWN_Pin}
        };
};




#endif  // TH_DSD_HPP_
