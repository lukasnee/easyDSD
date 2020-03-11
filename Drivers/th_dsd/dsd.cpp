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

#include <dsd.hpp>
#include "wucyFont8pt7b.h"


#ifdef __cplusplus
extern "C" {
#endif

// all of your legacy C code here

#ifdef __cplusplus
}
#endif

//osThreadId defaultTaskHandle;

void th_dsd_start(void) {
//	osThreadDef(th_dsd, openDSD::th_dsd_task, osPriorityAboveNormal, 0, 3*1024);
//	defaultTaskHandle = osThreadCreate(osThread(th_dsd), NULL);
	openDSD::th_dsd_task(NULL);
}

void openDSD::th_dsd_task(void const * argument)
{
FR_BEGIN

	openDSD dsd;
	//Logger logger(&dsd, 0, 0, 128, 128);

	dsd.buttonsBegin();

	char c;
	char txt[500] = "mega wow";
	static uint16_t i = 0;
	UINT bw, br;

	dsd.printStylePipboy();
	dsd.tft.print("test is running...\n");

	dsd.buttonsUpdate();
	FR_TRY(dsd.sd.sd_open("F7FILE5.TXT", FA_CREATE_ALWAYS | FA_WRITE));

	//Write to the text file
	FR_TRY(dsd.sd.sd_write(txt, strlen(txt), bw));
	dsd.tft.print("wrote text to file\n");
	FR_TRY(dsd.sd.sd_close());

	//Test read file
	FR_TRY(dsd.sd.sd_open("geras.txt",  FA_READ));
	FR_TRY(dsd.sd.sd_read(txt, sizeof(txt), br));
	dsd.tft.print("reading:\n");
	dsd.tft.print(txt);
	FR_TRY(dsd.sd.sd_close());
	dsd.tft.updateScreen();
	//dsd.list("*.*");
	dsd.scanFiles("/");
	//logger.draw();


	while(true) {
	}
FR_END
}

FRESULT openDSD::scanFiles (
    char* path        /* Start node to be scanned (***also used as work area***) */
)
{
    FRESULT res;
    DIR dir;
    UINT i;
    static FILINFO fno;
    char strLine[50];

	tft.setTextWrap(true);
	tft.setBounds(_GRAMWIDTH, _GRAMHEIGH);
	tft.setFont(&wucyFont8pt7b);
	tft.setTextSize(1);
	tft.setDrawColor(C_BLACK);
	tft.fillRect(0, 0, _GRAMWIDTH, _GRAMHEIGH);
	tft.setCursor(0, 0 + tft.getCharMaxHeight());
	tft.setTextColor(C_LIME);

    res = f_opendir(&dir, path);                       /* Open the directory */
    if (res == FR_OK) {
        for (;;) {
            res = f_readdir(&dir, &fno);                   /* Read a directory item */
            if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
            if (fno.fattrib & AM_DIR) {                    /* It is a directory */
                i = strlen(path);
                sprintf(&path[i], "/%s", fno.fname);
                res = scanFiles(path);                    /* Enter the directory */
                if (res != FR_OK) break;
                path[i] = 0;
            } else {                                       /* It is a file. */
                sprintf(strLine, "%s/%s\n", path, fno.fname);
                tft.print(strLine);
            }
        }
        f_closedir(&dir);
    }

    return res;
}

/* @brief print style (default) good for logging...
 *
 * */
void openDSD::printStylePipboy(void) {

	tft.setFont(&wucyFont8pt7b);
	tft.setTextSize(1);
	tft.setTextColor(C_LIME);
	tft.setTextWrap(true);
	tft.setBounds(tft.width(), tft.height());
	tft.setCursor(0, tft.getCharMaxHeight());
}

//void openDSD::list(const char * pattern) {
//
//FR_BEGIN
//	char fileStr[100];
//	DIR dp = { 0 };
//	FILINFO fno = { 0 };
//	uint16_t index = 0, i_cursor = 0, i_end;
//
//	//FR_TRY(f_readdir(&dp,&fno));
//	//FR_TRY(f_findfirst(&dp, &fno, SDPath, pattern));
//
//	//FR_DO
////		sprintf(fileStr, "%.*s %lu \n", 30, fno.fname, fno.fsize);
////		setTextColor(index == i_cursor ? C_YELLOW : C_LIME);
////		print(fileStr);
////		index++;
//	//FR_WHILE(f_findnext(&dp, &fno));
//
//FR_END
//}


//dsd.buttonsUpdate();
//
//HAL_GPIO_WritePin(LED_D2_GPIO_Port, LED_D2_Pin,
//		(GPIO_PinState) (dsd.btn[BTN_OK].pressedFor(500) | dsd.btn[BTN_OK].wasPressed()));
//HAL_GPIO_WritePin(LED_D2_GPIO_Port, LED_D2_Pin,
//		(GPIO_PinState) (dsd.btn[BTN_OK].pressedFor(1000) | dsd.btn[BTN_OK].wasReleased()));



/*void* operator new(size_t size) {
	return pvPortMalloc(size);
}

void* operator new[](size_t size) {
	return pvPortMalloc(size);
}

void operator delete(void *ptr) {
	vPortFree(ptr);
}

void operator delete[](void *ptr) {
	vPortFree(ptr);
}*/

/*
	static void testText(void) {

		static char c = 0;

		tft.setFont(&wucyFont8pt7b);

		tft.setTextWrap(true);
		tft.setBounds(tft.width(), tft.height());

		//tft.fillScreen();

		tft.setTextColor(C_YELLOW);
		tft.setTextSize(2);
		tft.setCursor(0, tft.getCharMaxHeight());
		tft.println("openDSD\n");
		tft.setCursor(0, tft.getCharMaxHeight()*3);
		tft.setTextColor(C_LIME);
		tft.setTextSize(1);
		for(unsigned int i = c; i <= 4*0xFF; i++)
			tft.write((char)i);
		c++;
	}
*/

//
//		if (dsd.btn[BTN_UP].wasPressed()) {
//			i++;
//			sprintf(txt, "The number is: %d.\n", i);
//			sprintf(txt, "12345");
//			logger.log(txt);
//		}
//		if (dsd.btn[BTN_OK].wasPressed()) {
//
//			for(int a = 32; a < i; a++)
//				*(txt+a-32) = a;
//			*(txt+i) = '\0';
//			logger.log(txt);
//		}
//
//		if (dsd.btn[BTN_DOWN].wasPressed()) {
//			i--;
//			sprintf(txt, "ABCDEF");
//			//sprintf(txt, "The number is: %d.\n", i);
//			logger.log(txt);
//		}

/*
	do{
//		sprintf(txt, "This is line %d.\n", i);
//		logger.log(txt);

		c = 32 + rand() % 96;
		if(c == 127) c = '\n';
		logger.log(c);

		logger.draw();
		tft.updateScreen();
		//delay(100);
		i++;
	}while(i != 0);
*/

/* end of th_dsd.cpp */
