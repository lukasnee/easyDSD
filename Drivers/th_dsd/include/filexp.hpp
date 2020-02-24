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
#ifndef FILEXP_HPP_
#define FILEXP_HPP_

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
//#include "cmsis_os.h"		/* cmsis freeRTOS API */
#include "fatfs.h"			/* FAT32 file system */
#include <hwg.hpp> 	/* hardware glue code for user to fill */

#endif  // FILEXP_HPP_
