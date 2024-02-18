#include  "../header/api.h"    		// private library - API layer
#include  "../header/halGPIO.h"     // private library - HAL layer
#include <stdio.h>
#include <stdlib.h>
//-------------------------------------------------------------
//            Print SWs value onto LEDs
//-------------------------------------------------------------
void printSWs2LEDs(void){
	unsigned char ch;
	
	ch = readSWs();
	print2LEDs(ch);
}
//--------------------------------------------------------------------
//            Print array to LEDs array with rate of LEDs_SHOW_RATE
//--------------------------------------------------------------------            
void printArr2SWs(char Arr[], int size, unsigned int rate){
	unsigned int i;
	
	for(i=0; i<size; i++){
		print2LEDs(Arr[i]);
		delay(rate);
	}
}

void uint2Str(char * str, unsigned int num, unsigned length){
    unsigned int i;
    str[length] = 0;
    for (i = length ; i ; i--,num = num/10){
        str[i-1] = num%10 + 48;
    }
}

unsigned int numOfDigits(unsigned int num){
    unsigned int temp,numLength=0;
    for  (temp =num ; temp ; temp = temp/10)
        numLength++;
    return (num)?  numLength : 1;
}

void SendVal2PC(unsigned int distArr[]){
    unsigned int i,cnt=0,index;
    unsigned int reference;
    for (i=DIST_ARR_LENGTH ; i; i--){
        if ((distArr[i-1] > 115) && (distArr[i-1] < 2900)){
            reference = distArr[i-1];
            cnt = 0;
            while((i-1) && distArr[i-1] < reference + 150){
                i--;
                cnt++;
            }
            index = i + (cnt>>1);
            sendValDeg2PC(distArr[index], index);
        }
    }
    sendValDeg2PC(0,0);

}


            /*else{

            }
            if (!scanFlag){
                cnt++;
                if ()
            }
        }
    }
    for (i=DIST_ARR_LENGTH ; i; i--){
        if ((distArr[i-1] > 115) && (distArr[i-1] < 2900)){
            reference = distArr[i-1];
            cnt = 0;
            while((i-1) && distArr[i-1] < reference + 100  && cnt < 10){
                i--;
                cnt++;
            }
            index = i + (cnt>>1);
            if (cnt > 8 && cnt < 3){
                sendValDeg2PC(distArr[index], index);

            }
        }
    }
    //sendValDeg2PC(0,0);

}*/

void sendValDeg2PC(unsigned int val, unsigned int degIndex){
    sendCharToPc((char)(val >> 8));
    sendCharToPc((char)val);
    sendCharToPc((char)degIndex);
    enableUartReceive();
    enterLPM(lpm_mode);
}

void ObjectsDtcSys(void){
    unsigned int distArr [DIST_ARR_LENGTH];
    unsigned int ServoDeg = SERVO_INIT_DEG;
    //MS_SERVO_DELAY = 0x1063;
    //ObjectsDetectorConfig(ServoDeg);
    ServoMotorConfig(ServoDeg);
    servoDelay(20);
    ServoUsScan(distArr, ServoDeg);
    SendVal2PC(distArr);
    state = state0;
}

unsigned int conDeg2DutyCyc(int deg){
   unsigned int dutyCycle = SERVO_INIT_DEG;
   unsigned int i;
   unsigned int j = 14;
   for (i=0; i<deg ; i++){
       dutyCycle += 11;
       if (!j){
           dutyCycle++;
           j = 14;
       }
       j--;
   }
   return dutyCycle;
}

unsigned int ultraSonicMeasure(void){
    UltSonTimerA1config();
    enterLPM(lpm_mode);
    stopTimerA1();
    return ECHO_HL_TIME;
}

void ServoUsScan(unsigned int * distArr,  unsigned int ServoDeg){
    unsigned int i=0;
    changeServoDegree(SERVO_INIT_DEG);
    while(i<DIST_ARR_LENGTH){
        distArr[i] = ultraSonicMeasure();
        ServoDeg += DEG_DELTA;
        changeServoDegree(ServoDeg);
        i++;
    }
}


void servoUsLdrScan(unsigned int * distArrUS,unsigned int distArrLDR[][DIST_ARR_LENGTH], unsigned char *deg){
    unsigned int i=0;
    unsigned int LDR_VAL_50CM = 1000;
    unsigned int diff,ServoDeg = SERVO_INIT_DEG;
    int delta = 60;
    unsigned int ldrVal1,ldrVal2;

    changeServoDegree(ServoDeg);
    while(i<DIST_ARR_LENGTH){
        distArrUS[i] = ultraSonicMeasure();
        LdrMeasure(distArrLDR, i);

        ldrVal1 = distArrLDR[0][i];
        ldrVal2 = distArrLDR[1][i];
        if (ldrVal1 <= LDR_VAL_50CM && ldrVal2 < LDR_VAL_50CM){
            diff = (ldrVal1 < ldrVal2) ? ldrVal2-ldrVal1 : ldrVal1 - ldrVal2;
            deg[i] = (diff < delta) ? 1 : 0;
            deg[i]=1;
        }
        ServoDeg += DEG_DELTA;
        changeServoDegree(ServoDeg);
        i++;
    }
}

