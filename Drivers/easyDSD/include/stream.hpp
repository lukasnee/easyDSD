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

#include "easyDSD.h"

typedef enum ping_pong_{

	// Stop state
	PP_STANDBY,			// PingPong buffer totally in standby mode.

	// Starting state
	PP_PI_READ_PO_STANDBY,  // starting to fill Ping buffer, no streaming at this state.

	// Active states - continuous dataflow. Two flipping states.
	PP_PI_STREAM_PO_READ, 	// Ping start to be streamed, while Pong reads in data.
	PP_PI_READ_PO_STREAM,	// Process flips: Ping reads data in, while Pong streams.

	// Stopping states - which of two depends on the active bi-state.
	PP_PI_HALT_PO_STREAM,	// this if stopped on PP_PI_READ_PO_STREAM active state.
	PP_PI_STREAM_PO_HALT,	// this if stopped on PP_PI_STREAM_PO_READ active state.

}ping_pong_e;

typedef enum channel_{
	CH_LEFT,
	CH_RIGHT,
}channel_e;

typedef enum pingpong_{
	PP_PING,
	PP_PONG,
}pingpong_e;

typedef uint8_t buff_block_ar[EDSD_MAX_BUF_SIZE];
typedef uint8_t buff_ping_pong_2ar[EDSD_PING_PONG][EDSD_MAX_BUF_SIZE];
typedef uint8_t buff_stream_3ar[EDSD_MAX_CHANNELS][EDSD_PING_PONG][EDSD_MAX_BUF_SIZE];

class Stream {

	public:

		Stream(void) : _state(PP_STANDBY), _statePrev(PP_STANDBY), _arStreamBuffer{ 0xAA } {};

		void routine(void);

		ping_pong_e getState(void) { return _state; };
		void setState(ping_pong_e state) { _state = state; };
		void flipActiveState(void) { _state = ((_state == PP_PI_READ_PO_STREAM) ? PP_PI_STREAM_PO_READ : PP_PI_READ_PO_STREAM); };
		bool stateHasChanged(void) {
			if(_state != _statePrev) {
				_statePrev = _state;
				return true;
			}
			return false;
		};
		bool isStandby(void) { return _state == PP_STANDBY; };
		bool isStarting(void) { return _state == PP_PI_READ_PO_STANDBY; };
		bool isActive(void) { return ((_state == PP_PI_STREAM_PO_READ) || (_state == PP_PI_READ_PO_STREAM)); };
		bool pingIsStreamingPongIsReading(void) { return _state == PP_PI_STREAM_PO_READ; };
		bool pingIsReadingPongIsStreaming(void) { return _state == PP_PI_READ_PO_STREAM; };
		bool isStopping(void) { return ((_state == PP_PI_HALT_PO_STREAM) || (_state == PP_PI_STREAM_PO_HALT)); };

	/* Main _arStreamBuffer segmented interface with
	 * Read/Write or Read-Only access and boundary protection */

		/* full buffer access */
		buff_stream_3ar* bufferRW(void) {
			return reinterpret_cast<buff_stream_3ar*>(_arStreamBuffer); };
		buff_stream_3ar const * bufferR(void) { return bufferRW(); }

		/* PingPong access */
		buff_ping_pong_2ar* bufferChannelRW(channel_e ch) {
			return ((ch < EDSD_MAX_CHANNELS) ? reinterpret_cast<buff_ping_pong_2ar*>(_arStreamBuffer[ch]) : NULL); };
		buff_ping_pong_2ar const * bufferPingPongR(channel_e ch) { return bufferChannelRW(ch); }

		/* individual block access */
		buff_block_ar* bufferBlockRW(channel_e ch, pingpong_e pp) {
			return ((pp < 2) ? reinterpret_cast<buff_block_ar*>(bufferChannelRW(ch)[pp]) : NULL);};
		buff_block_ar const * bufferBlockR(channel_e ch, pingpong_e pp) { return bufferBlockRW(ch, pp); }

		void alertPingBuffFinishedStreaming(void) { _state = PP_PI_READ_PO_STREAM; };
		void alertPongBuffFinishedStreaming(void) { _state = PP_PI_STREAM_PO_READ; };

	private:
		ping_pong_e _state;
		ping_pong_e _statePrev;

		/* THIS TAKES A LOT OF RAM, 16kB default, memory field must be DMA enabled */
		buff_stream_3ar _arStreamBuffer;
	};

#endif /* EASYDSD_INCLUDE_STREAM_HPP_ */
