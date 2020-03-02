/*
	SD card FAT32 file explorer for openDSD.
	Glue code for your specific hardware (initially designed for STM32F407VE).

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

#include "filexp.hpp"
#include "string.h"

#ifdef __cplusplus
extern "C" {
#endif

// all of your legacy C code here

#ifdef __cplusplus
}
#endif

void Filexp:: errorHandler(FRESULT r) {

	const char* errorMsg[] {
		"FR_OK",					/* (0) Succeeded */
		"FR_DISK_ERR",				/* (1) A hard error occurred in the low level disk I/O layer */
		"FR_INT_ERR",				/* (2) Assertion failed */
		"FR_NOT_READY",				/* (3) The physical drive cannot work */
		"FR_NO_FILE",				/* (4) Could not find the file */
		"FR_NO_PATH",				/* (5) Could not find the path */
		"FR_INVALID_NAME",			/* (6) The path name format is invalid */
		"FR_DENIED",				/* (7) Access denied due to prohibited access or directory full */
		"FR_EXIST",					/* (8) Access denied due to prohibited access */
		"FR_INVALID_OBJECT",		/* (9) The file/directory object is invalid */
		"FR_WRITE_PROTECTED",		/* (10) The physical drive is write protected */
		"FR_INVALID_DRIVE",			/* (11) The logical drive number is invalid */
		"FR_NOT_ENABLED",			/* (12) The volume has no work area */
		"FR_NO_FILESYSTEM",			/* (13) There is no valid FAT volume */
		"FR_MKFS_ABORTED",			/* (14) The f_mkfs() aborted due to any problem */
		"FR_TIMEOUT",				/* (15) Could not get a grant to access the volume within defined period */
		"FR_LOCKED",				/* (16) The operation is rejected according to the file sharing policy */
		"FR_NOT_ENOUGH_CORE",		/* (17) LFN working buffer could not be allocated */
		"FR_TOO_MANY_OPEN_FILES",	/* (18) Number of open files > _FS_LOCK */
		"FR_INVALID_PARAMETER"
	};
}

FRESULT Filexp::mount(void) {
	FR_BEGIN
	FR_TRY(f_mount(&SDFatFS, (TCHAR const*)SDPath, 0));
	FR_END_R
}
FRESULT Filexp::unmount(void) {
	FR_BEGIN
	FR_TRY(f_mount(&SDFatFS, (TCHAR const*)NULL, 0));
	FR_END_R
}
FRESULT Filexp::open(const TCHAR* path,	BYTE mode) {
	FR_BEGIN
	FR_TRY(f_open(&SDFile, path, mode));
	FR_END_R
}
FRESULT Filexp::close(void) {
	FR_BEGIN
	FR_TRY(f_close(&SDFile));
	FR_END_R
}
FRESULT Filexp::write(
	const void* buff,	/* Pointer to the data to be written */
	UINT btw,			/* Number of bytes to write */
	UINT &bw			/* Pointer to number of bytes written */
) {
	FR_BEGIN
	FR_TRY(f_write(&SDFile, buff, btw, &bw));
	FR_END_R
}
FRESULT Filexp::read(
	void* buff,	/* Pointer to data buffer */
	UINT btr,	/* Number of bytes to read */
	UINT &br	/* Pointer to number of bytes read */
) {
	FR_BEGIN
	FR_TRY(f_read(&SDFile, buff, btr, &br));
	FR_END_R
}


void Filexp::list(const char * pattern) {

FR_BEGIN
	char fileStr[100];
	DIR dp;
	FILINFO fno;
	uint16_t index = 0, i_cursor = 0, i_end;

	mount();

	FR_TRY(f_findfirst(&dp, &fno, SDPath, pattern));

	_disp->setTextWrap(true);
	_disp->setBounds(_w, _h);
	_disp->setFont(&wucyFont8pt7b);
	_disp->setTextSize(1);
	_disp->setDrawColor(C_BLACK);
	_disp->fillRect(_x, _y, _w, _h);
	_disp->setCursor(_x, _y + _disp->getCharMaxHeight());

	FR_DO
		sprintf(fileStr, "%.*s %lu \n", 30, fno.fname, fno.fsize);
		_disp->setTextColor(index == i_cursor ? C_YELLOW : C_LIME);
		_disp->print(fileStr);
		index++;
	FR_WHILE(f_findnext(&dp, &fno));

FR_END
}


/*
void fooo(void) {

	FRESULT res;                                           FatFs function common result code
	UINT byteswritten, bytesread;                      File write/read counts
	uint8_t wtext[] = "Hello from bambukas :)";  File write buffer
	uint8_t rtext[100];                                    File read buffer

	if(f_mount(&SDFatFS, (TCHAR const*)SDPath, 0) == FR_OK)
	{
		HAL_Delay(200);

		//Open file for writing (Create)
		if(f_open(&SDFile, "F7FILE5.TXT", FA_CREATE_ALWAYS | FA_WRITE) == FR_OK) {

			//Write to the text file
			res = f_write(&SDFile, wtext, strlen((char *)wtext), &byteswritten);

			f_close(&SDFile);

			//Test read file
			f_open(&SDFile, "F7FILE5.TXT",  FA_READ);
			memset(rtext,0,sizeof(rtext));
			res = f_read(&SDFile, rtext, sizeof(rtext), &bytesread);

			f_close(&SDFile);
		}
	}
	f_mount(&SDFatFS, (TCHAR const*)NULL, 0);
}
*/

