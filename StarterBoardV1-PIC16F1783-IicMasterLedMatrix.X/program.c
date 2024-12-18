#include "program.h"


// Delay x1.5us
void delay_x1o5us(uint8_t delay) {
    for(uint8_t i=0; i<delay; i++) NOP();
}

// Delay x24.25us
void delay_x24o25us(uint16_t delay) {
    for(uint16_t i=0; i<delay; i++) delay_x1o5us(15);
}

// Delay x1ms
void delay_ms(uint32_t delay) {
    for(uint32_t i=0; i<delay; i++) delay_x24o25us(41);
}


void programInitialize(void) {
    TRISBbits.TRISB6 = 0;
    TRISBbits.TRISB7 = 0;
    
    led1 = 0;
    led2 = 0;
    
    TRISBbits.TRISB5 = 0;
    TRISBbits.TRISB4 = 0;
    TRISBbits.TRISB3 = 0;
    TRISBbits.TRISB2 = 0;
    TRISBbits.TRISB1 = 0;
    TRISCbits.TRISC5 = 0;
    
    ANSELBbits.ANSB5 = 0;
    ANSELBbits.ANSB4 = 0;
    ANSELBbits.ANSB3 = 0;
    ANSELBbits.ANSB2 = 0;
    ANSELBbits.ANSB1 = 0;
    
    RS_Pin = 0;
    E_Pin = 0;
    D4_Pin = 0;
    D5_Pin = 0;
    D6_Pin = 0;
    D7_Pin = 0;
    
    TRISAbits.TRISA1 = 1;
    TRISAbits.TRISA2 = 1;
    TRISAbits.TRISA3 = 1;
    TRISAbits.TRISA4 = 1;
    
    ANSELAbits.ANSA1 = 0;
    ANSELAbits.ANSA2 = 0;
    ANSELAbits.ANSA3 = 0;
    ANSELAbits.ANSA4 = 0;
    
    // SCl pin
    TRISCbits.TRISC3 = 1;
    APFCONbits.SCKSEL = 0;
    
    // SDa pin
    TRISCbits.TRISC4 = 1;
    APFCONbits.SDISEL = 0;
    
    lcd_Initialize();
    
    lcd_Goto(0, 0);
    lcd_PrintString("I2C Master");
    lcd_Goto(1, 0);
    lcd_PrintDigitInt32(0, 2, false, true);
    
    i2c_Initialize(_User_FOSC, 600); // Set bit rate x9bit/baud = 5.4kHz
}

void programLoop(void) {
    // Alternately change LEDs pattern on slave boards
    
    led1 = 1;
    i2c_MasterByteWriteSlave(_Address_LMSB1, 0xAA); // Write data to LEDs matrix device 1
    i2c_MasterByteWriteSlave(_Address_LMSB2, 0x0F); // Write data to LEDs matrix device 2
    delay_ms(200);
    
    led1 = 0;
    lcd_Goto(1, 0);
    lcd_PrintDigitInt32(i2c_MasterByteReadSlave(_Address_LMSB1), 2, false, true); // Read switches data from device 1 and write to LCD
    lcd_Goto(1, 4);
    lcd_PrintDigitInt32(i2c_MasterByteReadSlave(_Address_LMSB2), 2, false, true); // Read switches data from device 2 and write to LCD
    delay_ms(200);
    
    led1 = 1;
    i2c_MasterByteWriteSlave(_Address_LMSB1, 0x55);
    i2c_MasterByteWriteSlave(_Address_LMSB2, 0xF0);
    delay_ms(200);
    
    led1 = 0;
    lcd_Goto(1, 0);
    lcd_PrintDigitInt32(i2c_MasterByteReadSlave(_Address_LMSB1), 2, false, true);
    lcd_Goto(1, 4);
    lcd_PrintDigitInt32(i2c_MasterByteReadSlave(_Address_LMSB2), 2, false, true);
    delay_ms(200);
}


// Delay to debounce mechanical noise
void pb_DelayDebounce(void) {
    delay_ms(200);
}

void lcd_DelaySetupTime(void) {
    // China TGK LCD delay
    delay_x1o5us(200);
}

void lcd_DelayPulse(void) {
    // China TGK LCD delay
    delay_x1o5us(200);
}

void lcd_EPulse(void) {
    E_Pin = 1;
    lcd_DelayPulse();
    
    E_Pin = 0;
    lcd_DelayPulse();
}

