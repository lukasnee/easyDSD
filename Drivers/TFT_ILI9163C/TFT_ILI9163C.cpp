#include "../TFT_ILI9163C/TFT_ILI9163C.h"

#include <limits.h>

TFT_ILI9163C::TFT_ILI9163C(uint8_t cspin,uint8_t dcpin,uint8_t rstpin) : GFXcanvas16(_TFTWIDTH,_TFTHEIGHT)
{
	_cs   = cspin;
	_rs   = dcpin;
	_rst  = rstpin;
}

void TFT_ILI9163C::updateScreen(void) {

	togglePingPong();
	homeAddress();
	spi.transfer((uint8_t*)getBuffer(), DC_DATA, _GRAMWIDTH * _GRAMHEIGH * sizeof(uint16_t));

}

void TFT_ILI9163C::writecommand(uint8_t c)
{
	spi.transfer(&c, DC_COMMAND);
}

void TFT_ILI9163C::writedata(uint8_t c)
{
	spi.transfer(&c, DC_DATA);
}

void TFT_ILI9163C::writedata16(uint16_t d)
{
	uint8_t a[2];
	a[0] = d >> 8;
	a[1] = d;
	spi.transfer(a, DC_DATA, 2);
}

void TFT_ILI9163C::begin(void) 
{
	sleep = 0;
	_initError = 0b00000000;
	pinMode(_rs, OUTPUT);
	pinMode(_cs, OUTPUT);
	spi.begin();
	digitalWrite(_cs, LOW);

	if (_rst != 255) {
		pinMode(_rst, OUTPUT);
		digitalWrite(_rst, HIGH);
		delay(500);
		digitalWrite(_rst, LOW);
		delay(500);
		digitalWrite(_rst, HIGH);
		delay(500);
	}

/*
7) MY:  1(bottom to top), 0(top to bottom) 	Row Address Order
6) MX:  1(R to L),        0(L to R)        	Column Address Order
5) MV:  1(Exchanged),     0(normal)        	Row/Column exchange
4) ML:  1(bottom to top), 0(top to bottom) 	Vertical Refresh Order
3) RGB: 1(BGR), 		   0(RGB)           	Color Space
2) MH:  1(R to L),        0(L to R)        	Horizontal Refresh Order
1)
0)

     MY, MX, MV, ML,RGB, MH, D1, D0
	 0 | 0 | 0 | 0 | 1 | 0 | 0 | 0	//normal
	 1 | 0 | 0 | 0 | 1 | 0 | 0 | 0	//Y-Mirror
	 0 | 1 | 0 | 0 | 1 | 0 | 0 | 0	//X-Mirror
	 1 | 1 | 0 | 0 | 1 | 0 | 0 | 0	//X-Y-Mirror
	 0 | 0 | 1 | 0 | 1 | 0 | 0 | 0	//X-Y Exchange
	 1 | 0 | 1 | 0 | 1 | 0 | 0 | 0	//X-Y Exchange, Y-Mirror
	 0 | 1 | 1 | 0 | 1 | 0 | 0 | 0	//XY exchange
	 1 | 1 | 1 | 0 | 1 | 0 | 0 | 0
*/
	_Mactrl_Data = 0b00000000;
	_colorspaceData = __COLORSPC;//start with default data;
	chipInit();
}

uint8_t TFT_ILI9163C::errorCode(void) 
{
	return _initError;
}

