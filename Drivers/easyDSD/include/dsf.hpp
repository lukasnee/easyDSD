/*
	DSD Stream File [.dsf] reader from SDcard.

	Reference: DSF File Format Specification v1.01. (11, Nov, 2005) by Sony Corporation.

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

#ifndef EASYDSD_INCLUDE_DSF_HPP_
#define EASYDSD_INCLUDE_DSF_HPP_

#include <cstdio>

const uint8_t chunk_header_dsd[] = {'D', 'S', 'D', ' '};
const uint8_t chunk_header_fmt[] = {'f', 'm', 't', ' '};
const uint8_t chunk_header_data[] = {'d', 'a', 't', 'a'};

#define DSF_DSD_CHUNK_SIZE 28
#define DSF_FMT_CHUNK_SIZE 52
#define DSF_FMT_FORMAT_VERSION_1 1
#define DSF_FMT_FORMAT_ID_RAW 0

typedef uint32_t ch_type_t;

typedef enum channel_type{
	CHT_MONO = 1,
	CHT_STEREO,
	CHT_3_CHANNELS,
	CHT_QUAD,
	CHT_4_CHANNELS,
	CHT_5_CHANNELS,
	CHT_5_1_CHANNELS
}ch_type_e;


typedef uint32_t ch_num_t;
typedef uint64_t dsf_ptr_t;

typedef enum channel_num{
	CHNUM_MONO = 1,
	CHNUM_STEREO,
	CHNUM_3,
	CHNUM_4,
	CHNUM_5,
	CHNUM_6
}ch_num_e;

typedef struct dsf{

/* dsd header data */

	uint64_t totalFileSize; // Total file size.
	dsf_ptr_t pMetadata; // Pointer to Metadata chunk.

/* fmt header data */

	uint32_t formatVersion; // Version of this file format. Should be 1.
	uint32_t formatId; // 0 : DSD raw.

	ch_type_e channelType; // ch_type_e
	/* 	2-CHANNELS STEREO
			1 : Front Left 2 : Front Right
		3-CHANNELS
			1 : Front Left 2 : Front Right 3 : Center
		QUAD
			1 : Front Left 2 : Front Right 3 : Back Left 4 : Back Right
		4-CHANNELS
			1 : Front Left 2 : Front Right 3 : Center 4 : Low Frequency
		5-CHANNELS
			1 : Front Left 2 : Front Right 3 : Center 4 : Back Left 5 : Back Right
		5.1-CHANNELS
			1 : Front Left 2 : Front Right 3 : Center 4 : Low Frequency 5 : Back Left 6 : Back Right
	*/

	ch_num_e channelNum; // ch_num_e
	uint32_t samplingFreq; // Sampling frequency in Hertz, ex.: 2822400, 5644800.
	uint32_t bitsPerSample; // in bits, ex.: 1, 8.

	uint64_t sampleCount;
	/* Sample count is the num per 1 channel.
		Ex) n second data: Sample count would be Sampling frequency * n */

	uint32_t blockSizePerChannel; // in bytes. ex.: 4096.
	/* Block size per channel is fixed, so please fill ZERO(0x00)
	 * for unused sample data area in the block. */

/* data header data */

	uint64_t sampleDataSize; // Equal to n(see next line) + 12.
	dsf_ptr_t pSampleData; // Data is set as LSB(Least Significant Bit) first.
/*	For example, if dsd stream data is 0x00, 0x01, 0x02, 0x03, 0x04….., then this is the sample data in data
	chunk.
	If “Bits per sample” is equal to 1, then store the data as LSB(Least Significant Bit) first.
	0x00, 0x80, 0x40, 0xC0, 0x20, ….
	(00000000, 10000000, 01000000, 11000000, 00100000, ….)
	If “Bits per sample” is equal to 8, then store the data as MSB(Most Significant Bit) first.
	0x00, 0x01, 0x02, 0x03, 0x04…..
	(00000000, 00000001, 00000010, 00000011, 00000100, ….)
*/

}dsf_t;

int8_t dsf_readHeader(const uint8_t* dsfBinaryBuff, dsf_t * pDsf);

#endif /* EASYDSD_INCLUDE_DSF_HPP_ */
