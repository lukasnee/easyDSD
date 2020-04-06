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
			blockSize && blockSize <= I2S_MAX_BUF_SIZE) {

		_streamStartPos = streamStartPos;
		_streamEndPos = streamEndPos;
		_blockSize = blockSize;

		if(startStream())
			return true;
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

bool Stream::isBusy(void) {
	return ((_state != STREAM_STANDBY) && (_state != STREAM_PAUSED));
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
	if(moveStreamPos(_streamPos +  step))
		return true;
	return false;
}

uint64_t Stream::getSampleDataPos(void) {
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

bool Stream::hardwareStateChanged(void) {

	// check only one of the channels because all should always be in sync anyways...

	i2s_state_e state = i2s.getState();
	if(_last_i2s_state != state) {
		_last_i2s_state = state;

		switch(state) {

		case I2S_STREAMING_PING:
			_state = STREAM_PING_STREAM_PONG_READ;
			break;
		case I2S_STREAMING_PONG:
			_state = STREAM_PING_READ_PONG_STREAM;
			break;
		default:
			_state = STREAM_STANDBY;
			break;
		}
		return true;
	}
	return false;
}

bool Stream::stateChanged(void) {

	hardwareStateChanged();

	if(_state != _statePrev) {
		_statePrev = _state;
		return true;
	}
	return false;
}

bool Stream::startStream(void) {

	if(moveStreamPos(_streamStartPos)) {

		setState(STREAM_PING_READ_PONG_STREAM);

		i2s.left.startCircularDMA(_blockSize);
		i2s.right.startCircularDMA(_blockSize);

		/* todo start routine task in os here */
		return true;
	}
	return false;
}

inline bool Stream::validateRead(uint16_t expectedBytes) {
/*
	if(SD::getBytesRead() == expectedBytes) {
		trackSampleDataPosPtr(expectedBytes);
		return true;
	}

	else {
		endStream();
		return false;
	}
*/
	return true;
}

/* streaming routine, should be continuously active (todo fit in a os task) */
void Stream::routine(void) {

	if(stateChanged()) {

		if(_streamPos > _streamEndPos)
			endStream();

		switch(getState()) {

			case STREAM_STANDBY:
			case STREAM_PAUSED:
				i2s.left.stopCircularDMA();
				i2s.right.stopCircularDMA();
				break;

			case STREAM_PING_STREAM_PONG_READ:
				SD::read(i2s.left.getBufferPing(), i2s.left.getSplitSize());
				if(!validateRead(i2s.left.getSplitSize())) break;

				SD::read(i2s.right.getBufferPing(), i2s.right.getSplitSize());
				if(!validateRead(i2s.right.getSplitSize())) break;
				break;

			case STREAM_PING_READ_PONG_STREAM:

				SD::read(i2s.left.getBufferPong(), i2s.left.getSplitSize());
				if(!validateRead(i2s.left.getSplitSize())) break;

				SD::read(i2s.right.getBufferPong(), i2s.right.getSplitSize());
				if(!validateRead(i2s.right.getSplitSize())) break;

				break;

			case STREAM_PING_HALT_PONG_STREAM: break;
			case STREAM_PING_STREAM_PONG_HALT: break;

		}
	}
}

/* stream escape */
bool Stream::endStream(void) {

	_streamStartPos = _streamPos = _streamEndPos = _blockSize = 0;
	_last_i2s_state = I2S_UNKNOWN;
	_state = _statePrev = STREAM_STANDBY;

	/* todo all channel DMA STOP */

	if(SD::close() == SD_OK)
		return true;
	return false;
}

void Stream::trackSampleDataPosPtr(uint64_t step) {
	_streamPos += step;
}
