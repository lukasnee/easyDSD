/*
	ILI9163C - A fast SPI driver for TFT that use Ilitek ILI9163C.
	
	Modified by Therram for project openDSD.
	
	-------------------------------------------------------------------------------
    Copyright (c) 2014, .S.U.M.O.T.O.Y., coded by Max MC Costa.    

    TFT_ILI9163C Library is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    TFT_ILI9163C Library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.

	-------------------------------------------------------------------------------
*/

#ifndef _TFT_ILI9163CLIB_H_
#define _TFT_ILI9163CLIB_H_

#include "dsd.hpp"

#include <Adafruit_GFX.h>

#include "../TFT_ILI9163C/_settings/TFT_ILI9163C_settings.h"
#include "../TFT_ILI9163C/_settings/TFT_ILI9163C_registers.h"

#define	BLACK   		0x0000
#define WHITE   		0xFFFF

class TFT_ILI9163C : public Adafruit_GFX {

 public:
				TFT_ILI9163C(uint8_t cspin,uint8_t dcpin,uint8_t rstpin = 0xFF);
	
	void     	begin(void),
				setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1),//graphic Addressing
				setCursor(int16_t x,int16_t y),//char addressing
				pushColor(uint16_t color),
				fillScreen(uint16_t color = 0x0000),
				clearScreen(uint16_t color = 0x0000),//same as fillScreen
				drawPixel(int16_t x, int16_t y, uint16_t color),
				drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color),
				drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color),
				fillRect(int16_t x, int16_t y, int16_t w, int16_t h,uint16_t color),
				setRotation(uint8_t r),
				invertDisplay(boolean i);
	uint8_t 	errorCode(void);			
	void		idleMode(boolean onOff);
	void		display(boolean onOff);	
	void		sleepMode(boolean mode);
	void 		defineScrollArea(uint16_t tfa, uint16_t bfa);
	void		scroll(uint16_t adrs);
	void 		startPushData(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
	void 		pushData(uint16_t color);
	void		writeScreen24(const uint32_t *bitmap,uint16_t size=_TFTWIDTH*_TFTHEIGHT);

	inline uint16_t Color565(uint8_t r, uint8_t g, uint8_t b) {return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);};
  //convert 24bit color into packet 16 bit one (credits for this are all mine)
	inline uint16_t Color24To565(int32_t color_) { return ((((color_ >> 16) & 0xFF) / 8) << 11) | ((((color_ >> 8) & 0xFF) / 4) << 5) | (((color_) &  0xFF) / 8);}

 protected:
	void		writecommand(uint8_t c);
	void		writedata(uint8_t d);
	void		writedata16(uint16_t d);

	volatile uint8_t	_Mactrl_Data;//container for the memory access control data
	uint8_t				_colorspaceData;
	uint8_t 			_cs,_rs,_rst;

 private:
	void 		colorSpace(uint8_t cspace);
	void 		setAddr(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
	uint8_t		sleep;
	void 		chipInit();
	bool 		boundaryCheck(int16_t x,int16_t y);
	void 		homeAddress();
	uint8_t		_initError;
};
#endif