void getStrFromPC(char * str){
    UART_INDEX = 0;
    EXIT = 0x0;
    STR_RX = str;
    while (!EXIT){
        enableUartReceive();
        enterLPM(lpm_mode);
    }
}

void telemeter(){
    char strDeg [4];
    TELEMETER_FLAG = 0x0;
    getStrFromPC(strDeg);
    TELEMETER_FLAG = 0x1;
    int deg = atoi(strDeg);
    unsigned int dutyCycDeg = conDeg2DutyCyc(deg);
    unsigned int dist;
    ServoMotorConfig(dutyCycDeg);
    servoDelay(20);
    stopTimerA0();
    enableUartReceive();
    while (state == state2){
        UART_INDEX = 0;
        dist = measureTelemDist();
        sendCharToPc((char)(dist >> 8));
        sendCharToPc((char)(dist));
        DelayHalfSecConfig();
        enterLPM(lpm_mode);
        stopTimerA0();
    }
}

unsigned int measureTelemDist(void){
    INDEX = 0;
    UltSonTimerA1config();
    enterLPM(lpm_mode);
    stopTimerA1();
    return ECHO_HL_TIME;
}

void scanAllObjects(){
    unsigned int distArrUS [DIST_ARR_LENGTH]= {0};
    unsigned char degLdr[DIST_ARR_LENGTH]= {0};
    unsigned int ServoDeg = SERVO_INIT_DEG;

    ServoMotorConfig(ServoDeg);
    servoDelay(20);
    servoUsLdrScan(distArrUS,DIST_LIGHT_ARR,degLdr);

    SendVal2PC(distArrUS);
    sendLightDtcVals(degLdr);

    state = state0;
}


void lightSrcDtcSys(unsigned int distArr[][DIST_ARR_LENGTH]){
    MS_SERVO_DELAY = 0x1063;
    unsigned int ServoDeg = SERVO_INIT_DEG;
    unsigned char deg[DIST_ARR_LENGTH] = {0};
    LDR_FLAG = 0x0;
    lightSrcDetectorConfig(ServoDeg);
    ServoLdrScan(distArr, ServoDeg, deg);
    stopTimerA0();
    sendLightDtcVals(deg);
    state = state0;
}

void ServoLdrScan(unsigned int distArr[][DIST_ARR_LENGTH],unsigned ServoDeg, unsigned char deg[]){
    unsigned int i=0;
    unsigned int ldrVal1,ldrVal2;
    unsigned int diff;
    int delta = 30;
    unsigned int LDR_VAL_50CM = 1000;

    ServoDeg = SERVO_INIT_DEG;
    while(i<DIST_ARR_LENGTH){
        changeServoDegree(ServoDeg);
        LdrMeasure(distArr, i);
        ldrVal1 = distArr[0][i];
        ldrVal2 = distArr[1][i];
        if (ldrVal1 <= LDR_VAL_50CM && ldrVal2 < LDR_VAL_50CM){
            diff = (ldrVal1 < ldrVal2) ? ldrVal2-ldrVal1 : ldrVal1 - ldrVal2;
            deg[i] = (diff < delta) ? 1 : 0;
                deg[i]=1;
        }
        else
            deg[i]=0;
        ServoDeg += DEG_DELTA;
        i++;
    }
}

void sendLightDtcVals(unsigned char deg[DIST_ARR_LENGTH]){
    unsigned int i=0, cnt,val,index;
    for (i = 0 ; i< DIST_ARR_LENGTH ; i++){
        cnt = 0;
        while(i < DIST_ARR_LENGTH && (deg[i])){
            deg[i++] = 0;
            cnt ++;
        }
        if (cnt > 1){
            index = i-1-(cnt>>1);
            deg[index] = 1;
            val = (DIST_LIGHT_ARR[0][index] + DIST_LIGHT_ARR[1][index])>>1;
            sendValDeg2PC(val,index);
        }
    }
    sendValDeg2PC(0,0);

}
void LdrMeasure(unsigned int (*distArr)[DIST_ARR_LENGTH], unsigned index){
    unsigned int i;
    DelayMs(15);
    for( i = 0 ; i < 2 ; i++ ){
        distArr[i][index] = enable_ADC(32,5);
    }
}

