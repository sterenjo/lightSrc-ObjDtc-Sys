#include  "../header/halGPIO.h"     // private library - HAL layer


unsigned int INDEX = 0;
unsigned int Cap1Val[2] = {0};
unsigned int ECHO_HL_TIME = 0;
unsigned int DIST = 0;
unsigned int ADC_VAL = 0;
unsigned int NUM_SAMPLES = 32;
unsigned int UART_INDEX;
unsigned char ACK;
unsigned char EXIT = 0x0;
unsigned int DIST_LIGHT_ARR[2][DIST_ARR_LENGTH];
unsigned char TELEMETER_FLAG;
unsigned int NUM_OF_500ms = 1;
unsigned int DELAY_RES = 0;
unsigned int TIMER_CNT;
unsigned char CH;

enum state5Mode scriptMode;

unsigned char STR_TX;
char *STR_RX;



//--------------------------------------------------------------------
//             System Configuration  
//--------------------------------------------------------------------
void sysConfig(void){ 
    UART_init();
	GPIOconfig();
	TIMERconfig();
	//ADCconfig();
	lcd_init();
}
//--------------------------------------------------------------------
// 				Print Byte to 8-bit LEDs array 
//--------------------------------------------------------------------
void print2LEDs(unsigned char ch){
	LEDsArrPort = ch;
}    
//--------------------------------------------------------------------
//				Clear 8-bit LEDs array 
//--------------------------------------------------------------------
void clrLEDs(void){
	LEDsArrPort = 0x000;
}  
//--------------------------------------------------------------------
//				Toggle 8-bit LEDs array 
//--------------------------------------------------------------------
void toggleLEDs(char ch){
	LEDsArrPort ^= ch;
}
//--------------------------------------------------------------------
//				Set 8-bit LEDs array 
//--------------------------------------------------------------------
void setLEDs(char ch){
	LEDsArrPort |= ch;
}
//--------------------------------------------------------------------
//				Read value of 4-bit SWs array 
//--------------------------------------------------------------------
unsigned char readSWs(void){
	unsigned char ch;
	
	ch = PBsArrPort;
	ch &= SWmask;     // mask the least 4-bit
	return ch;
}
//---------------------------------------------------------------------
//             Increment / decrement LEDs shown value 
//---------------------------------------------------------------------
void incLEDs(char val){
	LEDsArrPort += val;
}

void startTimerA0(void){
    TA0CTL = TASSEL_2 + MC_1; // SMCLK , up mode
}
void stopTimerA0(void){
    TA0CTL = TASSEL_2 + MC_0; // SMCLK , up mode

}

void startTimerA1(void){

    TA1CTL = TASSEL_2 + MC_3; // SMCLK , up mode
    TA1CCTL1 |= OUTMOD_7;
    //TA1CCTL2 |= CCIE;
}
void stopTimerA1(void){
    //TA1CCTL0 &=~CCIE;
    TA1CCTL1 &= ~OUTMOD_7;
    TA1CCTL2 &= ~CCIE;
    TA1CTL = MC_0; // SMCLK , up mode
}

void ObjectsDetectorConfig(unsigned int ServoDeg){
    ServoMotorConfig(ServoDeg);
    UltSonTimerA1config();
    //startTimerA0();
    //enterLPM(lpm_mode);
    //stopTimerA0();
}

void servoDelay(unsigned int loops){
    unsigned int i;

    enable_TA0_interrupts();
    startTimerA0();
    for ( i = loops ; i ; i--)
        enterLPM(lpm_mode);
    disable_TA0_interrupts();
    stopTimerA0();

}
void lightSrcDetectorConfig(unsigned int ServoDeg){
    ServoMotorConfig(ServoDeg);
    LDR_FLAG = 0x0;
    servoDelay(20);
    ADCconfig();
}

void timerDelay(void){
    TIMER_CNT = (NUM_OF_500ms)? (NUM_OF_500ms-1) : 0;
    TimerState5Config();
    if (NUM_OF_500ms){
        TimerA0CCR2_config(DELAY_500ms);
        timerA02InterruptEnable();
        enterLPM(lpm_mode);
    }
    if (DELAY_RES){
        TimerA0CCR2_config(DELAY_RES);
        timerA02InterruptEnable();
        enterLPM(lpm_mode);
    }
    timerA02InterruptDisable();
}

