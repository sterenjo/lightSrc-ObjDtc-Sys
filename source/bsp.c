#include  "../header/bsp.h"    // private library - BSP layer
#include  "../header/app.h"


//unsigned int MS_SERVO_DELAY = 0x28F6;
unsigned int MS_SERVO_DELAY = 0x1AA0;
unsigned char LDR_FLAG = '0';

//-----------------------------------------------------------------------------  
//           GPIO congiguration
//-----------------------------------------------------------------------------
void GPIOconfig(void){
 // volatile unsigned int i; // in case of while loop usage
  
  WDTCTL = WDTHOLD | WDTPW;		// Stop WDT
   
  // LEDs 8-bit Array Port configuration
  //LEDsArrPortSel &= ~0xFF;            // GPIO capability
  //LEDsArrPortDir |= 0xFF;             // output dir
  //LEDsArrPort = 0x00;				  // clear all LEDs
  
  /*// Switches Setup
  SWsArrPortSel &= ~0x0F;
  SWsArrPortDir &= ~0x0F;
  
  // PushButtons Setup
  PBsArrPortSel &= ~0xF0;
  PBsArrPortDir &= ~0xF0;
  PBsArrIntEdgeSel |= 0x30;  	     // pull-up mode
  PBsArrIntEdgeSel &= ~0xC0;         // pull-down mode
  PBsArrIntEn |= 0x70;
  PBsArrIntPend &= ~0xF0;            // clear pending interrupts*/
  


  // UltraSonic setup

  UsTrigDir     |= 0x04;           // P2.2 output
  UsTrigSel     |= 0x04;            // P2.2 TA1/1 options


  UsEchoDir     &= ~0x10;            // p2.4 TA1.2
  UsEchoSel     |= 0x10;            // p2.4 TA1.2

  // servo setup

  ServoDir     |= 0x40;           // P2.6 output
  ServoSel     |= 0x40;            // P2.6 TA1/1 options
  ServoSel     &= ~0x80;            // P2.7 TA1/1 options

  // ADC


  _BIS_SR(GIE);                     // enable interrupts globally
}
//------------------------------------------------------------------------------------- 
//            Timers congiguration 
//-------------------------------------------------------------------------------------
void TIMERconfig(void){
    //UltSonTimerA1config();
	
	//write here timers congiguration code
} 

void UltSonTimerA1config(void){
    TA1CCR0 = 0xFFFF;       //period = 60 ms
    TA1CCR1 = 0x000A;       // duty cycle of 10us
    TA1CCTL1 = OUTMOD_7;       // CCR1 reset/set
    TA1CCTL2 = CAP | CCIS_0 | CM_3 | SCS | CCIE; // captureMode , CC interrupt enable, CaptureCompare Input, on rising&falling edge, sync
    TA1CTL = TASSEL_2 + MC_3; // SMCLK , cont mode

}

void ServoMotorConfig(unsigned int DegreeDutyCycle){
    TA0CTL = TACLR;
    TA0CCR0 = 0x6300;       //period = 25 ms
    TA0CCR1 = DegreeDutyCycle;       // duty cycle
    TA0CCTL1 = OUTMOD_7;       // CCR1 reset/set
    TA0CCR2  = MS_SERVO_DELAY;
    TA0CCTL2 |= CCIE;      // Enable TACCR2 interrupt (optional, if you want to use interrupts)
    TA0CCTL2 &= ~(CAP | OUTMOD_7); // Set TACCR2 for compare mode, clear OUTMOD bits (OUTMOD_0)}
}


void DelayHalfSecConfig(){
    TA0CCTL1 = 0;
    TA0CCTL2 = 0;
    TA0CTL = TACLR | ID_3 | TAIE | MC_1 | TASSEL_2 ;
}

void TimerState5Config(){
    TA0CTL = TASSEL_2 + MC_2 + ID_3 + TACLR;  //  select: 2 - SMCLK ; control: 1 - Up ; divider: 3 - /8
}

void TimerA0CCR2_config(unsigned int counter){
    TACCR2 = counter;
}

void timerA02InterruptEnable(){
    TACCTL2 = CCIE;
}
void timerA02InterruptDisable(){
    TACCTL2 &= ~CCIE;
}


//------------------------------------------------------------------------------------- 
//            ADC congiguration 
//-------------------------------------------------------------------------------------
void ADCconfig(void){
    ADC10CTL0 = ADC10SHT_1 + ADC10ON + SREF_0 + MSC;  // 16*ADCLK+ Turn on, set ref to Vcc and Gnd
    ADC10CTL1 = ADC10SSEL_3 + ADC10DIV_3 + CONSEQ_2 ;
    ADC10AE0  = BIT0 | BIT3;
    if (!LDR_FLAG){
        ADC10CTL1 &= ~LDR2;
        ADC10CTL1 |= LDR1;
    }
    else{
        ADC10CTL1 &= ~LDR1;
        ADC10CTL1 |= LDR2;
    }
    LDR_FLAG = (~LDR_FLAG) & 0x01;
}

void changeAdcInput(void){
    ADC10CTL0 &= ~ENC ;
    if (!LDR_FLAG){
        ADC10CTL1 &= ~LDR2;
        ADC10CTL1 |= LDR1;
    }
    else{
        ADC10CTL1 &= ~LDR1;
        ADC10CTL1 |= LDR2;
    }
}
//-------------------------------------------------------------------------------------
//                              UART init
//-------------------------------------------------------------------------------------
void UART_init(void){
    if (CALBC1_1MHZ==0xFF)                  // If calibration constant erased
      {
        while(1);                               // do not load, trap CPU!!
      }
    DCOCTL = 0;                               // Select lowest DCOx and MODx settings
    BCSCTL1 = CALBC1_1MHZ;                    // Set DCO
    DCOCTL = CALDCO_1MHZ;

    P2DIR = 0xFF;                             // All P2.x outputs
    P2OUT = 0;                                // All P2.x reset
    P1SEL = BIT1 + BIT2 ;                     // P1.1 = RXD, P1.2=TXD
    P1SEL2 = BIT1 + BIT2 ;                     // P1.1 = RXD, P1.2=TXD
    P1OUT &= 0x00;

    UCA0CTL1 |= UCSSEL_2;                     // CLK = SMCLK
    UCA0BR0 = 104;                           //
    UCA0BR1 = 0x00;                           //
    UCA0MCTL = UCBRS0;               //
    UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
}


//********************************************************************
//                      flash
//********************************************************************
void flashConfig(void){
    if (CALBC1_1MHZ==0xFF)                    // If calibration constant erased
    {
      while(1);                               // do not load, trap CPU!!
    }
    DCOCTL = 0;                               // Select lowest DCOx and MODx settings
    BCSCTL1 = CALBC1_1MHZ;                    // Set DCO to 1MHz
    DCOCTL = CALDCO_1MHZ;
    FCTL2 = FWKEY + FSSEL0 + FN1;             // MCLK/3 for Flash Timing Generator
}
           
             

 
             
             
            
  

