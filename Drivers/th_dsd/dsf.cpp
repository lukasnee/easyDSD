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

#include <dsf.hpp>

#include <cstring>

void dsf_readHeader(const uint8_t* header, uint32_t size) {

	dsf_t dsf;

	// copy dsd and fmt chunks
	memcpy(&dsf, (const uint8_t*)header, sizeof(dsd_chunk_t) + sizeof(fmt_chunk_t));
	// copy data chunk without actual sample data
	memcpy(&dsf.data, (const uint8_t*)header + sizeof(dsd_chunk_t) + sizeof(fmt_chunk_t), 12);
}