void changeServoDegree(unsigned int ServoDeg){
    TA0CCR1 = ServoDeg;
    TA0CCR2 = MS_SERVO_DELAY;
    servoDelay(5);
}
//---------------------------------------------------------------------
//            Enter from LPM0 mode
//---------------------------------------------------------------------
void enterLPM(unsigned char LPM_level){
	if (LPM_level == 0x00)
	  _BIS_SR(LPM0_bits);     /* Enter Low Power Mode 0 */
        else if(LPM_level == 0x01) 
	  _BIS_SR(LPM1_bits);     /* Enter Low Power Mode 1 */
        else if(LPM_level == 0x02) 
	  _BIS_SR(LPM2_bits);     /* Enter Low Power Mode 2 */
	else if(LPM_level == 0x03) 
	  _BIS_SR(LPM3_bits);     /* Enter Low Power Mode 3 */
        else if(LPM_level == 0x04) 
	  _BIS_SR(LPM4_bits);     /* Enter Low Power Mode 4 */
}
//---------------------------------------------------------------------
//            Enable interrupts
//---------------------------------------------------------------------
void enable_interrupts(){
  _BIS_SR(GIE);
}
//---------------------------------------------------------------------
//            Disable interrupts
//---------------------------------------------------------------------
void disable_interrupts(){
  _BIC_SR(GIE);
}
//---------------------------------------------------------------------
//            Enable interrupts
//---------------------------------------------------------------------
void enable_TA0_interrupts(){
    TA0CCTL2 |= CCIE;      // Enable TACCR2 interrupt (optional, if you want to use interrupts)
}
//---------------------------------------------------------------------
//            disable interrupts
//---------------------------------------------------------------------
void disable_TA0_interrupts(){
    TA0CCTL2 &= ~CCIE;
}

void enableUartReceive(void){
    IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt
}
//---------------------------------------------------------------------
//            Enable ADC interrupts
//---------------------------------------------------------------------
unsigned int enable_ADC(unsigned int numOfSamples, unsigned int numOfShifts){
    ADC_VAL = 0;
    unsigned int sum = 0;
    int i ;
    //servoDelay(5);
    NUM_SAMPLES = numOfSamples;
    //changeAdcInput();
    ADCconfig();
    ADC10CTL0 |= ENC + ADC10IE +ADC10SC; // Start sampling
    enterLPM(lpm_mode);
    return( ADC_VAL >> numOfShifts);


    /*for (i = numOfSamples ; i ; i--){
        ADC10CTL0 |= ADC10SC + ENC; // Start sampling
        __bis_SR_register(CPUOFF + GIE);        // LPM0, ADC10_ISR will force exit
        ADC10CTL0 &= ~ENC ;
        sum = sum + ADC_VAL;
        if (ADC_VAL != 1023){
            __no_operation();                       // SET BREAKPOINT HERE
        }
    }*/
    //return(sum>>numOfShifts);
}
//---------------------------------------------------------------------
//            Disable ADC interrupts
//---------------------------------------------------------------------
void disable_ADC(){
    ADC10CTL0 &= ~ADC10IE; // Don't get into interrupt
}

void sendCharToPc(char ch){
    //UART_INDEX = 0;
    while (IE2 & UCA0TXIE);
    STR_TX = ch;
    IE2 |= UCA0TXIE;
}

/*void sendTerminator(char treminator){
    UART_INDEX = 0;
    *STR_TX = treminator;
    while (IE2 & UCA0TXIE);
    IE2 |= UCA0TXIE;

}*/

//*********************************************************************
//            Port2 Interrupt Service Rotine
//*********************************************************************
#pragma vector=PORT2_VECTOR
__interrupt void PBs_handler(void){
   
    delay(debounceVal);
//---------------------------------------------------------------------
//            selector of transition between states
//---------------------------------------------------------------------
    if(PBsArrIntPend & PB0){
        state = state1;
        PBsArrIntPend &= ~PB0;
    }
    else if(PBsArrIntPend & PB1){
        state = state2;
        PBsArrIntPend &= ~PB1;
    }
    else if(PBsArrIntPend & PB2){
        state = state0;
        PBsArrIntPend &= ~PB2;
    }
//---------------------------------------------------------------------
//            Exit from a given LPM 
//---------------------------------------------------------------------	
    switch(lpm_mode){
	    case mode0:
	        LPM0_EXIT; // must be called from ISR only
	        break;
		 
		case mode1:
		    LPM1_EXIT; // must be called from ISR only
		    break;
		 
		case mode2:
		    LPM2_EXIT; // must be called from ISR only
		    break;
                 
		case mode3:
		    LPM3_EXIT; // must be called from ISR only
		    break;
                 
		case mode4:
		    LPM4_EXIT; // must be called from ISR only
		    break;
    }
        
}

