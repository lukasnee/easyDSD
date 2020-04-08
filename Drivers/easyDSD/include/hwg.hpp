/*
	Hardware API for project openDSD. Glue code for your specific hardware.
	Initially designed for STM32F407VE.

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

#ifndef DSD_HWG_H_
#define DSD_HWG_H_

#include "stdio.h"
#include "main.h"

#define TH_STM32

#ifdef __cplusplus
extern "C" {
#endif
	void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi);
	void HAL_I2S_TxHalfCpltCallback(I2S_HandleTypeDef *hi2s);
	void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef *hi2s);
#ifdef __cplusplus
}
#endif

typedef bool boolean;

#define HIGH ((boolean)true)
#define LOW ((boolean)false)

/* prototypes of API functions here */

/* for accessing STM32cubeMx generated handlers */

extern DMA_HandleTypeDef hdma_spi2_tx;
extern DMA_HandleTypeDef hdma_spi3_tx;
extern RTC_HandleTypeDef hrtc;
extern SD_HandleTypeDef hsd;
extern SPI_HandleTypeDef hspi1;
extern DMA_HandleTypeDef hdma_spi1_tx;

/*==============================================================*/
/*								SPI								*/
/*==============================================================*/

typedef struct tft_pinout_stm32_{

	GPIO_TypeDef* GPIOx;
	uint16_t GPIO_Pin;

}tft_pinout_stm32;

typedef enum tft_pinout_id_{

	TFT_PIN_CS,
	TFT_PIN_RESET,
	TFT_PIN_SDA,
	TFT_PIN_SCK,
	TFT_PIN_A0,

}tft_pinout_id;

typedef enum dc_mode_{
	DC_DATA, DC_COMMAND
}dc_mode_e;

class SPI {

public:

	SPI() {}
	void begin() {};
	int8_t transfer(uint8_t* data, dc_mode_e dc, uint32_t size = 1);

	/* dummy code. todo: add functionality to it if ever needed... */

private:
	static bool _dmaBusy;
	static SPI_HandleTypeDef* _hspi;

	friend void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi);
};

extern SPI spi;

/*==============================================================*/
/*								I2S								*/
/*==============================================================*/


extern I2S_HandleTypeDef hi2s2;
extern I2S_HandleTypeDef hi2s3;

/* Max buffer size in bytes for each channel. Should be matched
 * with SD card read block size for optimal performance.
 * NOTE. Defined size eventually will be doubled because of
 * ping-pong buffer stream method and multpilied for every channel.
 * See TOTAL_BUFFER_SIZE calculated expression for exact size.
 * */
#define I2S_MAX_BUF_SIZE 4096
/* 2 for stereo, etc. NOTE.
 * NOTE. currently only stereo supported !
 * */
#define I2S_MAX_CHANNELS 2

/* USER CONFIGURATIONS END */

#define I2S_PING_PONG 2
#define I2S_MAX_PINGPONG_BUF_SIZE (I2S_MAX_BUF_SIZE * I2S_PING_PONG)
#define I2S_PING_BUF_SIZE (I2S_MAX_PINGPONG_BUF_SIZE / 2)
#define I2S_PONG_BUF_SIZE (I2S_MAX_PINGPONG_BUF_SIZE / 2)

/* Total space used in RAM for DSD streaming. [in bytes] */
#define I2S_TOTAL_BUFFER_SIZE (EDSD_PINGPONG_BUF_SIZE * I2S_MAX_CHANNELS)

/* for buffer access */
typedef enum pingpong_{ PP_PING, PP_PONG }pingpong_e;
/* for buffer access, todo maybe more channels support ? */
typedef enum channel_{CH_LEFT, CH_RIGHT}channel_e;
/* DMA buffer (memory field must be available by hardware DMA)
 * NOTE. THIS TAKES A LOT OF RAM, 16kB default*/
typedef uint8_t buff_stream_3ar[I2S_PING_PONG][I2S_MAX_BUF_SIZE];

typedef enum dma_state_{

	I2S_STREAMING_PING,
	I2S_STREAMING_PONG,
	I2S_UNKNOWN

}i2s_state_e;

