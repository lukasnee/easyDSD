/**
  ******************************************************************************
  * @file   fatfs.c
  * @brief  Code for fatfs applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

#include "fatfs.h"

uint8_t retSD;    /* Return value for SD */
char SDPath[4];   /* SD logical drive path */
FATFS SDFatFS;    /* File system object for SD logical drive */
FIL SDFile;       /* File object for SD */

/* USER CODE BEGIN Variables */
#include <string.h> //
FRESULT res;                                          /* FatFs function common result code */
uint32_t byteswritten, bytesread;                     /* File write/read counts */
uint8_t wtext[] = "Hello from Lukas :)"; /* File write buffer */
uint8_t rtext[100];                                   /* File read buffer */

/* USER CODE END Variables */    

void MX_FATFS_Init(void) 
{
  /*## FatFS: Link the SD driver ###########################*/
  retSD = FATFS_LinkDriver(&SD_Driver, SDPath);

  /* USER CODE BEGIN Init */
	if(f_mount(&SDFatFS, (TCHAR const*)SDPath, 0) == FR_OK)
	{
		HAL_Delay(200);

		//Open file for writing (Create)
		if(f_open(&SDFile, "F7FILE5.TXT", FA_CREATE_ALWAYS | FA_WRITE) == FR_OK) {

			//Write to the text file
			res = f_write(&SDFile, wtext, strlen((char *)wtext), (void *)&byteswritten);

			f_close(&SDFile);

			//Test read file
			f_open(&SDFile, "F7FILE5.TXT",  FA_READ);
			memset(rtext,0,sizeof(rtext));
			res = f_read(&SDFile, rtext, sizeof(rtext), (UINT*)&bytesread);

			f_close(&SDFile);
		}
	}


	f_mount(&SDFatFS, (TCHAR const*)NULL, 0);
  /* USER CODE END Init */
}

/**
  * @brief  Gets Time from RTC 
  * @param  None
  * @retval Time in DWORD
  */
DWORD get_fattime(void)
{
  /* USER CODE BEGIN get_fattime */
  return 0;
  /* USER CODE END get_fattime */  
}

/* USER CODE BEGIN Application */
     
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