//*********************************************************************
//            Port2 Interrupt Service Rotine
//*********************************************************************

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER1_A1_VECTOR
__interrupt void Timer_A1(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER1_A1_VECTOR))) Timer_A1 (void)
#else
#error Compiler not supported!
#endif
{

    switch(__even_in_range(TA1IV,0x0A)){
    case  TA1IV_NONE:       // Vector  0:  No interrupt
        //TA1CCTL0 &= ~CCIFG;
        //LPM0_EXIT; // must be called from ISR only
    break;
    case  TA1IV_TACCR1:             // Vector  2:  TACCR1 CCIFG
        TA1CTL &= ~(TAIFG);
        break;
    case  TA1IV_TACCR2:                    // Vector  4:  TACCR2 CCIFG
              // Rising/falling Edge was captured
        TA1CCTL2 &= ~CCIFG;
        Cap1Val[INDEX++] = TA1CCR2;  // save TA1CCR2 value inorder to calculte the difference
        if (INDEX == 2){
            ECHO_HL_TIME = (Cap1Val[1] > Cap1Val[0])? Cap1Val[1] - Cap1Val[0] : Cap1Val[1] - Cap1Val[0] + 0xffff;
            INDEX = 0;
            LPM0_EXIT; // must be called from ISR only
        }
        break;
    case TA1IV_6: break;                  // Vector  6:  Reserved CCIFG
    case TA1IV_8: break;                  // Vector  8:  Reserved CCIFG
    case TA1IV_TAIFG: break;              // Vector 10:  TAIFG

    default:  break; }
}



#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER0_A1_VECTOR
__interrupt void Timer_A0(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER0_A1_VECTOR))) Timer_A0 (void)
#else
#error Compiler not supported!
#endif
{

    switch(__even_in_range(TA0IV,0x0A)){
    case  TA0IV_NONE:
        LPM0_EXIT;
        break;        // Vector  0:  No interrupt
    case  TA0IV_TACCR1:             // Vector  2:  TACCR1 CCIFG
        TA0CCTL1 &= ~CCIFG;
        LPM0_EXIT;
        break;
    case TA0IV_TACCR2:
        TA0CCTL2 &= ~CCIFG;
        LPM0_EXIT;
        break;
    case TA0IV_6:
        LPM0_EXIT;
        break;                  // Vector  6:  Reserved CCIFG
    case TA0IV_8:
        LPM0_EXIT;
        break;                  // Vector  8:  Reserved CCIFG
    case TA0IV_TAIFG:                     // Vector 10:  TAIFG
        TA0CTL &= ~CCIFG;
        LPM0_EXIT;
        break;
    default:
        LPM0_EXIT;
        break;

    }
}

// ADC10 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(ADC10_VECTOR))) ADC10_ISR (void)
#else
#error Compiler not supported!
#endif
{
    if (NUM_SAMPLES){
        ADC_VAL += ADC10MEM;
        NUM_SAMPLES--;
    }
    else{
        ADC10CTL0 &= ~(ENC + ADC10IE);                    //
    //---------------------------------------------------------------------
    //            Exit from a given LPM
    //---------------------------------------------------------------------
        switch(lpm_mode){
            case mode0:
                LPM0_EXIT; // must be called from ISR only
                break;

            case mode1:
                LPM1_EXIT; // must be called from ISR only
                break;

            case mode2:
                LPM2_EXIT; // must be called from ISR only
                break;

            case mode3:
                LPM3_EXIT; // must be called from ISR only
                break;

            case mode4:
                LPM4_EXIT; // must be called from ISR only
                break;
    }
    }
}

//*********************************************************************
//                           TX ISR
//*********************************************************************
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCIAB0TX_VECTOR
__interrupt void USCI0TX_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCIAB0TX_VECTOR))) USCI0TX_ISR (void)
#else
#error Compiler not supported!
#endif
{
    UCA0TXBUF = STR_TX;
    IE2 &= ~UCA0TXIE;                       // Disable USCI_A0 TX interrupt

}


