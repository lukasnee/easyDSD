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
			blockSize && blockSize <= i2s.left.getSplitSize()) {

		_streamStartPos = streamStartPos;
		_streamEndPos = streamEndPos;
		_blockSize = blockSize;

		if(moveStreamPointer(_streamStartPos)) {

			if(readNewPingData() && readNewPongData() && i2s.startCircularDMA(_blockSize))
					return true;/* todo start routine task in os here */
		}
	}
	stop();
	return false;
};

bool Stream::stop(void)
{
	stream_state_e state = getState();

	if(state == SS_STREAMING_PING || state == SS_STREAMING_PONG) {

		if(!pause())
			return false;

		_streamStartPos = _streamPos = _streamEndPos = _blockSize = 0;
		_state = _prevState = SS_STANDBY;

		if(SD::close() == SD_OK)
			return true;
	}
	return false;
};


bool Stream::pause(void)
{
	stream_state_e state = getState();

	if(state == SS_STREAMING_PING || state == SS_STREAMING_PONG) {

		if(i2s.stopCircularDMA()) {
			_state = SS_PAUSED;
			return true;
		}
	}
	return false;
};

bool Stream::resume(void)
{
	stream_state_e state = getState();

	if(state == SS_PAUSED) {
		if(readNewPingData() &&
			i2s.startCircularDMA(_blockSize))
				return true;/* todo start routine task in os here */
	}
	return false;
};

/* for moving position relative to start of sample data
 * NOTE. position is interpreted in width of bytes. */
bool Stream::moveStreamPointer(uint64_t bytewisePosition)
{
	if(bytewisePosition >= _streamStartPos && bytewisePosition <= _streamEndPos) {
		if(SD::lseek(_streamStartPos + bytewisePosition) == SD_OK) {
			_streamPos = bytewisePosition;
			return true; // success
		}
	}
	return false; // failed: out of bounds or SD error...
}

/* for moving stream pointer relative to current position. */
bool Stream::advanceStreamPointer(uint64_t bytewiseStep)
{
	if(moveStreamPointer(_streamPos +  bytewiseStep))
		return true;
	return false;
}

uint64_t Stream::getStreamPointer(void)
{
	return _streamPos;
}

bool Stream::readNewPingData(void)
{
	DEBUG_SIG.set(0);
	//SD::lseek(getStreamPointer());
	//left channel ping buffer
	SD::read(i2s.left.getBufferPing(), _blockSize);
	if(!validateRead(_blockSize))
		return false;
	//right channel ping buffer
	SD::read(i2s.right.getBufferPing(), _blockSize);
	if(!validateRead(_blockSize))
		return false;

	DEBUG_SIG.reset(0);
	return true;
}

bool Stream::readNewPongData(void)
{
	DEBUG_SIG.set(1);
	//SD::lseek(getStreamPointer());
	//left channel pong buffer
	SD::read(i2s.left.getBufferPong(), _blockSize);
	if(!validateRead(_blockSize))
		return false;
	//right channel pong buffer
	SD::read(i2s.right.getBufferPong(), _blockSize);
	if(!validateRead(_blockSize))
		return false;

	DEBUG_SIG.reset(1);
	return true;
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
	return (_state == SS_STREAMING_PING || _state == SS_STREAMING_PONG);
}

void Stream::trackSampleDataPosPtr(uint64_t step)
{
	_streamPos += step;
}

inline bool Stream::validateRead(uint16_t expectedByteNum)
{
	if(expectedByteNum == SD::getBytesRead())
		trackSampleDataPosPtr(expectedByteNum);

	if(getStreamPointer() > _streamEndPos) {
		stop();
		return false;
	}
	return true;
}
