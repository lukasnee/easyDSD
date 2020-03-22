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
		_stream(),
		_dsf(),
		_state(P_STOPPED),
		_trackIsActive(false),
		_activeTrackName{ 0 },
		_TrackDataPtr(NULL)
	{

	};

	bool IsTrackActive(void) { return _trackIsActive; };

	void play(const char * file_name)
	{

		if(!_trackIsActive && getState() == P_STOPPED) {


			setState(P_PREPARE_TO_PLAY);
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


	void updateState(void) {

		if(_stream.isStandby()) {

			if(getState() == P_STOPPING)
				setState(P_STOPPED);
			else if (getState() == P_PAUSING)
				setState(P_PAUSED);
		}
		if(_stream.isActive()) {

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

private:

	Stream _stream;

	dsf_t _dsf;
	p_state_e _state;
	bool _trackIsActive;
	char _activeTrackName[MAX_TRACK_NAME_SIZE];

	void setTrackSampleDataPtr(uint8_t * position) { _TrackDataPos = position; };
	void advanceTrackSampleDataPosPtr(uint8_t * step) { _TrackDataPos += step; }

	uint8_t * _TrackDataPtr;


	void setState(p_state_e state) { _state = state; };
	void SetTrackActive(void) { _trackIsActive = true; };
	void SetTrackInactive(void) { _trackIsActive = false; };

};

#endif /* EASYDSD_INCLUDE_PLAYER_HPP_ */
