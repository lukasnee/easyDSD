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
#ifndef SD_HPP_
#define SD_HPP_

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#include "hwg.hpp"
#include "fatfs.h"	/* FAT32 file system */

typedef int FA_FLAGS;

class SD {

public:

	SD(void) : _bytesRead(0), _bytesWritten(0)
	{
		mount();
	};

	~SD(void) {};

	bool 	mount(void),
			unmount(void),
			open(const char * path,	FA_FLAGS mode),
			close(void),
			write(const void* buff, unsigned int bytesToWrite),
			read(void* buff, unsigned int bytesToRead),
			lseek(unsigned int offset),
			findFirst(const char * path, const char * pattern),
			findNext(void);

	unsigned long tell(void);

	char const * getSDPath(void);

	//bool scanFiles(char const * path);

	unsigned int getBytesRead(void) { return _bytesRead; };
	unsigned int getBytesWritten(void) { return _bytesWritten; };

	class File {

	public:
		unsigned long getSize(void) { return _fno.fsize; };
		unsigned short getDate(void) { return _fno.fdate; };
		unsigned short getTime(void) { return _fno.ftime; };
		unsigned char getAttributes(void) { return _fno.fattrib; };
		char * getAlternativeName(void) { return _fno.altname; };
		char * getName(void) { return _fno.fname; };

	private:

		friend class SD;
	    FILINFO _fno;    			/* File information */

	} file;

protected:

private:

    DIR _dj;         			/* Directory object */

	unsigned int _bytesRead;	/* Pointer to number of bytes read */
	unsigned int _bytesWritten;	/* Pointer to number of bytes written */
};

#endif  // SDs_HPP_
