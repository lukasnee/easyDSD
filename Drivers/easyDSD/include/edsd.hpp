/*
	easyDSD main module.

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

#ifndef EDSD_HPP_
#define EDSD_HPP_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef __cplusplus
}
#endif

#define WUCY_OS

#include "easyDSD.h"		/* user api, configurations */

#include "hwg.hpp"			/* Hardware Glue Code for user to fill */

#include "storage.hpp"		/* SD card storage mass storage for music data, etc. */
#include "player.hpp"		/* Audio file player */
#include "keyboard.hpp"		/* button inputs for player control, etc. */

#include <Adafruit_GFX.h>	/* graphics library for display */
#include <TFT_ILI9163C.h> 	/* display driver */

/* misc */

#include "log.hpp" 			/* text logger for display */
//#include <tag.h>			/* library for ID3v2 decoding from .dsf file */
//#include "sound.h"

class easyDSD : public Player, private virtual SD {

public:

	//start easyDSD player with:
	static void easy_dsd_start_task(void const * argument);

private:

	Keyboard 		kb;
	TFT_ILI9163C 	tft;

	easyDSD() : kb(), tft(TFT_PIN_CS, TFT_PIN_A0, TFT_PIN_RESET) { };

	void task_easy_dsd(void);

};

#endif  // EDSD_HPP_
