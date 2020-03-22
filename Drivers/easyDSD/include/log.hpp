/*
 * log.hpp
 *
 *  Created on: 2020-02-26
 *      Author: Lukas
 */

#ifndef EASYDSD_INCLUDE_LOG_HPP_
#define EASYDSD_INCLUDE_LOG_HPP_

#include "Adafruit_GFX.h"

class Logger{

#define	BOUNDS_CHECK_STEP 20
#define LOG_SIZE 500

public:

	Logger(Adafruit_GFX* disp, uint16_t x, uint16_t y, uint16_t w, uint16_t h) :
		_disp(disp), _x(), _y(y), _w(w), _h(h), _head(0) { };
	void log(String str);
	void log(char c) {

		char ch[2];
		ch[0] = c;
		ch[1] = '\0';
		log(ch);

	}
	void draw(void);
	void clear(void) {

		memset(_log, 0, LOG_SIZE);
		_head = 0;

	}

private:
	Adafruit_GFX* _disp; /* gfx to draw in */
	uint16_t _x, _y, _w, _h; /* log window position and dimensions */
	char _log[LOG_SIZE] = { 0 }; /* log buffer */
	uint32_t _head; /* last entered symbol index in _log buffer */
};

#endif /* EASYDSD_INCLUDE_LOG_HPP_ */
