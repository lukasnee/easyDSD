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

			break;

		case P_PREPARE_TO_PLAY:


			break;

		case P_PLAYING:

			break;

		case P_PAUSING:

			break;

		case P_PAUSED:

			break;

		case P_RESUMING:

			break;

		case P_STOPPING:

			break;

		}
	}
}
