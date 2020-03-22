/*
	Data stream manager. Circular ping-pong buffer method.

	1. Mass Storage memory ->
	2. Circular ping-pong RAM buffer ->
	3. Pulse-Density Modulation (PDM) data stream ->
	4. DSD DAC.

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

#include "stream.hpp"

void Stream::routine(void) {

	// while(true) { // todo when ported to OS
	// osDelay(1);

	if(stateHasChanged()) {

		if(_streamPos > _streamEndPos)
			endStream();

		switch(getState()) {

		case STREAM_STANDBY: break;

		case STREAM_PING_STREAM_PONG_READ:
			SD::read(bufferBlockRW(CH_LEFT, PP_PONG), _blockSize);
			SD::read(bufferBlockRW(CH_RIGHT, PP_PONG), _blockSize);
			break;

		case STREAM_PING_READ_PONG_STANDBY:
		case STREAM_PING_READ_PONG_STREAM:
			SD::read(bufferBlockRW(CH_LEFT, PP_PING), _blockSize);
			SD::read(bufferBlockRW(CH_RIGHT, PP_PING), _blockSize);
			break;

		case STREAM_PING_HALT_PONG_STREAM: break;
		case STREAM_PING_STREAM_PONG_HALT: break;

		}
	}
//}
}