void lcd_WriteData(uint8_t data) {
    // Send upper nibble data
    D7_Pin = (data >> 7) & 0x01;
    D6_Pin = (data >> 6) & 0x01;
    D5_Pin = (data >> 5) & 0x01;
    D4_Pin = (data >> 4) & 0x01;
    
    lcd_EPulse();
    
    // Send lower nibble data
    D7_Pin = (data >> 3) & 0x01;
    D6_Pin = (data >> 2) & 0x01;
    D5_Pin = (data >> 1) & 0x01;
    D4_Pin = data & 0x01;
    
    lcd_EPulse();
    
    delay_x1o5us(30);   // Execution time for instruction >37us - Page 24
                        // delay = 37us / 1.5us = 25
}

void lcd_PrintCmd(uint8_t command) {
    RS_Pin = 0;
    lcd_DelaySetupTime();
    
    lcd_WriteData(command);
}

void lcd_Initialize(void) {
    delay_ms(20);           // Wait for LCD power supply rise time >10ms - Datasheet page 50
    
    // China TGK LCD reset process
    lcd_PrintCmd(0x33);
    lcd_PrintCmd(0x32);
    
    // LCD command - Datasheet page 24
    lcd_PrintCmd(0x28);     // Set LCD to 4-bit mode
    lcd_PrintCmd(0x02);     // Set DDRAM address counter to 0
    lcd_PrintCmd(0x0C);     // Display is set ON, cursor is set OFF, cursor blink is set OFF
    lcd_PrintCmd(0x06);     // Cursor is set to shift right
    lcd_PrintCmd(0x80);     // Set cursor back to home
    lcd_PrintCmd(0x01);     // Clear entire display
    
    delay_x24o25us(65);     // Execution time to clear display instruction, lcd_PrintCmd(0x01) >1.52ms,
                            // delay = 1.52ms / 24.25us = 63
}

void lcd_ClearAll(void) {
    lcd_PrintCmd(0x02);
    lcd_PrintCmd(0x01);
    delay_x24o25us(65);
}

void lcd_Goto(uint8_t y, uint8_t x) {
    switch(y) {
        case 0:
            lcd_PrintCmd(0x80 + x);
            break;
            
        case 1:
            lcd_PrintCmd(0xC0 + x);
            break;
            
        default:
            lcd_PrintCmd(0x80 + x);
            break;
    }
}

void lcd_PrintChar(char character) {
    RS_Pin = 1;
    lcd_DelaySetupTime();
    
    lcd_WriteData(character);
}

void lcd_PrintString(char *string) {
    while(*string!=0) {
        lcd_PrintChar(*string);
        string++;
    }
}

void lcd_PrintInt32(int32_t number) {
    uint8_t i1 = 0,
            i2 = 0,
            totalDigit = 0;
    
    char numberRevChar[11];
    char numberChar[11];
    
    memset(numberRevChar, 0, 11);
    memset(numberChar, 0, 11);
    
    if(number<0) {
        lcd_PrintChar('-');
        number = labs(number);
    }
    
    do {
        int32_t tempN = number;
        number /= 10;
        char tempC = (char)(tempN -10 * number);
        numberRevChar[i1] = tempC + 48;
        i1++;
    } while(number);
    
    totalDigit = i1;
    
    for(i1=totalDigit, i2=0; i1>0; i1--, i2++) {
        numberChar[i2] = numberRevChar[i1-1];
    }
    
    lcd_PrintString(numberChar);
}

void lcd_PrintDigitInt32(int32_t number, uint8_t noDigit, bool enSign, bool enZero) {
    uint8_t i1 = 0,
            i2 = 0,
            totalDigit = 0;
    
    char numberRevChar[11];
    char numberChar[11];
    
    memset(numberRevChar, 0, 11);
    memset(numberChar, 0, 11);
    
    if(number<0) {
        if(enSign) lcd_PrintChar('-');
        number = labs(number);
    } else {
        if(enSign) lcd_PrintChar(' ');
    }
    
    do {
        int32_t tempN = number;
        number /= 10;
        char tempC = (char)(tempN -10 * number);
        numberRevChar[i1] = tempC + 48;
        i1++;
    } while(number);
    
    totalDigit = i1;
    
    for(i1=0; i1<(noDigit-totalDigit); i1++) {
        if(enZero) lcd_PrintChar('0');
        else lcd_PrintChar(' ');
    }
    
    for(i1=totalDigit, i2=0; i1>0; i1--, i2++) {
        numberChar[i2] = numberRevChar[i1-1];
    }
    
    lcd_PrintString(numberChar);
}