LdrCalibMeasure(unsigned int index){
    unsigned int i,j;
    unsigned int avg[2] = {0};
    unsigned int avgBoth;
    for (i = 32 ; i ; i--){
        for (j = 0 ; j<2 ; j++){
            DelayMs(15);
            enable_ADC(32, 5);
            disable_ADC();
            avg[j] += ADC_VAL;
        }
    }
    avg[0] = avg[0]>>5;
    avg[1] = avg[1]>>5;
    avgBoth = (avg[0] + avg[1])>>1 ;
}

void calibration(void){
    unsigned int centerDeg = 90;
    unsigned int i;
    unsigned int distArr[2]={0};
    //MS_SERVO_DELAY = 0x6300;
    unsigned int dutyCycDeg = conDeg2DutyCyc(centerDeg);
    ServoMotorConfig(dutyCycDeg);
    servoDelay(30);
    //changeServoDegree(dutyCycDeg);
    for ( ; ;){
        unsigned int i;
        DelayMs(15);
        for( i = 0 ; i < 2 ; i++ ){
            distArr[i] = enable_ADC(32,5);
        }
    }
 }

//************************************************************
//                  script mode
//************************************************************
char* SEG_ADDR[3] = { 0x1000, 0x1040, 0x1080 };
//scriptStruct* SCRIPT_VAR;
//scripts global
scriptStruct SCRIPT_VAR;
void saveScript(void){
    //unsigned int scriptToEX;
    unsigned char scrName;
    unsigned int i = 0;
    unsigned char data[64] = {0};

    //get script number from user
    scriptMode = scriptNum;
    scrName = getCharFromPC(); //will initialize to one of 1,2,3 as char

    unsigned int scriptIndex = scrName-'0'-1;
    if(SCRIPT_VAR.scriptName[scriptIndex] != scrName)
           SCRIPT_VAR.numOfFiles++;
    SCRIPT_VAR.scriptName[scriptIndex] = scrName;

    //get data of script from pc
    EXIT = 0x0;
    scriptMode = scriptData;
    while (!EXIT){
        enterLPM(mode0);
        data[i++] = CH;
    }data[i-1] = 0;
    SCRIPT_VAR.scriptLength[scriptIndex] = i-1;

    if (scriptIndex == 0)
        SCRIPT_VAR.startAdrr[scriptIndex] = (char*) 0x1000;
    else if (scriptIndex == 1)
        SCRIPT_VAR.startAdrr[scriptIndex] = (char*) 0x1040;
    else
        SCRIPT_VAR.startAdrr[scriptIndex] = (char*) 0x1080;
    //SCRIPT_VAR.startAdrr[scriptIndex] = (char*)SEG_ADDR[scriptIndex];

    // download all scripts to flash
    flashWrite(SCRIPT_VAR.startAdrr[scriptIndex], data, i);

    //send ACK
    sendCharToPc('1');

    state = state0;
}


void scriptEx(void){
    unsigned char scrName;
    unsigned int i = 0;
    unsigned char data[64] = {0};

    //get script number from user
    scriptMode = scriptNum;
    scrName = getCharFromPC(); //will initialize to one of 1,2,3 as char
    unsigned int scriptIndex = scrName-'0'-1;

    flashRead(SCRIPT_VAR.startAdrr[scriptIndex], data, SCRIPT_VAR.scriptLength[scriptIndex]);
    decodeAndExe(data, SCRIPT_VAR.scriptLength[scriptIndex]);
}

unsigned int hexCharToInt(unsigned char hexChar) {
    if ('0' <= hexChar && hexChar <= '9') {
        return hexChar - '0'; // For digits '0' to '9'
    }
    else {
        return hexChar - 'A' + 10; // For uppercase letters 'A' to 'F'
    }
}

void decodeAndExe(unsigned char *data , unsigned int len){
    unsigned char opcode= 0;
    unsigned int operand1, operand2;
    unsigned int i = 1;

    while (i < len){
        //ignore zeros before opcode
        opcode = data[i++];
        if (opcode == '1' || opcode == '2' || opcode == '3' || opcode == '4' || opcode == '6' ){
            operand1 = hexCharToInt(data[i++])<<4 ;
            operand1 +=hexCharToInt(data[i++]);
//            operand1 = ((unsigned int) data[i++]);
            if (opcode == '1'){
                sendCharToPc('1');
                incLcd(operand1);
            }
            else if (opcode == '2'){
                sendCharToPc('2');
                decLcd(operand1);
            }
            else if (opcode == '3'){
                sendCharToPc('3');
                rraLcd(operand1);
            }
            else if (opcode == '4'){
                sendCharToPc('4');
                setDelay((unsigned int)operand1*10);
            }
            else if (opcode == '6'){
                sendCharToPc('6');
                DelayMs(15);
                telemeterScript(operand1);

            }
        }
        else if(opcode == '7'){
             sendCharToPc('7');
            operand1 = hexCharToInt(data[i++])<<4;
            operand1 +=hexCharToInt(data[i++]);
            operand2 = hexCharToInt(data[i++])<<4;
            operand2 += hexCharToInt(data[i++]);
            ScanBetweenAngles();
        }
        else if(opcode == '5'){
            sendCharToPc('5');
            lcd_clear();
            DelayMs(15);
        }
        else if (opcode == '8'){
            sendCharToPc('8');
            enterLPM(mode0);
        }
        i++;

    }

    sendCharToPc('\n');

}

