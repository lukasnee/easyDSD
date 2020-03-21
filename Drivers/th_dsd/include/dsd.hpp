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

#ifndef DSD_HPP_
#define DSD_HPP_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <string.h>
//#include <string>
#include <stdio.h>

#include "fatfs.h"			/* FAT32 file system */
//#include <tag.h>			/* library for ID3v2 decoding from .dsf file */

#include "sound.h"

#ifdef __cplusplus
}
#endif


#define WUCY_OS

#include "openDSD.h"
#include <hwg.hpp>			/* hardware glue code for user to fill */
#include <Adafruit_GFX.h>
#include <TFT_ILI9163C.h> 	/* display driver */
#include "JC_Button.h" 		/* library for GPIO hardware buttons (debounce, etc.)*/
#include <log.hpp>
#include "storage.hpp"
#include "dsf.hpp"

extern const GFXfont wucyFont8pt7b;



typedef enum button_map_{

	BTN_UP,
	BTN_OK,
	BTN_DOWN,

BTN_COUNT} button_map;

class easyDSD {

public:

	TFT_ILI9163C tft;
	Storage sd;

	easyDSD() : tft(TFT_PIN_CS, TFT_PIN_A0, TFT_PIN_RESET) {
		tft.begin();
		sd.mount();
	}

	FRESULT scanFiles(char* path);
	//void list(const char * pattern);

	static void th_dsd_task(void const * argument);

	class Keyboard{

		void UpdateAll(void)
		{
			for(uint8_t i = 0; i < BTN_COUNT; i++)
				btn[i].read();
		}

		void BeginAll(void)
		{
			for(uint8_t i = 0; i < BTN_COUNT; i++)
				btn[i].begin();
		}

		button_map WasPressed(void)
		{
			buttonsUpdate();
			for(uint8_t i = 0; i < BTN_COUNT; i++) {
				if (btn[i].wasPressed())
					return static_cast<button_map>(i); // wasPressed
			}
			return BTN_COUNT; // none presssed
		}

		Button btn[BTN_COUNT] = {
				{BTN_UP_GPIO_Port, BTN_UP_Pin},
				{BTN_OK_GPIO_Port, BTN_OK_Pin},
				{BTN_DOWN_GPIO_Port, BTN_DOWN_Pin}
		};

	} keyboard;

	friend void th_dsd_start(void);


	void printStylePipboy(void);

	class Player {

		typedef enum ping_pong_{

			// Stop state
			PP_STANDBY,			// PingPong buffer totally in standby mode.

			// Starting state
			PP_PI_READ_PO_STANDBY,  // starting to fill Ping buffer, no streaming at this state.

			// Active states - continuous dataflow. Two flipping states.
			PP_PI_STREAM_PO_READ, 	// Ping start to be streamed, while Pong reads in data.
			PP_PI_READ_PO_STREAM,	// Process flips: Ping reads data in, while Pong streams.

			// Stopping states - which of two depends on the active bi-state.
			PP_PI_HALT_PO_STREAM,	// this if stopped on PP_PI_READ_PO_STREAM active state.
			PP_PI_STREAM_PO_HALT,	// this if stopped on PP_PI_STREAM_PO_READ active state.

		}ping_pong_e;


		// Data strem management class for circular PDM(DSD) bit stream to DAC.

		class Stream{

			public:

				Stream(void) : _state(PP_STANDBY), _statePrev(PP_STANDBY) {};

				ping_pong_e GetState(void) { return _state; };
				void SetState(ping_pong_e state) { _state = state; };
				void FlipActiveState(void) { _state = ((_state == PP_PI_READ_PO_STREAM) ? PP_PI_STREAM_PO_READ : PP_PI_READ_PO_STREAM); };
				bool StateHasChanged(void) {
					if(_state != _statePrev) {
						_statePrev = _state;
						return true;
					}
					return false;
				};
				bool IsStandby(void) { return _state == PP_STANDBY; };
				bool IsStarting(void) { return _state == PP_PI_READ_PO_STANDBY; };
				bool IsActive(void) { return ((_state == PP_PI_STREAM_PO_READ) || (_state == PP_PI_READ_PO_STREAM)); };
				bool PingIsStreamingPongIsReading(void) { return _state == PP_PI_STREAM_PO_READ; };
				bool PingIsReadingPongIsStreaming(void) { return _state == PP_PI_READ_PO_STREAM; };
				bool IsStopping(void) { return ((_state == PP_PI_HALT_PO_STREAM) || (_state == PP_PI_STREAM_PO_HALT)); };



				void AlertPingBuffFinishedStreaming(void) { _state = PP_PI_READ_PO_STREAM; };
				void AlertPongBuffFinishedStreaming(void) { _state = PP_PI_STREAM_PO_READ; };

			private:
				ping_pong_e _state;
				ping_pong_e _statePrev;

			} stream;


	public:

		Player() : _state(P_STOPPED), _trackIsActive(false) {  };

		typedef enum p_state_{

			P_STOPPED,

			P_STARTING_TO_PLAY,
			P_PLAYING,

			P_PAUSING,
			P_PAUSED,

			P_RESUMING,
			P_STOPPING

		}p_state_e;

		void Play(void)
		{
			if(!_trackIsActive && GetPlayerState() == P_STOPPED)
				SetPlayerState(P_STARTING_TO_PLAY);
		};
		void Stop(void)
		{
			if(_trackIsActive && GetPlayerState() == P_PLAYING)
				SetPlayerState(P_STOPPING);
		};
		void Pause(void)
		{
			if(_trackIsActive && GetPlayerState() == P_PLAYING)
				SetPlayerState(P_PAUSING);
		};
		void Resume(void)
		{
			if(_trackIsActive && GetPlayerState() == P_PAUSED)
				SetPlayerState(P_RESUMING);
		};

		void stream_task() {

			switch(GetPlayerState()) {

			case P_STOPPED:

				break;

			case P_STARTING_TO_PLAY:
				/* todo open file */
				_trackIsActive = true;
				break;

			case P_PLAYING:

				break;

			case P_PAUSING:

				break;

			case P_PAUSED:

				break;

			case P_RESUMING:

				break;

			case P_STOPPING:
				/* todo close file, stop DMA */
				_trackIsActive = false;

				break;

			}



		}

		void UpdateState(void) {

			if(stream.IsStandby()) {

				if(GetPlayerState() == P_STOPPING)
					SetPlayerState(P_STOPPED);
				else if (GetPlayerState() == P_PAUSING)
					SetPlayerState(P_PAUSED);
			}
			if(stream.IsActive()) {

				if(GetPlayerState() == P_STARTING_TO_PLAY ||
						GetPlayerState() == P_RESUMING)
					SetPlayerState(P_PLAYING);
			}
		}

		p_state_e GetPlayerState(void)
		{
			UpdateState();
			return _state;
		};

	private:

		p_state_e _state;
		bool _trackIsActive;


		void SetPlayerState(p_state_e state) { _state = state; };
	};
};


extern TFT_ILI9163C tft;
extern easyDSD dsd;


#endif  // DSD_HPP_
