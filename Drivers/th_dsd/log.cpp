/*
 * log.cpp
 *
 *  Created on: 2020-02-26
 *      Author: Lukas
 */

#include "log.hpp"

//works well with small numbers, be careful.
static void circularStep(uint32_t &value, int32_t steps, uint32_t range) {

	int32_t s = steps;

	if(steps >= 0) {
		while(value + s >= range) {
			s -= range - value;
			value = 0;
		}
	}
	else {
		while(value < (uint32_t)(-1*s)) { // if loops over
			s += value + 1;
			value = range - 1;
		}
	}
	value += s;
}

void Logger::log(String str) {

	for(char const * i = str; *i != '\0'; i++, circularStep(_head,1,LOG_SIZE))
		_log[_head] = *i;

	_log[_head] = '\0';
}
void Logger::draw(void) {

	int16_t x1 = 0, y1 = 0;
	uint16_t w = _w + 1, h = _h + 1;
	uint32_t drawFrom = 0;

	drawFrom = _head;
	circularStep(drawFrom, -1, LOG_SIZE);

	_disp->setTextWrap(true);
	_disp->setBounds(_w, _h);
	_disp->setFont(&wucyFont8pt7b);
	_disp->setTextSize(1);

	int16_t it = LOG_SIZE;
	while(it > -1) {
		it-= BOUNDS_CHECK_STEP;
		_disp->getTextBounds(_log + drawFrom,
				_x, _y + _disp->getCharMaxHeight(),
				&x1, &y1,
				&w, &h,
				_log, _log + LOG_SIZE - 1);

		if(h > _h -_disp->getCharMaxHeight())
			break;
		else
			circularStep(drawFrom, -1* BOUNDS_CHECK_STEP, LOG_SIZE);
	}
	if(it)
		circularStep(drawFrom, BOUNDS_CHECK_STEP, LOG_SIZE);
	else
		drawFrom = 0;

	if(*(_log + drawFrom) == '\n')
		circularStep(drawFrom, 1, LOG_SIZE);

	_disp->setDrawColor(C_BLACK);
	_disp->fillRect(_x, _y, _w, _h);

	_disp->setTextColor(C_LIME);
	_disp->setCursor(_x, _y + _disp->getCharMaxHeight());
	//_disp->printDebug(_log, LOG_SIZE);
	for(uint32_t i = drawFrom; *(_log + i) != '\0'; circularStep(i, 1, LOG_SIZE)) {
		_disp->write(*(_log + i));
	}
}
