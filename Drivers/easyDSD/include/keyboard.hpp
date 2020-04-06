/*
	keyboard module.

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

#ifndef EASYDSD_INCLUDE_KEYBOARD_HPP_
#define EASYDSD_INCLUDE_KEYBOARD_HPP_

#include "JC_Button.h" 		/* library for GPIO hardware buttons (debounce, etc.)*/

typedef enum button_map_{

		BTN_UP,
		BTN_OK,
		BTN_DOWN,

	BTN_COUNT} button_map;

	class Keyboard{

	public:

		void updateAll(void)
		{
			for(uint8_t i = 0; i < BTN_COUNT; i++)
				btn[i].read();
		}

		void beginAll(void)
		{
			for(uint8_t i = 0; i < BTN_COUNT; i++)
				btn[i].begin();
		}

		button_map wasPressed(void)
		{
			updateAll();
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
	};

#endif /* EASYDSD_INCLUDE_KEYBOARD_HPP_ */