// Two channel DMA enabled hardware I2S
class I2S {
	/*todo make so that user can change block size
			so they would align, currently fixed max size is aligned.*/
public:

	class ChannelBuffer {
	public:
		uint8_t * getBuffer(void) { return &(_buffer[PP_PING][0]); }
		uint8_t * getBufferPing(void) { return &(_buffer[PP_PING][0]); }
		uint8_t * getBufferPong(void) { return &(_buffer[PP_PONG][0]); }
		uint16_t getSplitSize(void) { return sizeof(_buffer[PP_PING]); }
	private:
		buff_stream_3ar _buffer;
	}left, right;

	bool stopCircularDMA(void) {
		if(HAL_I2S_DMAStop(&hi2s2) == HAL_OK &&
			HAL_I2S_DMAStop(&hi2s3) == HAL_OK)
			return true;
		return false;
	}

	bool startCircularDMA(uint16_t channelBlockSize) {
		if(HAL_I2S_Transmit_DMA(&hi2s2, (uint16_t*)(left.getBuffer()), channelBlockSize) == HAL_OK &&
			HAL_I2S_Transmit_DMA(&hi2s3, (uint16_t*)(right.getBuffer()), channelBlockSize) == HAL_OK)
			return true;
		return false;
	}

	i2s_state_e getState(void) { return _state; }

private:

	static i2s_state_e _state;

	friend void HAL_I2S_TxHalfCpltCallback(I2S_HandleTypeDef *hi2s);
	friend void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef *hi2s);

};

extern I2S i2s;

/*==============================================================*/
/*							Arduino API							*/
/*==============================================================*/


#define pinMode(pin, mode)  /* todo: does nothing, for now its enough. */
#define millis() HAL_GetTick()
#define bitClear(byte, bit) byte &= ~(1 << bit)
#define bitSet(byte, bit) byte |= (1 << bit)
#define delay(ms) HAL_Delay(ms)

//typedef std::string String;
typedef char const * String;
typedef char const * __FlashStringHelper;

/* Arduino workaround to work with AdafruitGFX */

class Print {
public:
	virtual size_t write(unsigned char c) = 0;
	void printDebug(char * const buffer, uint32_t size);
	void print(unsigned char c);
	void print(String str);
	void println(String str = "\0");
	void print(const float value);
	void println(const float value);
};

void digitalWrite(uint8_t pin, boolean state);
boolean digitalRead(uint8_t pin);

/*==============================================================*/
/*							DEBUG								*/
/*==============================================================*/

class DebugSignal {

	const tft_pinout_stm32 debug_stm32_pinmap[4] = {
		{DEBUG_0_GPIO_Port, DEBUG_0_Pin},
		{DEBUG_1_GPIO_Port, DEBUG_1_Pin},
		{DEBUG_2_GPIO_Port, DEBUG_2_Pin},
		{DEBUG_3_GPIO_Port, DEBUG_3_Pin}
	};

public:

	inline void resetAll(void) {
		for(int pin = 0; pin < 4; pin++)
			HAL_GPIO_WritePin(
				debug_stm32_pinmap[pin].GPIOx,
				debug_stm32_pinmap[pin].GPIO_Pin,
				GPIO_PIN_RESET);
	};
	inline void reset(uint8_t pin) {
		HAL_GPIO_WritePin(
				debug_stm32_pinmap[pin].GPIOx,
				debug_stm32_pinmap[pin].GPIO_Pin,
				GPIO_PIN_RESET);
	};
	inline void set(uint8_t pin) {
		HAL_GPIO_WritePin(
				debug_stm32_pinmap[pin].GPIOx,
				debug_stm32_pinmap[pin].GPIO_Pin,
				GPIO_PIN_SET);
	};
	inline void toggle(uint8_t pin) {
		HAL_GPIO_TogglePin(
				debug_stm32_pinmap[pin].GPIOx,
				debug_stm32_pinmap[pin].GPIO_Pin);
	};
};

extern DebugSignal DEBUG_SIG;

#endif  // DSD_HWG_H_