void TFT_ILI9163C::chipInit() {

	uint8_t i;
	
	writecommand(CMD_SWRESET);//software reset
	delay(500);

	writecommand(CMD_SLPOUT);//exit sleep
	delay(5);

	writecommand(CMD_PIXFMT);//Set Color Format 16bit
	writedata(0x05);
	delay(5);

	writecommand(CMD_GAMMASET);//default gamma curve 3
	writedata(0x04);//0x04
	delay(1);

	writecommand(CMD_GAMRSEL);//Enable Gamma adj
	writedata(0x01);
	delay(1);

	writecommand(CMD_NORML);

	writecommand(CMD_DFUNCTR);
	writedata(0b11111111);//
	writedata(0b00000110);//

	writecommand(CMD_PGAMMAC);//Positive Gamma Correction Setting
	for (i=0;i<15;i++){
		writedata(pGammaSet[i]);
	}
	
	writecommand(CMD_NGAMMAC);//Negative Gamma Correction Setting
	for (i=0;i<15;i++){
		writedata(nGammaSet[i]);
	}

	writecommand(CMD_FRMCTR1);//Frame Rate Control (In normal mode/Full colors)
	writedata(0x08);//0x0C//0x08
	writedata(0x02);//0x14//0x08
	delay(1);
	
	writecommand(CMD_DINVCTR);//display inversion
	writedata(0x07);
	delay(1);
	
	writecommand(CMD_PWCTR1);//Set VRH1[4:0] & VC[2:0] for VCI1 & GVDD
	writedata(0x0A);//4.30 - 0x0A
	writedata(0x02);//0x05
	delay(1);
	
	writecommand(CMD_PWCTR2);//Set BT[2:0] for AVDD & VCL & VGH & VGL
	writedata(0x02);
	delay(1);
	
	writecommand(CMD_VCOMCTR1);//Set VMH[6:0] & VML[6:0] for VOMH & VCOML
	writedata(0x50);//0x50
	writedata(99);//0x5b
	delay(1);
	
	writecommand(CMD_VCOMOFFS);
	writedata(0);//0x40
	delay(1);

	writecommand(CMD_CLMADRS);//Set Column Address
	writedata16(0x00);
	writedata16(_GRAMWIDTH);

	writecommand(CMD_PGEADRS);//Set Page Address
	writedata16(0X00);
	writedata16(_GRAMHEIGH);
	// set scroll area (thanks Masuda)
	writecommand(CMD_VSCLLDEF);
	writedata16(__OFFSET);
	writedata16(_GRAMHEIGH - __OFFSET);
	writedata16(0);

	colorSpace(_colorspaceData);

	setRotation(0);
	writecommand(CMD_DISPON);//display ON
	delay(1);
	writecommand(CMD_RAMWR);//Memory Write

	delay(1);
	fillScreen(BLACK);
}

/*
Colorspace selection:
0: RGB
1: GBR
*/
void TFT_ILI9163C::colorSpace(uint8_t cspace) {
	if (cspace < 1){
		bitClear(_Mactrl_Data,3);
	} else {
		bitSet(_Mactrl_Data,3);
	}
}

void TFT_ILI9163C::invertDisplay(boolean i) {
	writecommand(i ? CMD_DINVON : CMD_DINVOF);
}

void TFT_ILI9163C::display(boolean onOff) {
	if (onOff){
	writecommand(CMD_DISPON);
	} else {
		writecommand(CMD_DISPOFF);
	}
}

void TFT_ILI9163C::idleMode(boolean onOff) {
	if (onOff){
		writecommand(CMD_IDLEON);

	} else {
		writecommand(CMD_IDLEOF);
	}
}

void TFT_ILI9163C::sleepMode(boolean mode) {
	if (mode){
		if (sleep == 1) return;//already sleeping
		sleep = 1;
		writecommand(CMD_SLPIN);

		delay(5);//needed
	} else {
		if (sleep == 0) return; //Already awake
		sleep = 0;
		writecommand(CMD_SLPOUT);
		delay(120);//needed
	}
}

void TFT_ILI9163C::defineScrollArea(uint16_t tfa, uint16_t bfa){
    tfa += __OFFSET;
    int16_t vsa = _GRAMHEIGH - tfa - bfa;
    if (vsa >= 0) {
		writecommand(CMD_VSCLLDEF);
		writedata16(tfa);
		writedata16(vsa);
		writedata16(bfa);
    }
}

