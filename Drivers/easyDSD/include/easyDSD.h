/*

	easyDSD - Audio Player for Direct Stream Digital (DSD) files.
	v1.0. Currently only [.dsf] files supported.

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


#ifndef EASYDSD_H_
#define EASYDSD_H_

#ifdef __cplusplus
extern "C" {
#endif

/* USER CONFIGURATIONS */

/* 2 for stereo, etc. NOTE.
 * NOTE. currently only stereo supported !
 * */
#define EDSD_MAX_CHANNELS 2

/* Max buffer size in bytes for each channel. Should be matched
 * with SD card read block size for optimal performance.
 * NOTE. Defined size eventually will be doubled because of
 * ping-pong buffer stream method and multpilied for every channel.
 * See TOTAL_BUFFER_SIZE calculated expression for exact size.
 * */
#define EDSD_MAX_BUF_SIZE 4096

extern void HAL_I2S_TxHalfCpltCallback(I2S_HandleTypeDef *hi2s);
extern void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef *hi2s);

/* USER CONFIGURATIONS END */




#define EDSD_PING_PONG 2
#define EDSD_MAX_PINGPONG_BUF_SIZE (EDSD_MAX_BUF_SIZE * EDSD_PING_PONG)
#define EDSD_PING_BUF_SIZE (EDSD_MAX_PINGPONG_BUF_SIZE / 2)
#define EDSD_PONG_BUF_SIZE (EDSD_MAX_PINGPONG_BUF_SIZE / 2)

/* Total space used in RAM for DSD streaming. [in bytes] */
#define TOTAL_BUFFER_SIZE (EDSD_PINGPONG_BUF_SIZE * EDSD_MAX_CHANNELS)

void th_dsd_start(void); /* start easyDSD externally from c source */

#ifdef __cplusplus
}
#endif

#endif /* EASYDSD_H_ */
