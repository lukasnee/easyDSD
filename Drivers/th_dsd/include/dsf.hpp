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

#ifndef TH_DSD_INCLUDE_DSF_HPP_
#define TH_DSD_INCLUDE_DSF_HPP_

#include <cstdio>

const uint8_t dsd_chunk_header[] = "DSD ";
const uint32_t sizeOfDsdChunk = 8;
const uint8_t fmt_chunk_header[] = "fmt ";

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

typedef enum channel_num{
	CHNUM_MONO = 1,
	CHNUM_STEREO,
	CHNUM_3,
	CHNUM_4,
	CHNUM_5,
	CHNUM_6
}ch_num_e;

typedef struct dsd_chunk{

	uint8_t header[4]; // must equate to 'D', 'S', 'D', ' ' (includes 1 space).
	uint64_t sizeOfThisChunk; // usually 28 bytes.
	uint64_t totalFileSize; // Total file size.
	uint8_t* pMetadata; // Pointer to Metadata chunk.

}dsd_chunk_t;

typedef struct fmt_chunk{

	uint8_t header[4]; // must equate to 'f', 'm', 't', ' ' (includes 1 space).
	uint64_t sizeOfThisChunk; // usually 52 bytes.
	uint32_t formatVersion; // Version of this file format. Should be 1.
	uint32_t formatId; // 0 : DSD raw.

	ch_type_t channelType; // ch_type_e
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

	ch_num_t channelNum; // ch_num_e
	uint32_t samplingFreq; // Sampling frequency in Hertz, ex.: 2822400, 5644800.
	uint32_t bitsPerSample; // in bits, ex.: 1, 8.

	uint64_t sampleCount;
	/* Sample count is the num per 1 channel.
		Ex) n second data: Sample count would be Sampling frequency * n */

	uint32_t blockSizePerChannel; // in bytes. ex.: 4096.
	/* Block size per channel is fixed, so please fill ZERO(0x00)
	 * for unused sample data area in the block. */

	uint8_t reserved; // usually filled with zeros.

}fmt_chunk_t;

typedef struct data_chunk{

	uint8_t header[4]; // must equate to 'd', 'a', 't', 'a'.
	uint64_t sizeOfThisChunk; // Equal to n(see next line) + 12.

	uint8_t* sampleData; // Data is set as LSB(Least Significant Bit) first.
/*	For example, if dsd stream data is 0x00, 0x01, 0x02, 0x03, 0x04….., then this is the sample data in data
	chunk.
	If “Bits per sample” is equal to 1, then store the data as LSB(Least Significant Bit) first.
	0x00, 0x80, 0x40, 0xC0, 0x20, ….
	(00000000, 10000000, 01000000, 11000000, 00100000, ….)
	If “Bits per sample” is equal to 8, then store the data as MSB(Most Significant Bit) first.
	0x00, 0x01, 0x02, 0x03, 0x04…..
	(00000000, 00000001, 00000010, 00000011, 00000100, ….)
*/

}data_chunk_t;

typedef struct metadata_chunk{

	uint8_t* metaData; // ID3v2

}metadata_chunk_t;

typedef struct dsf{

	dsd_chunk_t 		dsd;
	fmt_chunk_t 		fmt;
	data_chunk_t 		data;
	metadata_chunk_t 	metadata;

}dsf_t;

void dsf_readHeader(const uint8_t* header, uint32_t size);

#endif /* TH_DSD_INCLUDE_DSF_HPP_ */
