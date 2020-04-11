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
		uint64_t streamPos,
		uint64_t streamStartPos,
		uint64_t streamEndPos,
		uint16_t blockSize
		)
{
	if(streamStartPos < streamEndPos &&
		streamPos < streamEndPos &&
		blockSize && blockSize <= i2s.left.getSplitSize()) {

		_streamStartPos = streamStartPos;
		_streamEndPos = streamEndPos;
		_blockSize = blockSize;

		 /* move to _streamStartPos */
		if(moveStreamPointer(streamPos)) {

			_state = SS_STARTING;
			if(i2s.startCircularDMA(_blockSize))
				return true;/* todo start routine task in os here */
		}
	}
	stop();
	return false;
};

bool Stream::stop(void)
{
	stream_state_e state = getState();

	if(pause()) {
		_streamStartPos = _streamPos = _streamEndPos = _blockSize = 0;
		_state = SS_STANDBY;
		if(SD::close())
			return true;
	}
	return false;
};


bool Stream::pause(void)
{
	stream_state_e state = getState();

	if(isBusy() && i2s.stopCircularDMA()) {
		_state = SS_PAUSED;
		return true;
	}
	return false;
};

bool Stream::resume(void)
{
	if(getState() == SS_PAUSED) {
		if(i2s.startCircularDMA(_blockSize))
				return true;/* todo start routine task in os here */
	}
	return false;
};

/* for moving position relative to start of sample data
 * NOTE. position is interpreted in width of bytes. */
bool Stream::moveStreamPointer(uint64_t bytewisePosition)
{
	if(bytewisePosition < _streamEndPos - _streamStartPos) {
		if(SD::lseek(_streamStartPos + bytewisePosition)) {
			_streamPos = bytewisePosition;
			return true; // success
		}
	}
	return false; // failed: out of bounds or SD error...
}

/* for moving stream pointer relative to current position. */
bool Stream::advanceStreamPointer(uint64_t bytewiseStep)
{
	if(moveStreamPointer(_streamPos + bytewiseStep))
		return true;
	return false;
}

uint64_t Stream::getStreamPointer(void)
{
	return _streamPos;
}

bool Stream::readNewPingData(void)
{
	bool res = true;

	DEBUG_SIG.set(0);

	//left channel ping buffer
	SD::read(i2s.left.getBufferPing(), _blockSize);
	if(!readIsValid(_blockSize)) {
		res = false;
		goto end;
	}

	//right channel ping buffer
	SD::read(i2s.right.getBufferPing(), _blockSize);
	if(!readIsValid(_blockSize)) {
		res = false;
		goto end;
	}

	end:;
	DEBUG_SIG.reset(0);
	return res;
}

bool Stream::readNewPongData(void)
{
	bool res = true;

	DEBUG_SIG.set(1);

	//left channel ping buffer
	SD::read(i2s.left.getBufferPong(), _blockSize);
	if(!readIsValid(_blockSize)) {
		res = false;
		goto end;
	}

	//right channel ping buffer
	SD::read(i2s.right.getBufferPong(), _blockSize);
	if(!readIsValid(_blockSize)) {
		res = false;
		goto end;
	}

	end:;
	DEBUG_SIG.reset(1);
	return res;
}

/* streaming routine, should be continuously active (todo fit in a os task) */
bool Stream::routine(void)
{
	i2s_state_e i2s_state = i2s.getState();

	if(i2s_state == I2S_STREAMING_PING && _state != SS_STREAMING_PING) {
		_state = SS_STREAMING_PING;
		if(readNewPongData())
			return true;
	}
	else if(i2s_state == I2S_STREAMING_PONG && _state != SS_STREAMING_PONG) {
		_state = SS_STREAMING_PONG;
		if(readNewPingData())
			return true;
	}
	return false;
}

bool Stream::isBusy(void)
{
	return (_state == SS_STREAMING_PING || _state == SS_STREAMING_PONG || _state == SS_STARTING);
}

void Stream::trackSampleDataPosPtr(uint64_t step)
{
	_streamPos += step;
}

inline bool Stream::readIsValid(uint16_t expectedByteNum)
{
	if(expectedByteNum != SD::getBytesRead() || getStreamPointer() + _streamStartPos > _streamEndPos) {
		stop();
		return false;
	}
	trackSampleDataPosPtr(expectedByteNum);
	return true;
}
