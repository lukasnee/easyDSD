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

/* NOTE. FILE MUST ALREADY BE OPENED */
bool Stream::start(
		uint64_t streamStartPos,
		uint64_t streamPos,
		uint64_t streamEndPos,
		uint16_t blockSize
		)
{

	if(streamStartPos < streamEndPos &&
			blockSize && blockSize <= EDSD_MAX_BUF_SIZE) {
		_streamStartPos = streamStartPos;
		_streamEndPos = streamEndPos;
		_blockSize = blockSize;

		if(moveStreamPos(streamPos)) {

			/* todo DMA start !!! */
			setState(STREAM_PING_READ_PONG_STANDBY);
			routine();	/* todo start routine task in os */
			return true;
		}
	}
	endStream();
	return false;
};

bool Stream::stop(void) {

	if(getState() == STREAM_PING_STREAM_PONG_HALT)
		setState(STREAM_PING_STREAM_PONG_HALT);
	else if (getState() == STREAM_PING_HALT_PONG_STREAM)
		setState(STREAM_PING_HALT_PONG_STREAM);

	if(endStream())
		return true;
	return false;
};


bool Stream::pause(void) {

	if(getState() != STREAM_STANDBY || getState() != STREAM_PAUSED) {
		setState(STREAM_PAUSED);
		return true;
	}
	return false;
};

bool Stream::resume(void) {

	if(getState() == STREAM_PAUSED) {
		setState(STREAM_PING_READ_PONG_STANDBY);
		return true;
	}
	return false;
};

/* streamer state identifiers */

bool Stream::isStandby(void) {
	return _state == STREAM_STANDBY;
}

bool Stream::isActive(void) {
	return ((_state != STREAM_STANDBY) &&
			(_state != STREAM_PAUSED));
}

/* full buffer access */
buff_stream_3ar* Stream::bufferRW(void) {
	return reinterpret_cast<buff_stream_3ar*>(_arStreamBuffer);
}

buff_stream_3ar const * Stream::bufferR(void) {
	return bufferRW();
}

/* PingPong access */
buff_ping_pong_2ar* Stream::bufferChannelRW(channel_e ch) {
	return ((ch < EDSD_MAX_CHANNELS) ?
			reinterpret_cast<buff_ping_pong_2ar*>(_arStreamBuffer[ch]) : NULL);
}

buff_ping_pong_2ar const * Stream::bufferPingPongR(channel_e ch) {
	return bufferChannelRW(ch);
}

/* individual block access */
buff_block_ar* Stream::bufferBlockRW(channel_e ch, pingpong_e pp) { return ((pp < 2) ?
			reinterpret_cast<buff_block_ar*>(bufferChannelRW(ch)[pp]) : NULL);
}

buff_block_ar const * Stream::bufferBlockR(channel_e ch, pingpong_e pp) {
	return bufferBlockRW(ch, pp);
}

/* triggers for letting streamer know if
 * data block succeeded streaming via DMA */

void Stream::alertPingBuffFinishedStreaming(void) {
	_state = STREAM_PING_READ_PONG_STREAM;
}

void Stream::alertPongBuffFinishedStreaming(void) {
	_state = STREAM_PING_STREAM_PONG_READ;
}

/* for moving position relative to start of sample data
 * NOTE. position is interpreted in width of bytes. */
bool Stream::moveStreamPos(uint64_t position) {
	if(position >= _streamStartPos && position <= _streamEndPos) {
		if(SD::lseek(_streamStartPos + position) == SD_OK) {
			_streamPos = position;
			return true; // success
		}
	}
	return false; // failed: out of bounds or SD error...
}

/* for moving stream pointer relative to current position. */
bool Stream::advanceStreamPos(uint64_t step) {
	if(moveStreamPos(getSampleDataPosPos() +  step))
		return true;
	return false;
}

uint64_t Stream::getSampleDataPosPos(void) {
	return _streamPos;
}

bool Stream::pingIsStreamingPongIsReading(void) {
	return _state == STREAM_PING_STREAM_PONG_READ;
}

bool Stream::pingIsReadingPongIsStreaming(void) {
	return _state == STREAM_PING_READ_PONG_STREAM;
}

bool Stream::isStarting(void) {
	return _state == STREAM_PING_READ_PONG_STANDBY;
}

bool Stream::isStopping(void) {
	return ((_state == STREAM_PING_HALT_PONG_STREAM) ||
			(_state == STREAM_PING_STREAM_PONG_HALT));
}

ping_pong_e Stream::getState(void) {
	return _state;
}

void Stream::setState(ping_pong_e state) {
	_state = state;
}

void Stream::flipActiveState(void) {
	_state = ((_state == STREAM_PING_READ_PONG_STREAM) ?
	STREAM_PING_STREAM_PONG_READ :
	STREAM_PING_READ_PONG_STREAM);
}

bool Stream::stateHasChanged(void) {
	if(_state != _statePrev) {
		_statePrev = _state; return true;
	}
	return false;
}

/* streaming routine, should be continuously active (todo fit in a os task) */
void Stream::routine(void) {

	// while(true) { // todo when ported to OS
	// osDelay(1);

	if(stateHasChanged()) {

		if(_streamPos > _streamEndPos)
			endStream();

		switch(getState()) {

		case STREAM_STANDBY: break;
		case STREAM_PAUSED: break;

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

/* stream escape */
bool Stream::endStream(void) {

	_streamStartPos = _streamPos = _streamEndPos = _blockSize = 0;
	_state = _statePrev = STREAM_STANDBY;

	/* todo all channel DMA STOP */

	if(SD::close() == SD_OK)
		return true;
	return false;
}

void Stream::trackSampleDataPosPtr(uint64_t step) {
	_streamPos += step;
}
