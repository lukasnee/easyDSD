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

typedef enum SD_MODE_{
	SD_READ	= 0x01,
	SD_WRITE = 0x02,
	SD_OPEN_EXISTING = 0x00,
	SD_CREATE_NEW = 0x04,
	SD_CREATE_ALWAYS = 0x08,
	SD_OPEN_ALWAYS = 0x10,
	SD_OPEN_APPEND = 0x30,
}SD_MODE;

typedef enum SD_RESULT_{

	SD_OK,
	SD_DISK_ERR,
	SD_INT_ERR,
	SD_NOT_READY,
	SD_NO_FILE,
	SD_NO_PATH,
	SD_INVALID_NAME,
	SD_DENIED,
	SD_EXIST,
	SD_INVALID_OBJECT,
	SD_WRITE_PROTECTED,
	SD_INVALID_DRIVE,
	SD_NOT_ENABLED,
	SD_NO_FILESYSTEM,
	SD_MKFS_ABORTED,
	SD_TIMEOUT,
	SD_LOCKED,
	SD_NOT_ENOUGH_CORE,
	SD_TOO_MANY_OPEN_FILES,
	SD_INVALID_PARAMETER

}SD_RESULT;

class SD {

public:

	SD(void) : _bytesRead(0), _bytesWritten(0)
	{
		mount();
	};

	~SD(void) {};

	SD_RESULT 	mount(void),
				unmount(void),
				open(const char * path,	SD_MODE mode),
				close(void),
				write(const void* buff, unsigned int bytesToWrite),
				read(void* buff, unsigned int bytesToRead),
				lseek(unsigned int offset),
				getSDPath(char const * path);

	unsigned long tell(void);

				//scanFiles(char const * path);

	unsigned int getBytesRead(void) { return _bytesRead; };
	unsigned int getBytesWritten(void) { return _bytesWritten; };

protected:
private:

	unsigned int _bytesRead;	/* Pointer to number of bytes read */
	unsigned int _bytesWritten;	/* Pointer to number of bytes written */
};

#endif  // SDs_HPP_
