/*
 * log.cpp
 *
 *  Created on: 2020-02-26
 *      Author: Lukas
 */

#include "log.hpp"

void Logger::log(String str) {

	for(char const * i = str; *i != '\0'; i++, _head++){
		_head %= LOG_SIZE;
		_log[_head] = *i;
	}
	_log[_head] = '\0';
}
void Logger::draw(void) {

	static uint16_t drawFrom = 0;
	int16_t x1 = 0, y1 = 0;
	uint16_t w = _w + 1, h = _h + 1;

	_disp->setTextWrap(true);
	_disp->setBounds(_w, _h);
	_disp->setFont(&wucyFont8pt7b);
	_disp->setTextSize(1);

	uint16_t it = LOG_SIZE;

	while(it) {
		it--;
		_disp->getTextBounds(_log+drawFrom, _x, _y, &x1, &y1, &w, &h);
		if(h > _h - 1*_disp->getCharMaxHeight()) {
			drawFrom++;
			drawFrom %= LOG_SIZE;
		}
		else{
			break;
		}
	}
	_disp->setDrawColor(C_BLACK);
	_disp->fillRect(_x, _y, _w, _h);

	_disp->setTextColor(C_LIME);
	_disp->setCursor(_x, _y);
	_disp->print(_log+drawFrom);
}
