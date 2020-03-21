/*

	Direct Stream Digital (DSD) player. C wrapper.

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


#ifndef OPENDSD_H_
#define OPENDSD_H_

#ifdef __cplusplus
extern "C" {
#endif

extern void HAL_I2S_TxHalfCpltCallback(I2S_HandleTypeDef *hi2s);
extern void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef *hi2s);

extern uint8_t dsd_PingPongState;

#define DSD_PING_PONG 2

#define DSD_PING_BUF_SIZE 4096
#define DSD_PONG_BUF_SIZE 4096
#define DSD_PINGPONG_BUF_SIZE 8192

#define DSD_PING_READ_PONG_STREAM() dsd_PingPongState = 1
#define DSD_PING_STREAM_PONG_READ() dsd_PingPongState = 0

#define DSD_PING_PONG_FLIP() dsd_PingPongState = dsd_PingPongState ? 0 : 1
#define DSD_PING 0
#define DSD_PONG 1


void th_dsd_start(void); /* start openDSD externally from c source */



#ifdef __cplusplus
}
#endif

#endif /* OPENDSD_H_ */
