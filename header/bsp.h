#ifndef _bsp_H_
#define _bsp_H_

#include  <msp430g2553.h>          // MSP430x2xx
// #include  <msp430xG46x.h>  // MSP430x4xx


#define   debounceVal      250
#define   LEDs_SHOW_RATE   0xFFFF  // 62_5ms
#define   DEG_DELTA        0x1C
//#define   SERVO_INIT_DEG   0x275
#define   SERVO_INIT_DEG   0x275 // 629 - 0.6 ms
#define   DIST_ARR_LENGTH  55
#define   HEX               16
#define   DELAY_500ms       0xFFFF

// UltraSonic abstraction
#define UsTrigDir         P2DIR
#define UsTrigSel         P2SEL

#define UsEcho            P2IN
#define UsEchoDir         P2DIR
#define UsEchoSel         P2SEL

// servo abstraction
#define ServoDir         P2DIR
#define ServoSel         P2SEL

// ldr abstraction
#define LDR1            INCH_3
#define LDR2            INCH_0

#define TXD BIT2
#define RXD BIT1



// LEDs abstraction
#define LEDsArrPort        P1OUT
#define LEDsArrPortDir     P1DIR
#define LEDsArrPortSel     P1SEL

// Switches abstraction
#define SWsArrPort         P2IN
#define SWsArrPortDir      P2DIR
#define SWsArrPortSel      P2SEL
#define SWmask             0x0F

// PushButtons abstraction
#define PBsArrPort	   P2IN 
#define PBsArrIntPend	   P2IFG 
#define PBsArrIntEn	   P2IE
#define PBsArrIntEdgeSel   P2IES
#define PBsArrPortSel      P2SEL 
#define PBsArrPortDir      P2DIR 
#define PB0                0x10
#define PB1                0x20
#define PB2                0x40
#define PB3                0x80

// LCDs abstraction
#define LCD_DATA_WRITE     P1OUT
#define LCD_DATA_DIR       P1DIR
#define LCD_DATA_READ      P1IN
#define LCD_DATA_SEL       P1SEL

#define OUTPUT_PIN      1
#define INPUT_PIN       0
#define OUTPUT_DATA     (LCD_MODE ? 0xFF : (0x0F << LCD_DATA_OFFSET))
#define INPUT_DATA      0x00



#define FOURBIT_MODE    0x0
#define EIGHTBIT_MODE   0x1
#define LCD_MODE        FOURBIT_MODE

#define LCD_CTL_SEL        P2SEL
#define LCD_EN_BIT         0x01  //p2.0
#define LCD_RS_BIT         0x02  //p2.1
#define LCD_RW_BIT         0x08  //p2.3

/*----------------------------------------------------------
  CONFIG: change values according to your port pin selection
------------------------------------------------------------*/
#define LCD_EN(a)   (!a ? (P2OUT&=~LCD_EN_BIT) : (P2OUT|=LCD_EN_BIT)) // P2.0 is lcd enable pin
#define LCD_EN_DIR(a)   (!a ? (P2DIR&=~LCD_EN_BIT) : (P2DIR|=LCD_EN_BIT)) // P2.0 pin direction

#define LCD_RS(a)   (!a ? (P2OUT&=~LCD_RS_BIT) : (P2OUT|=LCD_RS_BIT)) // P2.1 is lcd RS pin
#define LCD_RS_DIR(a)   (!a ? (P2DIR&=~LCD_RS_BIT) : (P2DIR|=LCD_RS_BIT)) // P2.1 pin direction

#define LCD_RW(a)   (!a ? (P2OUT&=~LCD_RW_BIT) : (P2OUT|=LCD_RW_BIT)) // P2.3 is lcd RW pin
#define LCD_RW_DIR(a)   (!a ? (P2DIR&=~LCD_RW_BIT) : (P2DIR|=LCD_RW_BIT)) // P2.3 pin direction

#define LCD_DATA_OFFSET 0x04 //data pin selection offset for 4 bit mode, variable range is 0-4, default 0 - Px.0-3, no offset

// #define CHECKBUSY    1  // using this define, only if we want to read from LCD

#ifdef CHECKBUSY
    #define LCD_WAIT lcd_check_busy()
#else
    #define LCD_WAIT DelayMs(5)
#endif


#define LCD_STROBE_READ(value)  LCD_EN(1), \
                asm("nop"), asm("nop"), \
                value=LCD_DATA_READ, \
                LCD_EN(0)

#define lcd_cursor(x)       lcd_cmd(((x)&0x7F)|0x80)
#define lcd_clear()         lcd_cmd(0x01)
#define lcd_putchar(x)      lcd_data(x)
#define lcd_goto(x)         lcd_cmd(0x80+(x))
#define lcd_cursor_right()  lcd_cmd(0x14)
#define lcd_cursor_left()   lcd_cmd(0x10)
#define lcd_display_shift() lcd_cmd(0x1C)
#define lcd_home()          lcd_cmd(0x02)
#define cursor_off          lcd_cmd(0x0C)
#define cursor_on           lcd_cmd(0x0F)
#define lcd_function_set    lcd_cmd(0x3C) // 8bit,two lines,5x10 dots
#define lcd_new_line        lcd_cmd(0xC0)
/*---------------------------------------------------------
  END CONFIG
-----------------------------------------------------------*/

extern unsigned int MS_SERVO_DELAY;
extern unsigned char LDR_FLAG;
extern void GPIOconfig(void);
extern void TIMERconfig(void);
extern void ADCconfig(void);
extern void ServoMotorConfig(unsigned int DegreeDutyCycle);
extern void UltSonTimerA1config(void);
extern void changeAdcInput(void);
extern void UART_init(void);
extern void DelayHalfSecConfig();
extern void TimerState5Config();
extern void TimerA0CCR2_config(unsigned int counter);
extern void timerA02InterruptEnable();
extern void timerA02InterruptDisable();

extern void flashConfig(void);

#endif



