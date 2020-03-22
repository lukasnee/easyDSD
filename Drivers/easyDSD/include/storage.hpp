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
#ifndef STORAGE_HPP_
#define STORAGE_HPP_

 #ifdef __cplusplus
 #define EXTERNC extern "C"
 #else
 #define EXTERNC
 #endif

#ifdef __cplusplus
extern "C" {
#endif

#include "stdio.h"

#ifdef __cplusplus
}
#endif

#include "../../easyDSD/include/hwg.hpp"
#include "fatfs.h"			/* FAT32 file system */

#define FR_BEGIN \
	FRESULT res = FR_OK;

#define FR_TRY(function) \
		res = function; \
		if (res != FR_OK ) Storage::errorHandler(res)	\


#define FR_DO \
		do {

#define FR_WHILE(function) \
			res = function; \
		}while(res == FR_OK)


#define FR_END fr_end:;

#define FR_END_R fr_end:;\
	return res;

extern uint8_t retSD;    /* Return value for SD */
extern char SDPath[4];   /* SD logical drive path */
extern FATFS SDFatFS;    /* File system object for SD logical drive */
extern FIL SDFile;       /* File object for SD */



class Storage {

public:

	Storage(void) : seekPos(0)
	{
		mount();
	};

	~Storage(void) {};

	static void errorHandler(FRESULT r);

	FRESULT mount(void),
			unmount(void),
			open(const TCHAR* path,	BYTE mode),
			close(void),
			write(const void* buff, UINT btw, UINT &bw),
			read(void* buff, UINT btr, UINT &br),
			lseek(FSIZE_t offset),
			getSDPath(String path),
			scanFiles(char* path);

	uint32_t getSeekPos(void) { return seekPos; };
	void setSeekPos(uint32_t position) { seekPos = position; };
	void advanceSeekPosBy(int32_t step) { seekPos += step; }


protected:
private:

	uint32_t seekPos;

};


#endif  // STORAGE_HPP_
