#ifndef _halGPIO_H_
#define _halGPIO_H_

#include  "../header/bsp.h"    		// private library - BSP layer
#include  "../header/app.h"    		// private library - APP layer

enum state5Mode{scriptNum, scriptData, exitCall}; // global variable



extern enum FSMstate state;   // global variable
extern enum SYSmode lpm_mode; // global variable
extern enum state5Mode scriptMode;
extern unsigned int INDEX;
extern unsigned int Cap1Val[2];
extern unsigned int ECHO_HL_TIME;
extern unsigned int DIST;
extern unsigned char LDR_FLAG;
extern unsigned int ADC_VAL;
extern unsigned int NUM_SAMPLES;
extern unsigned int UART_INDEX;
extern unsigned char ACK;
extern unsigned char EXIT;
extern unsigned char STR_TX;
extern char *STR_RX;
extern unsigned char TELEMETER_FLAG;
extern unsigned int DIST_LIGHT_ARR[2][DIST_ARR_LENGTH];
extern unsigned int NUM_OF_500ms;
extern unsigned int DELAY_RES;
extern unsigned int TIMER_CNT;
extern unsigned char CH;



// FLASH
#define FLASH_SEG_D 0x1000
#define FLASH_SEG_C 0x1040
#define FLASH_SEG_B 0x1080

//scripts global
typedef struct scriptStruct{
    char* startAdrr[3];
    unsigned int scriptLength[3];
    unsigned char  scriptName[3];
    unsigned int numOfFiles;
}scriptStruct;


extern char* getScriptSeg(int);



extern unsigned char DOWNLOAD_SCRIPT_FLAG;
extern unsigned char GET_NUM_OF_SCRIPT_FLAG;



extern void sysConfig(void);
extern void print2LEDs(unsigned char);
extern void clrLEDs(void);
extern void toggleLEDs(char);
extern void setLEDs(char);
extern unsigned char readSWs(void);
extern void delay(unsigned int);
extern void enterLPM(unsigned char);
extern void incLEDs(char);
extern void enable_interrupts();
extern void disable_interrupts();
extern void ObjectsDetectorConfig(unsigned int ServoDeg);
extern void lightSrcDetectorConfig(unsigned int ServoDeg);
extern __interrupt void PBs_handler(void);
extern void startTimerA1(void);
extern void stopTimerA1(void);
extern void startTimerA0(void);
extern void stopTimerA0(void);
extern void enable_TA0_interrupts();
extern void disable_TA0_interrupts();
extern unsigned int enable_ADC(unsigned int numOfSamples , unsigned int numOfShifts);
extern void disable_ADC();
extern void sendCharToPc(char ch);
extern void sendTerminator(char treminator);
extern void enableUartReceive(void);
extern void timerDelay(void);

void flashWrite(unsigned char* seg,unsigned char* data, unsigned int length);
extern void flashRead(char* start_address, unsigned char* arr, unsigned int len);
extern void flashErase(void);
extern void setFlashWRT(void);
extern void flashLock(void);
extern void clrFlashWrt(void);

#endif






extern void lcd_cmd(unsigned char);
extern void lcd_data(unsigned char);
extern void lcd_puts(const char * s);
extern void lcd_init();
extern void lcd_strobe();
extern void DelayMs(unsigned int);
extern void DelayUs(unsigned int);
extern void changeServoDegree(unsigned int ServoDeg);
extern void servoDelay(unsigned int loops);
/*
 *  Delay functions for HI-TECH C on the PIC18
 *
 *  Functions available:
 *      DelayUs(x)  Delay specified number of microseconds
 *      DelayMs(x)  Delay specified number of milliseconds
*/