//*********************************************************************
//                         RX ISR
//*********************************************************************
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCIAB0RX_VECTOR))) USCI0RX_ISR (void)
#else
#error Compiler not supported!
#endif
{
    /*if (delay_ifg){
        string1[j] = UCA0RXBUF;
        j++;
        if (string1[j-1] == '\n'){
            j = 0;
            delay_ifg = 0;
            state_flag = 0;
        }
    }*/
    CH = UCA0RXBUF;
    if (state == state0){
        if(CH == 's'){
            EXIT = 0x1;
            state = state1;
        }
        else if(CH == 'o'){
            EXIT = 0x1;
            state = state2;
        }
        else if(CH == 't'){
            EXIT = 0x1;
            state = state3;
        }
        else if(CH == 'l'){
            EXIT = 0x1;
            state = state4;
        }
        else if(CH == 'b'){
            EXIT = 0x1;
            state = state5;
        }
        else if (CH == 'e'){
            state = state6;
        }
    }
    else if (state == state1){
        if(CH == '1')
            EXIT = 0x1;
    }
    else if (state == state2){
        STR_RX[UART_INDEX] = CH;
        if (!TELEMETER_FLAG){
            if (CH == '\n'){
                EXIT = 0x1;
                STR_RX[UART_INDEX] = '\0';
            }
            else
                UART_INDEX++;
        }
        else if (STR_RX[UART_INDEX] == '0'){
            //state = state0;
        }
        else if (CH == 's')
            state = state1;
        else if (CH == 'o')
            state = state3;
        else if (CH== 'l')
            state = state4;
        else if (CH == 'b')
            state = state5;
        else
            EXIT = 0x1;
    }
    else if (state == state3){
            if(CH == '1')
                EXIT = 0x1;
    }

    else if (state == state5){
        if (scriptMode == scriptNum){

        }
        else if (scriptMode == scriptData){   //if user press execute script
            if (CH == '\n'){
                EXIT = 0x1;
            }

        }
        else if (scriptMode == exitCall){
            if (CH == '0'){
                state = state0;
            }
            else if(CH == 's'){
                EXIT = 0x1;
                state = state1;
            }
            else if(CH == 'o'){
                EXIT = 0x1;
                state = state2;
            }
            else if(CH == 't'){
            EXIT = 0x1;
                state = state3;
            }
            else if(CH == 'l'){
                EXIT = 0x1;
                state = state4;
            }
        }

    }

    /*else if(UCA0RXBUF == '6'){
        state = state6;
    }
    else if(UCA0RXBUF == '7' && delay_ifg == 0){
        state = state7;
    }
    else if(UCA0RXBUF == '8' && delay_ifg == 0){
        state = state8;
    }
    else if(UCA0RXBUF == '9' && delay_ifg == 0){
        state = state9;
    }*/
    switch(lpm_mode){
        case mode0:
            LPM0_EXIT; // must be called from ISR only
            break;
        case mode1:
            LPM1_EXIT; // must be called from ISR only
            break;
        case mode2:
            LPM2_EXIT; // must be called from ISR only
            break;
        case mode3:
            LPM3_EXIT; // must be called from ISR only
            break;
        case mode4:
            LPM4_EXIT; // must be called from ISR only
            break;

    }
}

