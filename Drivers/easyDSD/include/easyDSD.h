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

void easy_dsd_start_task(void); /* start easyDSD externally from c source */

#ifdef __cplusplus
}
#endif

#endif /* EASYDSD_H_ */
