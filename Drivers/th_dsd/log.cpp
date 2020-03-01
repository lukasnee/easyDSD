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

	int16_t x1 = 0, y1 = 0;
	uint16_t w = _w + 1, h = _h + 1;
	uint16_t drawFrom = 0;

	drawFrom = _head ? _head - 1 : LOG_SIZE - 1;

	_disp->setTextWrap(true);
	_disp->setBounds(_w, _h);
	_disp->setFont(&wucyFont8pt7b);
	_disp->setTextSize(1);

	uint16_t it = LOG_SIZE;
	while(it) {
		it--;

		//todo: this fnc can't calculate circular buffer string
		_disp->getTextBounds(_log + drawFrom,
				_x, _y + _disp->getCharMaxHeight(),
				&x1, &y1,
				&w, &h);

		if(h > _h -_disp->getCharMaxHeight())
			break;
		else
			drawFrom = drawFrom ? drawFrom - 1 : LOG_SIZE - 1;
	}
	if(it)
		drawFrom = (drawFrom + 1) % LOG_SIZE;
	else
		drawFrom = 0;

	if(*(_log + drawFrom) == '\n')
		drawFrom = (drawFrom + 1) % LOG_SIZE;

	_disp->setDrawColor(C_BLACK);
	_disp->fillRect(_x, _y, _w, _h);

	_disp->setTextColor(C_LIME);
	_disp->setCursor(_x, _y + _disp->getCharMaxHeight());
	//_disp->printDebug(_log, LOG_SIZE);
	for(uint16_t i = drawFrom; *(_log + i) != '\0'; i++) {
		i %= LOG_SIZE;
		_disp->write(*(_log + i));
	}
}
