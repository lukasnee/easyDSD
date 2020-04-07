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


#ifndef EASYDSD_INCLUDE_STREAM_HPP_
#define EASYDSD_INCLUDE_STREAM_HPP_

#include <stdio.h>
#include "Storage.hpp"
#include "hwg.hpp"
#include "easyDSD.h"

/*
TODO:
	- add frequency configuring
	- add support for PCM
	- add support for multi-bit DSD (maybe)
*/

typedef enum stream_state_{

	SS_STANDBY,
	SS_PAUSED,
	SS_STREAMING_PING, // While ping buffer gets streamed, new following data reads into pong buffer.
	SS_STREAMING_PONG, // mirror process

}stream_state_e;

class Stream : private virtual SD {

public:

	/* INTERFACE.*/

	/* starts streaming on given parameters
	 * note. file must already be opened in FAT file-system. */
	bool start(uint64_t streamStartPos, uint64_t streamPos,
			uint64_t streamEndPos, uint16_t blockSize);
	/* just stops streaming */
	bool pause(void);
	/* simply continues streaming */
	bool resume(void);
	/* stops streaming, forgets state, closes SD file */
	bool stop(void);
	/* returns current stream position bytewise */
	uint64_t getStreamPointer(void);
	/* useful streamer state identifiers */
	bool isBusy(void);
	/* for moving position relative to start of sample data */
	bool moveStreamPointer(uint64_t bytewisePosition);
	/* for fast forward, etc. Step relative to current position. */
	bool advanceStreamPointer(uint64_t bytewiseStep);

	Stream(void) :
		_streamStartPos(0),
		_streamPos(0),
		_streamEndPos(0),
		_blockSize(0),
		_state(SS_STANDBY),
		_prevState(SS_STANDBY)
	{

	};

	/* streaming routine, should be continuously active (todo fit in a os task) */
	bool routine(void); /* returns if stream state changed */

private:

	stream_state_e getState(void) { return _state; };
	/* returns true if state changed */
	bool updateState(void);
	/* Read new data to half buffers from SD storage */
	bool readNewPingData(void);
	bool readNewPongData(void);
	/* validate stream read */
	bool validateRead(uint16_t expectedByteNum);
	/* f11or tracking position within streamer. Not for user. */
	void trackSampleDataPosPtr(uint64_t step);

	/* NOTE. all stream psitions interpreted in width of bytes.
	relative to file's first byte position. */
	uint64_t _streamStartPos;
	uint64_t _streamPos;
	uint64_t _streamEndPos;
	uint16_t _blockSize;
	/* streamer dataflow state */
	stream_state_e _state, _prevState;

};

#endif /* EASYDSD_INCLUDE_STREAM_HPP_ */
