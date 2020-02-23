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

// Color definitions
#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

/*
Teensy3.x and Arduino's
You are using 4 wire SPI here, so:
 MOSI:  11//Teensy3.x/Arduino UNO (for MEGA/DUE refere to arduino site)
 MISO:  12//Teensy3.x/Arduino UNO (for MEGA/DUE refere to arduino site)
 SCK:   13//Teensy3.x/Arduino UNO (for MEGA/DUE refere to arduino site)
 the rest of pin below:
 */
#define __CS 10
#define __DC 9
/*
Teensy 3.x can use: 2,6,9,10,15,20,21,22,23
Arduino's 8 bit: any
DUE: check arduino site
If you do not use reset, tie it to +3V3
*/

TFT_ILI9163C tft = TFT_ILI9163C(TFT_PIN_CS, TFT_PIN_A0, TFT_PIN_RESET);

unsigned long testText() {
  tft.fillScreen();
  unsigned long start = millis();
  tft.setCursor(0, 0);
  tft.setTextColor(WHITE);
  tft.setTextSize(1);
  tft.println("Hello World!");
  tft.setTextColor(YELLOW);
  tft.setTextSize(2);
  tft.println(1234.56);
  tft.setTextColor(RED);
  tft.setTextSize(3);
  tft.println("MAZAFAKA");
  tft.println();
  tft.setTextColor(GREEN);
  tft.setTextSize(4);
  tft.println("Hello");
  return millis() - start;
}

unsigned long testLines(uint16_t color) {
  tft.fillScreen();
  unsigned long start, t;
  int           x1, y1, x2, y2,
  w = tft.width(),
  h = tft.height();
  tft.fillScreen();
  x1 = y1 = 0;
  y2    = h - 1;
  start = millis();
  for(x2=0; x2<w; x2+=6) tft.drawLine(x1, y1, x2, y2, color);
  x2    = w - 1;
  for(y2=0; y2<h; y2+=6) tft.drawLine(x1, y1, x2, y2, color);
  t     = millis() - start; // fillScreen doesn't count against timing
  tft.fillScreen();
  x1    = w - 1;
  y1    = 0;
  y2    = h - 1;
  start = millis();
  for(x2=0; x2<w; x2+=6) tft.drawLine(x1, y1, x2, y2, color);
  x2    = 0;
  for(y2=0; y2<h; y2+=6) tft.drawLine(x1, y1, x2, y2, color);
  t    += millis() - start;
  tft.fillScreen();
  x1    = 0;
  y1    = h - 1;
  y2    = 0;
  start = millis();
  for(x2=0; x2<w; x2+=6) tft.drawLine(x1, y1, x2, y2, color);
  x2    = w - 1;
  for(y2=0; y2<h; y2+=6) tft.drawLine(x1, y1, x2, y2, color);
  t    += millis() - start;
  tft.fillScreen();
  x1    = w - 1;
  y1    = h - 1;
  y2    = 0;
  start = millis();
  for(x2=0; x2<w; x2+=6) tft.drawLine(x1, y1, x2, y2, color);
  x2    = 0;
  for(y2=0; y2<h; y2+=6) tft.drawLine(x1, y1, x2, y2, color);
  return millis() - start;
}

void openDSD::th_dsd_task(void const * argument)
{

	static openDSD dsd;

	  tft.begin();
	dsd.buttonsBegin();

	while (true) {

		dsd.buttonsUpdate();

		HAL_GPIO_WritePin(LED_D2_GPIO_Port, LED_D2_Pin,
				(GPIO_PinState) (dsd.btn[BTN_OK].pressedFor(500) | dsd.btn[BTN_OK].wasPressed()));
		HAL_GPIO_WritePin(LED_D2_GPIO_Port, LED_D2_Pin,
				(GPIO_PinState) (dsd.btn[BTN_OK].pressedFor(1000) | dsd.btn[BTN_OK].wasReleased()));

	  testLines(std::rand() % 0xffff + 0x00ff);
	  osDelay(100);
	  testText();
	  osDelay(500);
	}
}

void th_dsd_start(void) {

	osThreadDef(th_dsd, openDSD::th_dsd_task, osPriorityAboveNormal, 0, 1024);
	defaultTaskHandle = osThreadCreate(osThread(th_dsd), NULL);

}


/* end of th_dsd.cpp */
