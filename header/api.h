#ifndef _api_H_
#define _api_H_

#include  "../header/halGPIO.h"     // private library - HAL layer


extern void printSWs2LEDs(void);
extern void printArr2SWs(char Arr[], int size, unsigned int rate);
extern void ServoUsScan(unsigned int * distArr,  unsigned int ServoDeg);
extern void SendVal2PC(unsigned int distArr[]);
extern void ObjectsDtcSys(void);
extern unsigned int conDeg2DutyCyc(int deg);
extern unsigned int ultraSonicMeasure(void);
extern void telemeter();
extern void lightSrcDtcSys(unsigned int distArr[][DIST_ARR_LENGTH]);
extern void ServoLdrScan(unsigned int distArr[][DIST_ARR_LENGTH],unsigned ServoDeg, unsigned char deg[]);
extern void LdrMeasure(unsigned int (*distArr)[DIST_ARR_LENGTH], unsigned index);
extern void sendLightDtcVals(unsigned char deg[DIST_ARR_LENGTH]);
extern unsigned int measureTelemDist(void);
extern void calibration();
extern void sendValDeg2PC(unsigned int val, unsigned int degIndex);
extern void incLcd(unsigned int x);
extern void decLcd(unsigned int x);
extern void decLcd(unsigned int x);
extern void rraLcd(unsigned char x);
extern unsigned int conDegToIndex(int deg);
extern void setDelay(unsigned int delay);
extern void telemeterScript(unsigned int);
extern void downloadScripts(void);
extern unsigned char getCharFromPC(void);
extern void scriptEx(void);
extern void decodeAndExe(unsigned char *data , unsigned int len);
extern void saveScript(void);
#endif







