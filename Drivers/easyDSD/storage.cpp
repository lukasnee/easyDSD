/*
	SD card FAT32 storage wrapper for openDSD.

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

#include "storage.hpp"

extern uint8_t retSD;    /* Return value for SD */
extern char SDPath[4];   /* SD logical drive path */
extern FATFS SDFatFS;    /* File system object for SD logical drive */
extern FIL SDFile;       /* File object for SD */

#include "string.h"

#ifdef __cplusplus
extern "C" {
#endif

// all of your legacy C code here

#ifdef __cplusplus
}
#endif

#define FR_BEGIN FRESULT res = FR_OK;
#define FR_TRY(function) res = function; if (res != FR_OK ) {errorHandler(res); goto fr_end;}
#define FR_END fr_end: return (res == FR_OK) ? true : false;

#define FR_DO do {
#define FR_WHILE(function) res = function; } while(res == FR_OK)

void errorHandler(FRESULT r) {

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

	/* todo handle somehow */

}

bool SD::mount(void) {
	FR_BEGIN
	FR_TRY(f_mount(&SDFatFS, static_cast<TCHAR const*>(SDPath), 0));
	FR_END
}
bool SD::unmount(void) {
	FR_BEGIN
	FR_TRY(f_mount(&SDFatFS, static_cast<TCHAR const*>(NULL), 0));
	FR_END
}
bool SD::open(const char * path, FA_FLAGS mode)
{
	FR_BEGIN
	FR_TRY(f_open(&SDFile, path, mode));
	FR_END
}
bool SD::close(void) {
	FR_BEGIN
	FR_TRY(f_close(&SDFile));
	FR_END
}
/* buff[in] - pointer from where to write data */
bool SD::write(const void* buff, unsigned int bytesToWrite)
{
	FR_BEGIN
	FR_TRY(f_write(&SDFile, buff, bytesToWrite, &_bytesWritten));
	FR_END
}
/* buff[out] - pointer where to read in data */
bool SD::read(void* buff, unsigned int bytesToRead)
{
	FR_BEGIN
	FR_TRY(f_read(&SDFile, buff, bytesToRead, &_bytesRead));
	FR_END
}

bool SD::lseek(unsigned int offset)
{
	FR_BEGIN
	FR_TRY(f_lseek(&SDFile, offset));
	FR_END
}

unsigned long SD::tell(void)
{
	return f_tell(&SDFile);
}

char const * SD::getSDPath(void)
{
	return SDPath;
}

bool SD::findFirst(const char * path, const char * pattern)
{
	FR_BEGIN
	FR_TRY(f_findfirst(&_dj, &file._fno, path, pattern));
	FR_END
}

bool SD::findNext(void) {
	FR_BEGIN
	FR_TRY(f_findnext(&_dj, &file._fno));
	FR_END
}

//FRESULT SD::scanFiles (
//    char const * path        /* Start node to be scanned (***also used as work area***) */
//)
//{
//    FRESULT res;
//    DIR dir;
//    UINT i;
//    static FILINFO fno;
//    char strLine[50];
//
//	tft.setTextWrap(true);
//	tft.setBounds(_GRAMWIDTH, _GRAMHEIGH);
//	tft.setFont(&wucyFont8pt7b);
//	tft.setTextSize(1);
//	tft.setDrawColor(C_BLACK);
//	tft.fillRect(0, 0, _GRAMWIDTH, _GRAMHEIGH);
//	tft.setCursor(0, 0 + tft.getCharMaxHeight());
//	tft.setTextColor(C_LIME);
//
//    res = f_opendir(&dir, path);                       /* Open the directory */
//    if (res == FR_OK) {
//        for (;;) {
//            res = f_readdir(&dir, &fno);                   /* Read a directory item */
//            if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
//            if (fno.fattrib & AM_DIR) {                    /* It is a directory */
//                i = strlen(path);
//                sprintf(&path[i], "/%s", fno.fname);
//                res = scanFiles(path);                    /* Enter the directory */
//                if (res != FR_OK) break;
//                path[i] = 0;
//            } else {                                       /* It is a file. */
//                sprintf(strLine, "%s/%s\n", path, fno.fname);
//                tft.print(strLine);
//            }
//        }
//        f_closedir(&dir);
//    }
//
//    return res;
//}



//void fooo(void) {
//
//	FRESULT res;                                           FatFs function common result code
//	UINT byteswritten, bytesread;                      File write/read counts
//	uint8_t wtext[] = "Hello from bambukas :)";  File write buffer
//	uint8_t rtext[100];                                    File read buffer
//
//	if(f_mount(&SDFatFS, (TCHAR const*)SDPath, 0) == FR_OK)
//	{
//		HAL_Delay(200);
//
//		//Open file for writing (Create)
//		if(f_open(&SDFile, "F7FILE5.TXT", FA_CREATE_ALWAYS | FA_WRITE) == FR_OK) {
//
//			//Write to the text file
//			res = f_write(&SDFile, wtext, strlen((char *)wtext), &byteswritten);
//
//			f_close(&SDFile);
//
//			//Test read file
//			f_open(&SDFile, "F7FILE5.TXT",  FA_READ);
//			memset(rtext,0,sizeof(rtext));
//			res = f_read(&SDFile, rtext, sizeof(rtext), &bytesread);
//
//			f_close(&SDFile);
//		}
//	}
//	f_mount(&SDFatFS, (TCHAR const*)NULL, 0);
//}