void telemeterScript(unsigned int deg){
    unsigned int dutyCycDeg = conDeg2DutyCyc(deg);
    unsigned int dist;
    ServoMotorConfig(dutyCycDeg);
    servoDelay(20);
    stopTimerA0();
    enableUartReceive();
    UART_INDEX = 0;

    dist = measureTelemDist();
    sendCharToPc((char)(dist >> 8));
    sendCharToPc((char)(dist));
    DelayHalfSecConfig();
    enterLPM(mode0);
    stopTimerA0();
}



unsigned char getCharFromPC(void){
    enableUartReceive();
    enterLPM(lpm_mode);
    return CH;
}





unsigned int moduloHalfSec(unsigned int delay){
    unsigned int cnt = 0;
    while (delay > 500){
        delay -= 500;
        cnt++;
    }
    return cnt;
}

unsigned int resHalfSec(unsigned int delay){
    while (delay > 500){
        delay -= 500;
    }
    unsigned int i = 0, returnDelay;
    returnDelay = delay << 7;
    for ( i = 3 ; i ; i--){
        returnDelay += delay;
    }
    return (returnDelay);
}
//-------------------------------------------------------------
//                 2. Count up
//------------------------------------------------------------
void incLcd(unsigned int x ){
    char counter_char[7] = {0};
    unsigned int i  , counterLength,cnt = 0;
    lcd_puts("CountUp:");
    lcd_new_line;
    while (cnt < x){
        counterLength = numOfDigits(cnt);
        uint2Str(counter_char , cnt, counterLength);
        lcd_puts(counter_char);
        timerDelay();
        cnt ++;
        for(i = counterLength; i; i--)
            lcd_cursor_left();
    }
    lcd_clear();
    stopTimerA0();
}

//-------------------------------------------------------------
//                2. Count down
//------------------------------------------------------------
void decLcd(unsigned int x ){
    char counter_char[7] = {0};
    unsigned int i  , counterLength,cnt = x;
    lcd_puts("CountDown:");
    lcd_new_line;
    while (cnt){
        counterLength = numOfDigits(cnt);
        uint2Str(counter_char , cnt, counterLength);
        lcd_puts(counter_char);
        timerDelay();
        cnt --;
        for(i = counterLength; i; i--)
            lcd_cursor_left();
    }
    lcd_clear();
    stopTimerA0();
}


//-------------------------------------------------------------
//                2. rotate right lcd
//------------------------------------------------------------
void rraLcd(unsigned char x ){
    unsigned int cnt;
    cursor_off;
    for (cnt = 32 ; cnt ; cnt--){
        lcd_putchar(x);
        timerDelay();
        if(cnt == 17){
            lcd_clear();
            lcd_new_line;
        }
        else{
            lcd_cursor_left();
            lcd_putchar(' ');
        }
    }
    lcd_clear();
    stopTimerA0();
}

//-------------------------------------------------------------
//                4. Change Delay Time [ms]
//------------------------------------------------------------
void setDelay(unsigned int delay){
    NUM_OF_500ms = moduloHalfSec(delay);
    DELAY_RES = resHalfSec(delay);
    TIMER_CNT = (NUM_OF_500ms)? (NUM_OF_500ms-1) : 0;
}


void ScanBetweenAngles(int l, int r){
    unsigned int lIndex = conDegToIndex(l);
    unsigned int rIndex = conDegToIndex(r);
    unsigned int servoDeg = conDeg2DutyCyc(l);
    unsigned int distArr[DIST_ARR_LENGTH] = {0};
    ServoMotorConfig(servoDeg);
    servoDelay(20);
    while (lIndex <= rIndex){
        distArr[lIndex++] = ultraSonicMeasure();
        servoDeg += DEG_DELTA;

        changeServoDegree(servoDeg);
    }
    SendVal2PC(distArr);
}

unsigned int conDegToIndex(int deg){
    unsigned int cnt = 0;
    while (deg > 0){
        deg -= 3;
        cnt ++;
    }
    return cnt;
}