void TFT_ILI9163C::scroll(uint16_t adrs) {
	if (adrs <= _GRAMHEIGH) {
		writecommand(CMD_VSSTADRS);
		writedata16(adrs + __OFFSET);
	}
}
/*
//corrected! v3
void TFT_ILI9163C::clearScreen(uint16_t color) {
	int px;
	setAddr(0x00,0x00,_GRAMWIDTH,_GRAMHEIGH);//go home
	for (px = 0;px < _GRAMSIZE; px++)
		writedata16(color);
}
*/
/*
void TFT_ILI9163C::startPushData(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
	setAddr(x0,y0,x1,y1);
}
*/
/*
void TFT_ILI9163C::pushData(uint16_t color) {
	writedata16(color);
}
*/
/*
void TFT_ILI9163C::pushColor(uint16_t color) {
	writedata16(color);
}
*/
/*
void TFT_ILI9163C::writeScreen24(const uint32_t *bitmap,uint16_t size) {
	uint16_t color, px;
	writecommand(CMD_RAMWR);
	for (px = 0;px < size; px++){
		color = Color24To565(bitmap[px]);
		writedata16(color);
	}
	homeAddress();
}
*/
void TFT_ILI9163C::homeAddress() {
	setAddrWindow(0x00,0x00,_GRAMWIDTH,_GRAMHEIGH);
}
/*
void TFT_ILI9163C::setCursor(int16_t x, int16_t y) {
	if (boundaryCheck(x,y)) return;
	setAddrWindow(0x00,0x00,x,y);
	cursor_x = x;
	cursor_y = y;
}
*/
/*
void TFT_ILI9163C::drawPixel(int16_t x, int16_t y, uint16_t color) {
	if (boundaryCheck(x,y)) return;
	if ((x < 0) || (y < 0)) return;
	setAddr(x,y,x+1,y+1);
	writedata16(color);
}
*/
/*
void TFT_ILI9163C::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
	// Rudimentary clipping
	if (boundaryCheck(x,y)) return;
	if (((y + h) - 1) >= _height) h = _height-y;
	setAddr(x,y,x,(y+h)-1);
	while (h-- > 0) {
		writedata16(color);
	}
}
*/

bool TFT_ILI9163C::boundaryCheck(int16_t x,int16_t y){
	if ((x >= _width) || (y >= _height)) return true;
	return false;
}
/*
void TFT_ILI9163C::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
	// Rudimentary clipping
	if (boundaryCheck(x,y)) return;
	if (((x+w) - 1) >= _width)  w = _width-x;
	setAddr(x,y,(x+w)-1,y);
	while (w-- > 0) {
		writedata16(color);
	}
}
*/
/*
void TFT_ILI9163C::fillScreen(uint16_t color) {
	clearScreen(color);
}
*/
// fill a rectangle
/*
void TFT_ILI9163C::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
	if (boundaryCheck(x,y)) return;
	if (((x + w) - 1) >= _width)  w = _width  - x;
	if (((y + h) - 1) >= _height) h = _height - y;
	setAddr(x,y,(x+w)-1,(y+h)-1);
	for (y = h;y > 0;y--) {
		for (x = w;x > 0;x--)
			writedata16(color);
	}
}
*/
void TFT_ILI9163C::setAddr(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1){

		setAddrWindow(x0,y0,x1,y1);
}

void TFT_ILI9163C::setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {

		writecommand(CMD_CLMADRS); // Column
		if (rotation == 0 || rotation > 1){
			writedata16(x0);
			writedata16(x1);
		} else {
			writedata16(x0 + __OFFSET);
			writedata16(x1 + __OFFSET);
		}

		writecommand(CMD_PGEADRS); // Page
		if (rotation == 0){
			writedata16(y0 + __OFFSET);
			writedata16(y1 + __OFFSET);
		} else {
			writedata16(y0);
			writedata16(y1);
		}
		writecommand(CMD_RAMWR); //Into RAM
}

void TFT_ILI9163C::setRotation(uint8_t m) {
	rotation = m % 4; // can't be higher than 3
	switch (rotation) {
	case 0:
		_Mactrl_Data = 0b00001000;
		_width  = _TFTWIDTH;
		_height = _TFTHEIGHT;//-__OFFSET;
		break;
	case 1:
		_Mactrl_Data = 0b01101000;
		_width  = _TFTHEIGHT;//-__OFFSET;
		_height = _TFTWIDTH;
		break;
	case 2:
		_Mactrl_Data = 0b11001000;
		_width  = _TFTWIDTH;
		_height = _TFTHEIGHT;//-__OFFSET;
		break;
	case 3:
		_Mactrl_Data = 0b10101000;
		_width  = _TFTWIDTH;
		_height = _TFTHEIGHT;//-__OFFSET;
		break;
	}
	colorSpace(_colorspaceData);

		writecommand(CMD_MADCTL);
		writedata(_Mactrl_Data);
}
