#ifndef program_H
#define	program_H

#include <xc.h>

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#ifdef	__cplusplus
extern "C" {
#endif
    
    
#define _User_FOSC      32000000 // CPU Clock Frequency
    
#define _Address_LMSB1      0x10 // Address for LEDs matrix slave board 1
#define _Address_LMSB2      0x20 // Address for LEDs matrix slave board 2
    
#define led1        LATBbits.LATB7
#define led2        LATBbits.LATB6
    
#define RS_Pin      LATBbits.LATB5
#define E_Pin       LATBbits.LATB4
#define D4_Pin      LATBbits.LATB3
#define D5_Pin      LATBbits.LATB2
#define D6_Pin      LATBbits.LATB1
#define D7_Pin      LATCbits.LATC5
    
#define pb_Up       PORTAbits.RA1
#define pb_Down     PORTAbits.RA2
#define pb_Left     PORTAbits.RA3
#define pb_Right    PORTAbits.RA4


    void delay_x1o5us(uint8_t delay);
    void delay_x24o25us(uint16_t delay);
    void delay_ms(uint32_t delay);
    
    void programInitialize(void);
    void programLoop(void);
    
    void pb_DelayDebounce(void);
    
    void lcd_DelaySetupTime(void);
    void lcd_DelayPulse(void);
    void lcd_EPulse(void);
    void lcd_WriteData(uint8_t data);
    
    void lcd_PrintCmd(uint8_t command);
    void lcd_Initialize(void);
    void lcd_ClearAll(void);
    void lcd_Goto(uint8_t y, uint8_t x);
    void lcd_PrintChar(char character);
    void lcd_PrintString(char *string);
    void lcd_PrintInt32(int32_t number);
    void lcd_PrintDigitInt32(int32_t number, uint8_t noDigit, bool enSign, bool enZero);
    
    void i2c_Initialize(uint32_t fosc, uint16_t baudrate);
    void i2c_BusIdle(void);
    void i2c_MasterStart(void);
    void i2c_MasterRstart(void);
    void i2c_MasterStop(void);
    void i2c_MasterWrite(uint8_t byte);
    uint8_t i2c_MasterRead(uint8_t ack);
    
    void i2c_MasterByteWriteSlave(uint8_t addrDev, uint8_t data);
    uint8_t i2c_MasterByteReadSlave(uint8_t addrDev);
    

#ifdef	__cplusplus
}
#endif

#endif	/* program_H */
