/*
	SD card FAT32 file explorer for openDSD.
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

#include "filexp.hpp"

#include "string.h"

#ifdef __cplusplus
extern "C" {
#endif

// all of your legacy C code here

#ifdef __cplusplus
}
#endif


void foo(void) {

	FRESULT res;                                          /* FatFs function common result code */
	UINT byteswritten, bytesread;                     /* File write/read counts */
	uint8_t wtext[] = "Hello from bambukas :)"; /* File write buffer */
	uint8_t rtext[100];                                   /* File read buffer */

	if(f_mount(&SDFatFS, (TCHAR const*)SDPath, 0) == FR_OK)
	{
		HAL_Delay(200);

		//Open file for writing (Create)
		if(f_open(&SDFile, "F7FILE5.TXT", FA_CREATE_ALWAYS | FA_WRITE) == FR_OK) {

			//Write to the text file
			res = f_write(&SDFile, wtext, strlen((char *)wtext), &byteswritten);

			f_close(&SDFile);

			//Test read file
			f_open(&SDFile, "F7FILE5.TXT",  FA_READ);
			memset(rtext,0,sizeof(rtext));
			res = f_read(&SDFile, rtext, sizeof(rtext), &bytesread);

			f_close(&SDFile);
		}
	}


	f_mount(&SDFatFS, (TCHAR const*)NULL, 0);

}

/* Glue code template for user to fill */
