/*
	DSD Stream File [.dsf] reader from SDcard.

	Reference: DSF File Format Specification v1.01. by Sony Corporation.

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

#include "dsf.hpp"

#include <cstring>

#define PARSE_START(p_head) { \
	uint8_t* pHead = (uint8_t*)p_head; \
	uint8_t* pMem = pHead

#define PARSE_P_HEAD pHead
#define PARSE_P_MEM pMem
#define PARSE_POSITION static_cast<dsf_ptr_t>(reinterpret_cast<int>(pMem - pHead))

/* little endian */
#define PARSE_READ(dst, type) dst = *(type*)pMem; pMem+=sizeof(type)
#define PARSE_READ_BUFF(pDestination, length) \
	memcpy(pDestination, pMem, length); pMem+=length

#define PARSE_END_BR(bytesRead) bytesRead = PARSE_POSITION;}
#define PARSE_END() }

#define VALIDATE(expected, type) \
	if(*(type*)pMem != expected) return -1;

#define VALIDATE_PASS(expected, type) \
	VALIDATE(expected, type) \
	pMem+=sizeof(type)

#define VALIDATE_BETWEEN(value, from, to) \
	if(value < from || value > to) return -1;

#define HEADER_SIZE 4U
#define HEADER_EQUAL(actual, expected) 	\
		(memcmp(actual, expected, HEADER_SIZE) == 0 ? 1 : 0)
#define PARSE_PASS(type) pMem+=sizeof(type)

int8_t dsf_readHeader(const uint8_t* dsfBinaryBuff, dsf_t * pDsf) {

	uint8_t header_tmp[HEADER_SIZE];
	dsf_t dsf = { 0 };

	PARSE_START(dsfBinaryBuff);

	/* read dsd chunk */

	PARSE_READ_BUFF(header_tmp, HEADER_SIZE);
	if(!HEADER_EQUAL(header_tmp, chunk_header_dsd)) return -1;

	VALIDATE_PASS(DSF_DSD_CHUNK_SIZE, uint64_t);

	PARSE_READ(dsf.totalFileSize, uint64_t);
	PARSE_READ(dsf.pMetadata, dsf_ptr_t);

	/* read fmt header */

	PARSE_READ_BUFF(header_tmp, HEADER_SIZE);
	if(!HEADER_EQUAL(header_tmp, chunk_header_fmt)) return -1;

	VALIDATE_PASS(DSF_FMT_CHUNK_SIZE, uint64_t); // why check const ? because stupid standard

	VALIDATE(DSF_FMT_FORMAT_VERSION_1, uint32_t); // version 1 whatever that means
	PARSE_READ(dsf.formatVersion, uint32_t);

	VALIDATE(DSF_FMT_FORMAT_ID_RAW, uint32_t); // raw - the one and only, lol
	PARSE_READ(dsf.formatId, uint32_t);

	PARSE_READ(dsf.channelType,ch_type_e);
	VALIDATE_BETWEEN(dsf.channelType, CHT_MONO, CHT_5_1_CHANNELS);

	PARSE_READ(dsf.channelNum, ch_num_e);
	VALIDATE_BETWEEN(dsf.channelNum, CHNUM_MONO, CHNUM_6);

	PARSE_READ(dsf.samplingFreq, uint32_t);
	PARSE_READ(dsf.bitsPerSample, uint32_t);
	PARSE_READ(dsf.sampleCount, uint64_t);
	PARSE_READ(dsf.blockSizePerChannel, uint32_t);
	PARSE_PASS(uint32_t); // skip reserved

	/* read data header */

	PARSE_READ_BUFF(header_tmp, HEADER_SIZE);
	if(!HEADER_EQUAL(header_tmp, chunk_header_data)) return -1;

	PARSE_READ(dsf.sampleDataSize, uint64_t);
	dsf.pSampleData = PARSE_POSITION;

	PARSE_END();

	*pDsf = dsf;
	return 1;

	}