//---------------------------------------------------------------------
//            LCD
//---------------------------------------------------------------------
//******************************************************************
// send a command to the LCD
//******************************************************************
void lcd_cmd(unsigned char c){

    LCD_WAIT; // may check LCD busy flag, or just delay a little, depending on lcd.h

    if (LCD_MODE == FOURBIT_MODE)
    {
        LCD_DATA_WRITE &= ~OUTPUT_DATA;// clear bits before new write
        LCD_DATA_WRITE |= ((c >> 4) & 0x0F) << LCD_DATA_OFFSET;
        lcd_strobe();
        LCD_DATA_WRITE &= ~OUTPUT_DATA;
        LCD_DATA_WRITE |= (c & (0x0F)) << LCD_DATA_OFFSET;
        lcd_strobe();
    }
    else
    {
        LCD_DATA_WRITE = c;
        lcd_strobe();
    }
}
//******************************************************************
// send data to the LCD
//******************************************************************
void lcd_data(unsigned char c){

    LCD_WAIT; // may check LCD busy flag, or just delay a little, depending on lcd.h

    LCD_DATA_WRITE &= ~OUTPUT_DATA;
    LCD_RS(1);
    if (LCD_MODE == FOURBIT_MODE)
    {
            LCD_DATA_WRITE &= ~OUTPUT_DATA;
            LCD_DATA_WRITE |= ((c >> 4) & 0x0F) << LCD_DATA_OFFSET;
            lcd_strobe();
            LCD_DATA_WRITE &= (0xF0 << LCD_DATA_OFFSET) | (0xF0 >> 8 - LCD_DATA_OFFSET);
            LCD_DATA_WRITE &= ~OUTPUT_DATA;
            LCD_DATA_WRITE |= (c & 0x0F) << LCD_DATA_OFFSET;
            lcd_strobe();
    }
    else
    {
            LCD_DATA_WRITE = c;
            lcd_strobe();
    }

    LCD_RS(0);
}
//******************************************************************
// write a string of chars to the LCD
//******************************************************************
void lcd_puts(const char * s){

    while(*s)
        lcd_data(*s++);
}
//******************************************************************
// initialize the LCD
//******************************************************************
void lcd_init(){

    char init_value;

    if (LCD_MODE == FOURBIT_MODE) init_value = 0x3 << LCD_DATA_OFFSET;
    else init_value = 0x3F;

    LCD_RS_DIR(OUTPUT_PIN);
    LCD_EN_DIR(OUTPUT_PIN);
    LCD_RW_DIR(OUTPUT_PIN);
    LCD_DATA_DIR |= OUTPUT_DATA;
    LCD_RS(0);
    LCD_EN(0);
    LCD_RW(0);

    DelayMs(15);
    LCD_DATA_WRITE &= ~OUTPUT_DATA;
    LCD_DATA_WRITE |= init_value;
    lcd_strobe();
    DelayMs(5);
    LCD_DATA_WRITE &= ~OUTPUT_DATA;
    LCD_DATA_WRITE |= init_value;
    lcd_strobe();
    DelayUs(200);
    LCD_DATA_WRITE &= ~OUTPUT_DATA;
    LCD_DATA_WRITE |= init_value;
    lcd_strobe();

    if (LCD_MODE == FOURBIT_MODE){
        LCD_WAIT; // may check LCD busy flag, or just delay a little, depending on lcd.h
        LCD_DATA_WRITE &= ~OUTPUT_DATA;
        LCD_DATA_WRITE |= 0x2 << LCD_DATA_OFFSET; // Set 4-bit mode
        lcd_strobe();
        lcd_cmd(0x28); // Function Set
    }
    else lcd_cmd(0x3C); // 8bit,two lines,5x10 dots

    lcd_cmd(0xF); //Display On, Cursor On, Cursor Blink
    lcd_cmd(0x1); //Display Clear
    lcd_cmd(0x6); //Entry Mode
    lcd_cmd(0x80); //Initialize DDRAM address to zero
}
//******************************************************************
// lcd strobe functions
//******************************************************************
void lcd_strobe(){
  LCD_EN(1);
  asm("NOP");
  asm("nOP");
  LCD_EN(0);
}
//---------------------------------------------------------------------
//                     Polling delays
//---------------------------------------------------------------------
//******************************************************************
// Delay usec functions
//******************************************************************
void DelayUs(unsigned int cnt){

    unsigned char i;
    for(i=cnt ; i>0 ; i--) asm("nop"); // tha command asm("nop") takes raphly 1usec

}
//******************************************************************
// Delay msec functions
//******************************************************************
void DelayMs(unsigned int cnt){

    unsigned int i;
    for(i=cnt ; i>0 ; i--)
        DelayUs(1000); // tha command asm("nop") takes raphly 1usec

}
//******************************************************************
//            Polling based Delay function
//******************************************************************
void delay(unsigned int t){  //
    volatile unsigned int i;

    for(i=t; i>0; i--);
}



//******************************************************************
//                          FLASH
//******************************************************************



unsigned char DOWNLOAD_SCRIPT_FLAG;
unsigned char GET_NUM_OF_SCRIPT_FLAG;


char* getScriptSeg(int num) {
    char *ptr = 0;
    if (num == 1)
        ptr = (char*)FLASH_SEG_D;
    if (num == 2)
        ptr = (char*)FLASH_SEG_C;
    if (num == 3)
        ptr = (char*)FLASH_SEG_B;

    return ptr;
}


void flashWrite(unsigned char* seg,unsigned char* data, unsigned int length){
    char *flashPtr;    // Flash pointer
    unsigned int i;



    flashConfig();

    flashPtr = (char *)seg;

    flashErase();

    *flashPtr = 0;                           // Dummy write to erase Flash segment

    setFlashWRT();

    for (i = 0 ; i < length ; i++){
        *flashPtr++ = data[i];
    }
    clrFlashWrt();
    flashLock();
}


void flashRead(char* dataAddr, unsigned char* outputArr , unsigned int len){
    char *flashPtr;
    unsigned int i;

    flashPtr = dataAddr;
    for (i = 0 ; i< len ; i++){
        outputArr[i] = flashPtr[i];
    }
}

void flashErase(void){
    FCTL1 = FWKEY + ERASE;         // Set Erase bit
    FCTL3 = FWKEY;                 // Clear Lock bit
}

void setFlashWRT(void){
    FCTL1 = FWKEY + WRT;                      // Set WRT bit for write operation
}

void flashLock(void){
    FCTL3 = FWKEY + LOCK;          // Set LOCK bit
}
void clrFlashWrt(void){
    FCTL1 = FWKEY;                            // Clear WRT bit
}
