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

#include <th_dsd.hpp>

#ifdef __cplusplus
extern "C" {
#endif

// all of your legacy C code here

#ifdef __cplusplus
}
#endif

osThreadId defaultTaskHandle;

void openDSD::th_dsd_task(void const * argument)
{
	openDSD dsd = openDSD::getInstance();

	dsd.buttonsBegin();

	while (1) {

		dsd.buttonUpdate();

		HAL_GPIO_WritePin(LED_D2_GPIO_Port, LED_D2_Pin,
				(GPIO_PinState) (dsd.btn[BTN_OK].pressedFor(500) | dsd.btn[BTN_OK].wasPressed()));
		HAL_GPIO_WritePin(LED_D3_GPIO_Port, LED_D3_Pin,
				(GPIO_PinState) (dsd.btn[BTN_OK].pressedFor(1000) | dsd.btn[BTN_OK].wasReleased()));

		osDelay(10);
	}
}

void th_dsd_start(void) {

	osThreadDef(th_dsd, th_dsd_task, osPriorityAboveNormal, 0, 1024);
	defaultTaskHandle = osThreadCreate(osThread(th_dsd), NULL);

}

/* end of th_dsd.cpp */
