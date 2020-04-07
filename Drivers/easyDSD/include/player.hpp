/*
	Direct Stream Digital (DSD) player core module.

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

#ifndef EASYDSD_INCLUDE_PLAYER_HPP_
#define EASYDSD_INCLUDE_PLAYER_HPP_

#define MAX_TRACK_NAME_SIZE 100

#include <cstring>

#include "storage.hpp"
#include "dsf.hpp"
#include "stream.hpp"


typedef enum p_state_{

	P_STOPPED,

	P_PREPARE_TO_PLAY,
	P_PLAYING,

	P_PAUSING,
	P_PAUSED,

	P_RESUMING,
	P_STOPPING

}p_state_e;

class Player : private virtual SD {

public:

	void task_player(void);

	Player() :
		stream(),
		_state(P_STOPPED),
		_activeTrackName{ 0 }
	{

	};

	/* TEMPORARY: routine workaround because no OS task todo */
	void playRoutine(void) {
		DEBUG_SIG.set(0);
		stream.routine();
		DEBUG_SIG.reset(0);
	}

	void play(const char * file_name)
	{
		if(!stream.isBusy() && getState() == P_STOPPED) {

			// open .dsf file, read first block for header
			if(SD::open(file_name,  SD_READ) == SD_OK && readDSFheader()) {

				strcpy(_activeTrackName, file_name);

				uint64_t startPos = dsf.getSampleDataP();
				uint64_t endPos = startPos + dsf.getSampleDataSize();
				uint16_t blockSize = dsf.getBlockSizePerChannel();

				stream.start(startPos, startPos, endPos, blockSize);
				setState(P_PLAYING);

				/* todo in os make it a stream task */
				while(true) {
					playRoutine();
				}
			}
		}
	};

	bool stop(void)
	{
		if(stream.isBusy() && getState() == P_PLAYING) {
			if(stream.stop()) {
				setState(P_STOPPING);
				return true;
			}
		}
		return false;
	};

	bool pause(void)
	{
		if(stream.isBusy() && getState() == P_PLAYING) {
			if(stream.pause()) {
				setState(P_PAUSING);
				return true;
			}
		}
		return false;
	};

	bool resume(void)
	{
		if(!stream.isBusy() && getState() == P_PAUSED) {
			if(stream.resume()) {
				setState(P_RESUMING);
				return true;
			}
		}
		return false;
	};

	uint32_t getElapsedPlayTime(void) {

		uint32_t elapsedTime = stream.getStreamPointer();
		elapsedTime = elapsedTime * 8 /
				dsf.getChannelNum() / dsf.getBitsPerSample() /
				dsf.getSamplingFreq();

		return elapsedTime;
	}


	const char * getTrackFileName(void) const { return _activeTrackName; }

	void updateState(void) {

		if(!stream.isBusy()) {

			if(_state == P_STOPPING)
				setState(P_STOPPED);
			else if (_state == P_PAUSING)
				setState(P_PAUSED);
		}
		if(stream.isBusy()) {

			if(_state== P_PREPARE_TO_PLAY ||
					_state == P_RESUMING)
				setState(P_PLAYING);
		}
	}

	p_state_e getState(void)
	{
		updateState();
		return _state;
	};

private:

	Stream stream;
	DSF dsf;

	p_state_e _state;
	char _activeTrackName[MAX_TRACK_NAME_SIZE];

	void setState(p_state_e state) { _state = state; };

	bool readDSFheader(void) {

		/* todo check if it is .dsf, if not, do not parse as dsf */
		// parse dsf header
		uint8_t * headerBlockRoughly = new uint8_t[100];
		if(SD::read(headerBlockRoughly, 100) != SD_OK)
			return false;
		if(!dsf.readHeader(headerBlockRoughly))
			return false;
		delete headerBlockRoughly;
		return true;
	}
};

#endif /* EASYDSD_INCLUDE_PLAYER_HPP_ */
