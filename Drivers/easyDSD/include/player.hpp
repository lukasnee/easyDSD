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
		_trackIsActive(false),
		_activeTrackName{ 0 }
	{

	};

	void play(const char * file_name)
	{
		if(!_trackIsActive && getState() == P_STOPPED) {

			/*"2L-125_stereo-2822k-1b_04.dsf"*/
			/*"03 - Roxy Music - Avalon.dsf"*/

			// open .dsf file, read first block for header
			if(SD::open(file_name,  SD_READ) == SD_OK && readDSFheader()) {

				strcpy(_activeTrackName, file_name);

				uint64_t startPos = dsf.getSampleDataP();
				uint64_t endPos = startPos + dsf.getSampleDataSize();
				uint16_t blockSize = dsf.getBlockSizePerChannel();

				stream.start(startPos, startPos, endPos, blockSize);

			}
		}
	};

	void stop(void)
	{
		if(_trackIsActive && getState() == P_PLAYING)
			setState(P_STOPPING);
	};

	void pause(void)
	{
		if(_trackIsActive && getState() == P_PLAYING)
			setState(P_PAUSING);
	};

	void resume(void)
	{
		if(_trackIsActive && getState() == P_PAUSED)
			setState(P_RESUMING);
	};

	uint32_t getElapsedPlayTime(void) const {

		uint32_t elapsedTime = stream.getSampleDataPos();
		elapsedTime = elapsedTime * 8 /
				dsf.getChannelNum() / dsf.getBitsPerSample() /
				dsf.getSamplingFreq();

		return elapsedTime;
	}


	auto & getTrackFileName(void) const { return _activeTrackName; }


	void updateState(void) {

		if(stream.isStandby()) {

			if(getState() == P_STOPPING)
				setState(P_STOPPED);
			else if (getState() == P_PAUSING)
				setState(P_PAUSED);
		}
		if(stream.isActive()) {

			if(getState() == P_PREPARE_TO_PLAY ||
					getState() == P_RESUMING)
				setState(P_PLAYING);
		}
	}

	p_state_e getState(void)
	{
		updateState();
		return _state;
	};


	bool IsActive(void) const { return _trackIsActive; };


private:

	Stream stream;
	DSF dsf;

	p_state_e _state;
	bool _trackIsActive;
	char _activeTrackName[MAX_TRACK_NAME_SIZE];

	void setState(p_state_e state) { _state = state; };
	void SetTrackActive(void) { _trackIsActive = true; };
	void SetTrackInactive(void) { _trackIsActive = false; };


	bool readDSFheader(void) {

		/* todo check if it is .dsf, if not, do not parse as dsf */
		// parse dsf header
		uint8_t * headerBlockRoughly = new uint8_t[100];
		if(SD::read(headerBlockRoughly, sizeof(headerBlockRoughly)) != SD_OK)
			return false;
		if(!dsf.readHeader(headerBlockRoughly))
			return false;
		delete headerBlockRoughly;
		return true;
	}
};

#endif /* EASYDSD_INCLUDE_PLAYER_HPP_ */
