/*
	easyDSD main module.

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

#include "player.hpp"

void Player::task_player(void) {

	while(true) {

		switch(getState()) {

		case P_STOPPED:

			// TaskDelay(1);

			break;

		case P_PREPARE_TO_PLAY:

			/*"2L-125_stereo-2822k-1b_04.dsf"*/
			/*"03 - Roxy Music - Avalon.dsf"*/

			// open .dsf file, read first block for header
			SD::open(_activeTrackName,  SD_READ);
			SD::read(_stream.bufferBlockRW(CH_LEFT, PP_PING), sizeof(_stream.bufferBlockRW(CH_LEFT, PP_PING)));

			/* todo check if it is .dsf, if not, do not parse as dsf */
			// parse dsf header
			dsf_readHeader(reinterpret_cast<uint8_t const *>(_stream.bufferBlockR(CH_LEFT, PP_PING)), _dsf);

			if(_dsf.blockSizePerChannel > EDSD_MAX_BUF_SIZE)
				setState(P_STOPPING);

			// seek to start of sample data
			setTrackSampleDataPtr(_dsf.pSampleData);

			HAL_I2S_Transmit_DMA(&hi2s2,
					reinterpret_cast<uint16_t *>(_stream.bufferChannelRW(CH_LEFT)),
					EDSD_MAX_PINGPONG_BUF_SIZE/sizeof(uint16_t));

			HAL_I2S_Transmit_DMA(&hi2s3,
					reinterpret_cast<uint16_t *>(_stream.bufferChannelRW(CH_RIGHT)),
					EDSD_MAX_PINGPONG_BUF_SIZE/sizeof(uint16_t));

			_trackIsActive = true;
			setState(P_PLAYING);
			break;

		case P_PLAYING:
			// read sample block

			_stream.routine();

			break;

		case P_PAUSING:

			break;

		case P_PAUSED:

			break;

		case P_RESUMING:

			break;

		case P_STOPPING:
			sd.close();
			/* todo close file, stop DMA */
			_trackIsActive = false;

			break;

		}
	}
}
