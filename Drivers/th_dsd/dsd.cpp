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

#include <dsd.hpp>

#ifdef __cplusplus
extern "C" {
#endif

// all of your legacy C code here

#ifdef __cplusplus
}
#endif

osThreadId defaultTaskHandle;

#include <Adafruit_GFX.h>
#include <TFT_ILI9163C.h>

TFT_ILI9163C tft = TFT_ILI9163C(TFT_PIN_CS, TFT_PIN_A0, TFT_PIN_RESET);

unsigned long testText() {
  tft.fillScreen();
  unsigned long start = millis();
  tft.setCursor(0, 0);
  tft.setTextColor(COLOR_LIME);
  tft.setTextSize(1);
  tft.println("Hello World!");
  tft.setTextColor(COLOR_YELLOW);
  tft.setTextSize(2);
  tft.println(1234.56);
  tft.setTextColor(COLOR_RED);
  tft.setTextSize(3);
  tft.println("MAZAFAKA");
  tft.println();
  tft.setTextColor(COLOR_GREEN);
  tft.setTextSize(4);
  tft.println("Hello");
  return millis() - start;
}

unsigned long testLines(color_rgb16_t color) {

	tft.setDrawColor(color);
  tft.fillScreen();
  unsigned long start, t;
  int           x1, y1, x2, y2,
  w = tft.width(),
  h = tft.height();
  tft.fillScreen();
  x1 = y1 = 0;
  y2    = h - 1;
  start = millis();
  for(x2=0; x2<w; x2+=6) tft.drawLine(x1, y1, x2, y2);
  x2    = w - 1;
  for(y2=0; y2<h; y2+=6) tft.drawLine(x1, y1, x2, y2);
  t     = millis() - start; // fillScreen doesn't count against timing
  tft.fillScreen();
  x1    = w - 1;
  y1    = 0;
  y2    = h - 1;
  start = millis();
  for(x2=0; x2<w; x2+=6) tft.drawLine(x1, y1, x2, y2);
  x2    = 0;
  for(y2=0; y2<h; y2+=6) tft.drawLine(x1, y1, x2, y2);
  t    += millis() - start;
  tft.fillScreen();
  x1    = 0;
  y1    = h - 1;
  y2    = 0;
  start = millis();
  for(x2=0; x2<w; x2+=6) tft.drawLine(x1, y1, x2, y2);
  x2    = w - 1;
  for(y2=0; y2<h; y2+=6) tft.drawLine(x1, y1, x2, y2);
  t    += millis() - start;
  tft.fillScreen();
  x1    = w - 1;
  y1    = h - 1;
  y2    = 0;
  start = millis();
  for(x2=0; x2<w; x2+=6) tft.drawLine(x1, y1, x2, y2);
  x2    = 0;
  for(y2=0; y2<h; y2+=6) tft.drawLine(x1, y1, x2, y2);
  return millis() - start;
}

void openDSD::th_dsd_task(void const * argument)
{

	int * foo = NULL;
	foo = new int;
	*foo= 8;
	*foo= -98;
	delete foo;

	static openDSD dsd;

	  tft.begin();
	dsd.buttonsBegin();

	while (true) {

		dsd.buttonsUpdate();

		HAL_GPIO_WritePin(LED_D2_GPIO_Port, LED_D2_Pin,
				(GPIO_PinState) (dsd.btn[BTN_OK].pressedFor(500) | dsd.btn[BTN_OK].wasPressed()));
		HAL_GPIO_WritePin(LED_D2_GPIO_Port, LED_D2_Pin,
				(GPIO_PinState) (dsd.btn[BTN_OK].pressedFor(1000) | dsd.btn[BTN_OK].wasReleased()));

	  testLines((color_rgb16_t)(std::rand() % 0xffff));
	  osDelay(100);
	  testText();
	  osDelay(500);
	}
}

void th_dsd_start(void) {

	osThreadDef(th_dsd, openDSD::th_dsd_task, osPriorityAboveNormal, 0, 256);
	defaultTaskHandle = osThreadCreate(osThread(th_dsd), NULL);

}


/* end of th_dsd.cpp */
