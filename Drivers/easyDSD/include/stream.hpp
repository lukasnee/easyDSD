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
#include "easyDSD.h"

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

/* for buffer access */
typedef enum pingpong_{ PP_PING, PP_PONG }pingpong_e;

/* for buffer access, todo maybe more channels support ? */
typedef enum channel_{
	CH_LEFT,
	CH_RIGHT,
}channel_e;

typedef uint8_t buff_block_ar[EDSD_MAX_BUF_SIZE];
typedef uint8_t
		buff_ping_pong_2ar[EDSD_PING_PONG][EDSD_MAX_BUF_SIZE];
typedef uint8_t
		buff_stream_3ar[EDSD_MAX_CHANNELS][EDSD_PING_PONG][EDSD_MAX_BUF_SIZE];

class Stream : private virtual SD {

	public:

		Stream(void) :
			_streamStartPos(0),
			_streamPos(0),
			_streamEndPos(0),
			_blockSize(0),
			_state(STREAM_STANDBY),
			_statePrev(STREAM_STANDBY),
			_arStreamBuffer{ 0xAA } { };

	/* stream user interface */

		/* NOTE. FILE MUST ALREADY BE OPENED */
		bool start(
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

					_streamEndPos = streamEndPos;
					setState(STREAM_PING_READ_PONG_STANDBY);
					routine();	/* todo start routine task in os */
					return true;
				}
			}
			endStream();
			return false;
		};

		bool stop(void) {

			if(getState() == STREAM_PING_STREAM_PONG_HALT)
				setState(STREAM_PING_STREAM_PONG_HALT);
			else if (getState() == STREAM_PING_HALT_PONG_STREAM)
				setState(STREAM_PING_HALT_PONG_STREAM);

			if(endStream())
				return true;
			return false;
		};


		bool pause(void) { /* todo */

			if(getState() != STREAM_STANDBY || getState() != STREAM_PAUSED) {
				setState(STREAM_PAUSED);
				return true;
			}
			return false;
		};

		bool resume(void) { /* todo */

			if(getState() == STREAM_PAUSED) {
				setState(STREAM_PING_READ_PONG_STANDBY);
				return true;
			}
			return false;
		};

	/* streamer state identifiers */

		bool isStandby(void) { return _state == STREAM_STANDBY; };
		bool isActive(void) {
			return ((_state != STREAM_STANDBY) &&
					(_state != STREAM_PAUSED)); };

	/* Main _arStreamBuffer segmented interface with
	 * Read/Write or Read-Only access and boundary protection */

		/* full buffer access */
		buff_stream_3ar* bufferRW(void) {
			return reinterpret_cast<buff_stream_3ar*>(_arStreamBuffer); };
		buff_stream_3ar const * bufferR(void) { return bufferRW(); }

		/* PingPong access */
		buff_ping_pong_2ar* bufferChannelRW(channel_e ch) {
			return ((ch < EDSD_MAX_CHANNELS) ? reinterpret_cast<buff_ping_pong_2ar*>(_arStreamBuffer[ch]) : NULL); };
		buff_ping_pong_2ar const * bufferPingPongR(channel_e ch) { return bufferChannelRW(ch); };

		/* individual block access */
		buff_block_ar* bufferBlockRW(channel_e ch, pingpong_e pp) {
			return ((pp < 2) ? reinterpret_cast<buff_block_ar*>(bufferChannelRW(ch)[pp]) : NULL);};
		buff_block_ar const * bufferBlockR(channel_e ch, pingpong_e pp) { return bufferBlockRW(ch, pp); };

	/* triggers for letting streamer know if
	 * data block succeeded streaming via DMA */

		void alertPingBuffFinishedStreaming(void) { _state = STREAM_PING_READ_PONG_STREAM; };
		void alertPongBuffFinishedStreaming(void) { _state = STREAM_PING_STREAM_PONG_READ; };

	/* streamer sample data read pointer position managing methods */

		/* for moving position relative to start of sample data
		 * NOTE. position is interpreted in width of bytes. */
		bool moveStreamPos(uint64_t position) {
			if(position >= _streamStartPos && position <= _streamEndPos) {
				if(SD::lseek(_streamStartPos + position) == SD_OK) {
					_streamPos = position;
					return true; // success
				}
			}
			return false; // failed: out of bounds or SD error...
		};

		/* for moving stream pointer relative to current position. */
		bool advanceStreamPos(uint64_t step) {
			if(moveStreamPos(getSampleDataPosPos() +  step))
				return true;
			return false;
		};

		uint64_t getSampleDataPosPos(void) { return _streamPos; };

	private:


	/* stream state */

		bool pingIsStreamingPongIsReading(void) {
			return _state == STREAM_PING_STREAM_PONG_READ; };
		bool pingIsReadingPongIsStreaming(void) {
			return _state == STREAM_PING_READ_PONG_STREAM; };
		bool isStarting(void) { return _state == STREAM_PING_READ_PONG_STANDBY; };
		bool isStopping(void) {
			return ((_state == STREAM_PING_HALT_PONG_STREAM) ||
					(_state == STREAM_PING_STREAM_PONG_HALT)); };

		ping_pong_e getState(void) { return _state; };
		void setState(ping_pong_e state) { _state = state; };
		void flipActiveState(void) { _state = ((_state == STREAM_PING_READ_PONG_STREAM) ? STREAM_PING_STREAM_PONG_READ : STREAM_PING_READ_PONG_STREAM); };
		bool stateHasChanged(void) {
			if(_state != _statePrev) {
				_statePrev = _state; return true;
			}
			return false;
		};

	/* streaming routine, should be continuously active (todo fit in a os task) */
		void routine(void);

		/* stream escape */
		bool endStream(void) {

			_streamStartPos = _streamPos = _streamEndPos = _blockSize = 0;
			_state = _statePrev = STREAM_STANDBY;
			if(SD::close() == SD_OK)
				return true;
			return false;
		}

	// for tracking position within streamer. Not for user.
		void trackSampleDataPosPtr(uint64_t step) { _streamPos += step; };

	/* NOTE. all stream psitions interpreted in width of bytes.
	relative to file's first byte position. */
		uint64_t _streamStartPos;
		uint64_t _streamPos;
		uint64_t _streamEndPos;
		uint16_t _blockSize;

	/* streamer dataflow state */
		ping_pong_e _state;
		ping_pong_e _statePrev; // for triggering a change

		/* THIS TAKES A LOT OF RAM, 16kB default,
		 * memory field must be DMA enabled */
		buff_stream_3ar _arStreamBuffer;

	};

#endif /* EASYDSD_INCLUDE_STREAM_HPP_ */
