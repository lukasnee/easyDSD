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

typedef enum ping_pong_{

	// Stop state
	STREAM_STANDBY,			// PingPong buffer totally in standby mode.
	STREAM_PAUSED,

	// Starting state
	STREAM_PING_READ_PONG_STANDBY,  // starting to fill Ping buffer, no streaming at this state.

	// Active states - continuous dataflow. Two flipping states.
	STREAM_PING_STREAM_PONG_READ, 	// Ping start to be streamed, while Pong reads in data.
	STREAM_PING_READ_PONG_STREAM,	// Process flips: Ping reads data in, while Pong streams.

	// Stopping states - which of two depends on the active bi-state.
	STREAM_PING_HALT_PONG_STREAM,	// this if stopped on PP_PI_READ_PO_STREAM active state.
	STREAM_PING_STREAM_PONG_HALT,	// this if stopped on PP_PI_STREAM_PO_READ active state.

}ping_pong_e;

class Stream : private virtual SD {

public:

	/* INTERFACE.*/

	/* TEMPORARY: routine workaround because no OS task todo */
	void routineIteration(void) {
		DEBUG_SIG.set(0);
		routine();
		DEBUG_SIG.reset(0);
	}


	// note.file must already be opened in FAT file-system.
	bool start(uint64_t streamStartPos, uint64_t streamPos,
			uint64_t streamEndPos, uint16_t blockSize);
	bool stop(void);
	bool pause(void);
	bool resume(void);

	/* useful streamer state identifiers */
	bool isBusy(void);

	/* streamer sample data read pointer position managing methods */

	/* for moving position relative to start of sample data
	 * NOTE. position is interpreted in width of bytes. */
	bool moveStreamPos(uint64_t position);
	/* for moving stream pointer relative to current position. */
	bool advanceStreamPos(uint64_t step);
	uint64_t getSampleDataPos(void);

	Stream(void) :
		_streamStartPos(0),
		_streamPos(0),
		_streamEndPos(0),
		_blockSize(0),
		_state(STREAM_STANDBY),
		_statePrev(STREAM_STANDBY),
		_last_i2s_state(I2S_UNKNOWN)
	{

	};

private:

	/* stream state */
	bool pingIsStreamingPongIsReading(void);
	bool pingIsReadingPongIsStreaming(void);
	bool isStarting(void);
	bool isStopping(void);
	ping_pong_e getState(void);
	void setState(ping_pong_e state);
	void flipActiveState(void);
	bool hardwareStateChanged(void);
	bool stateChanged(void);

	/* start routine on given parameters */
	bool startStream(void);
	/* streaming routine, should be continuously active (todo fit in a os task) */
	void routine(void);
	/* stream escape routine */
	bool endStream(void);

	/* validate stream read */
	bool validateRead(uint16_t expectedBytes);
	/* f11or tracking position within streamer. Not for user. */
	void trackSampleDataPosPtr(uint64_t step);

	/* NOTE. all stream psitions interpreted in width of bytes.
	relative to file's first byte position. */
	uint64_t _streamStartPos;
	uint64_t _streamPos;
	uint64_t _streamEndPos;
	uint16_t _blockSize;

	/* streamer dataflow state */
	ping_pong_e _state, _statePrev;

	/* trigger for letting Stream object know which
	 * data block is streaming at the moment */
	i2s_state_e _last_i2s_state;

};

#endif /* EASYDSD_INCLUDE_STREAM_HPP_ */