void i2c_Initialize(uint32_t fosc, uint16_t baudrate) {
    SSPSTATbits.SMP = 1; // Set disabled slew rate control for standard speed mode - Page 304
	
    SSPCON1bits.SSPM = 8; // Set MSSP module for I2C master mode with clock = FOSC/(4*(SSPADD+1)) - Page 306
    
    SSPCON3bits.SDAHT = 0; // Set 100ns for minimum SDA hold time - Page 308
    
    if(baudrate>2223) baudrate = 2223; // Set maximum baud rate to 2223 - baud rate tested and worked below 2224
    else if(baudrate<1113) baudrate = 1113; // Set minimum baud rate to 1113 - baud rate tested and worked above 1112
    SSPADD = (uint8_t)((fosc/baudrate) / 4 - 1); // Set baud rate generator register - Page 309
	
    SSPCON1bits.SSPEN = 1; // Set enable MSSP module - Page 306
    
    delay_x1o5us(2); // Wait for MSSP module configuration - Page 373 from I/O pin timing
                     // Tioz = 2us
}

void i2c_BusIdle(void) { // Wait for I2C bus to be idle
    while(SSPCON2 & 0b00011111); // Polling to hold program when one of SSPCON2 bit from 4th to 0th is 1 - Page 307
                                 // The registers are ACKEN, RCEN, PEN, RSEN or SEN.
}

void i2c_MasterStart(void) { // Initiate start condition to bus
    i2c_BusIdle();
    
    SSPCON2bits.SEN = 1; // Set to initiate start condition - Page 307
    NOP(); // Wait for start condition setup time - Page 385
    NOP(); // Tdelay = THD:STA, minimum high speed
    NOP(); // Tdelay = 600ns / 0.125us = 4.8
    NOP();
    NOP();
}

void i2c_MasterRstart(void) { // Initiate repeat start condition to bus
    i2c_BusIdle();
    
    SSPCON2bits.RSEN = 1; // Set to initiate repeat start condition - Page 307
    NOP(); // Wait for start condition setup time - Page 385
    NOP(); // Tdelay = THD:STA, minimum high speed
    NOP(); // Tdelay = 600ns / 0.125us = 4.8
    NOP();
    NOP();
}

void i2c_MasterStop(void) { // Initiate stop condition to bus
    i2c_BusIdle();
    
    SSPCON2bits.PEN = 1; // Set to initiate stop condition - Page 307
    
    delay_x1o5us(7); // Wait for stop condition setup time - Page 385 and 386
                     // Tdelay = TR + TSU:STO + TBUF
                     // delay = Tdelay / 1.5us = (1000ns + 4700ns + 4.7uf) / 1.5uf = 6.9
                     // Value in datasheet not tested note as * in page 386
    
    delay_x24o25us(60); // Additional time the bus must be free before a new transmission can start - tested
}

void i2c_MasterWrite(uint8_t byte) { // Write a byte to bus
    i2c_BusIdle();
    
    SSPBUF = byte;          // Write to MSSP register to transmit data - Page 291
    
    while(SSPSTATbits.BF);  // Polling to hold program while MSSP module transmit not complete  - Page 293
}

uint8_t i2c_MasterRead(uint8_t ack) { // Read a byte from bus
    uint8_t data = 0;        // Declare a variable to read MSSP receive register
    
    i2c_BusIdle();
    
    SSPCON2bits.RCEN = 1;    // Set enable MSSP receive mode - Page 307
    
    while(!SSPSTATbits.BF);  // Polling to hold program while MSSP module receive not complete - Page 293
    
    data = SSPBUF;           // Read MSSP receive register - Page 305
    
    SSPCON2bits.ACKEN = ack; // Set variable ack value to acknowledge sequence - Page 307
    
    return data;
}

void i2c_MasterByteWriteSlave(uint8_t addrDev, uint8_t data) {
    i2c_MasterStart();
    
    i2c_MasterWrite((uint8_t)(addrDev << 1)); // Device write address
    
    i2c_MasterWrite(data); // Write data to slave
    
    i2c_MasterStop();
}

uint8_t i2c_MasterByteReadSlave(uint8_t addrDev) {
    uint8_t data = 0;
    
    i2c_MasterStart();
    
    i2c_MasterWrite((uint8_t)((addrDev << 1) | 1)); // Device read address
    
    data = i2c_MasterRead(0); // Read data from slave
    
    i2c_MasterStop();
    
    return data;
}
