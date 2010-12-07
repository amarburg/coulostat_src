//********************************************************************
//
//				LCD_driver.c: Interface for Nokia LCD
//
//********************************************************************
#include "LCD_driver.h"
#include "LPC214x.h"
#include "stdlib.h"
#include "rprintf.h"

//********************************************************************
//
//				 Function Definitions
//
//********************************************************************
void LCDSetPixel(int color, unsigned char x, unsigned char y);
void LCDClear(int color);
void LCDData(unsigned char data);
void LCDInit(void);
void LCDSetPixel(int color, unsigned char x, unsigned char y);

//********************************************************************
//
//				Global Variables for LCD driver
//
//********************************************************************
//Usage: LCDClear(black);
//Inputs: char color: 8-bit color to be sent to the screen.
//Outputs: None
//Description: This function will clear the screen with "color" by writing the
//			   color to each location in the RAM of the LCD.
void LCDClear(int color)
{
	#ifdef EPSON
		LCDCommand(PASET);
		LCDData(0);
		LCDData(131);
	
		LCDCommand(CASET);
		LCDData(0);
		LCDData(131);

		LCDCommand(RAMWR);
	#endif
	#ifdef	PHILLIPS
		LCDCommand(PASETP);
		LCDData(0);
		LCDData(131);
	
		LCDCommand(CASETP);
		LCDData(0);
		LCDData(131);

		LCDCommand(RAMWRP);
	#endif
	for(int i=0; i < (131*131)/2; i++)
	{
		LCDData((color>>4)&0x00FF);
		LCDData(((color&0x0F)<<4)|(color>>8));
		LCDData(color&0x0FF);
	}
}

//Usage: LCDCommand(RAMWR);
//Inputs: char data - character command to be sent to the LCD
//Outputs: None
//Description: Sends a 9 bit command over SPI to the LCD with
//			   the first bit HIGH indicating "command" mode.
void LCDCommand(unsigned char data)
{

	IODIR0 |= (LCD_DIO | LCD_SCK | LCD_CS | LCD_RES);		//Assign LCD pins as Outputs
	
	IOCLR0 = LCD_CS;      // enable chip, p0.20 goes low
    IOCLR0 = LCD_DIO;     // output low on data out (9th bit low = command), p0.19

    IOCLR0 = LCD_SCK;      // send clock pulse
    IOSET0 = LCD_SCK;

    for (char j = 0; j < 8; j++)
    {
        if ((data & 0x80) == 0x80) IOSET0 = LCD_DIO;
        else IOCLR0 = LCD_DIO;

        IOCLR0 = LCD_SCK;   // send clock pulse
        IOSET0 = LCD_SCK;

        data <<= 1;
    }

    IOSET0 = LCD_CS;    		// disable

}

//Usage: LCDData(RAMWR);
//Inputs: char data - character data to be sent to the LCD
//Outputs: None
//Description: Sends a 9 bit command over SPI to the LCD with
//			   the first bit LOW indicating "data" mode.
void LCDData(unsigned char data)
{

	IODIR0 |= (LCD_DIO | LCD_SCK | LCD_CS | LCD_RES);		//Assign LCD pins as Outputs
	
	IOCLR0 = LCD_SCK;      // send clock pulse
	IOCLR0 = LCD_CS;      // enable chip, p0.20 goes low    
	IOSET0 = LCD_DIO;    // output high on data out (9th bit high = data), p0.19
    IOCLR0 = LCD_SCK;    // send clock pulse
    IOSET0 = LCD_SCK;
	for (char j = 0; j < 8; j++)
    {
        if ((data & 0x80) == 0x80) IOSET0 = LCD_DIO;
        else IOCLR0 = LCD_DIO;
        IOCLR0 = LCD_SCK;   // send clock pulse
        IOSET0 = LCD_SCK;
        data <<= 1;
    }

    IOSET0 = LCD_CS;     		// disable
}

//Usage: LCDInit();
//Inputs: None
//Outputs: None
//Description:  Initializes the LCD regardless of if the controlller is an EPSON or PHILLIPS.
void LCDInit(void)
{
    int j;	

	delay_ms(200);
	
    // reset display
	IODIR0 |= (LCD_DIO | LCD_SCK | LCD_CS | LCD_RES);		//Assign LCD pins as Outputs	
	
    IOCLR0 = (LCD_SCK | LCD_DIO);							//output_low (SPI_CLK);//output_low (SPI_DO);
    IOSET0 = LCD_CS;				//output_high (LCD_CS);
    for (j = 0; j < 16; j++)asm volatile ("nop");
    IOCLR0 = LCD_RES;				//output_low (LCD_RESET);
    for (j = 0; j < 300000; j++)asm volatile ("nop");
    IOSET0 = LCD_RES;				//output_high (LCD_RESET);
    IOSET0 = (LCD_SCK | LCD_DIO);
    for (j = 0; j < 300000; j++)asm volatile ("nop");	//delay_ms(100);
	
    LCDCommand(DISCTL);  	// display control(EPSON)
    LCDData(0x0C);   		// 12 = 1100 - CL dividing ratio [don't divide] switching period 8H (default)
	LCDData(0x20);    
	LCDData(0x00);
	
	LCDData(0x01);
	
    LCDCommand(COMSCN);  	// common scanning direction(EPSON)
    LCDData(0x01);
    
    LCDCommand(OSCON);  	// internal oscialltor ON(EPSON)
	
    LCDCommand(SLPOUT);  	// sleep out(EPSON)
	LCDCommand(SLEEPOUT);	//sleep out(PHILLIPS)
    
    LCDCommand(PWRCTR); 	// power ctrl(EPSON)
    LCDData(0x0F);    		//everything on, no external reference resistors
    LCDCommand(BSTRON);		//Booset On(PHILLIPS)
	
	LCDCommand(DISINV);  	// invert display mode(EPSON)
	LCDCommand(INVON);		// invert display mode(PHILLIPS)
    
    LCDCommand(DATCTL);  	// data control(EPSON)
    LCDData(0x03);			//correct for normal sin7
	LCDData(0x00);   		// normal RGB arrangement
	LCDData(0x02);			// 16-bit Grayscale Type A
	
	LCDCommand(MADCTL);		//Memory Access Control(PHILLIPS)
	LCDData(0xC8);
	
	LCDCommand(COLMOD);		//Set Color Mode(PHILLIPS)
	LCDData(0x02);	
	
    LCDCommand(VOLCTR);  	// electronic volume, this is the contrast/brightness(EPSON)
	LCDData(0x24);   		// volume (contrast) setting - fine tuning, original
    LCDData(0x03);   		// internal resistor ratio - coarse adjustment
	LCDCommand(SETCON);		//Set Contrast(PHILLIPS)
	LCDData(0x30);	
	   
    LCDCommand(NOP);  	// nop(EPSON)
	LCDCommand(NOPP);		// nop(PHILLIPS)

    LCDCommand(DISON);   	// display on(EPSON)
	LCDCommand(DISPON);	// display on(PHILLIPS)
}

//Usage: LCDSetPixel(white, 0, 0);
//Inputs: unsigned char color - desired color of the pixel
//		  unsigned char x - Page address of pixel to be colored
//		  unsigned char y - column address of pixel to be colored
//Outputs: None
//Description: Sets the starting page(row) and column (x & y) coordinates in ram,
//  		   then writes the colour to display memory.  The ending x & y are left
//  		   maxed out so one can continue sending colour data bytes to the 'open'
//  		   RAMWR command to fill further memory.  issuing any red command
//  		   finishes RAMWR.
void LCDSetPixel(int color, unsigned char x, unsigned char y)
{
	#ifdef EPSON
		LCDCommand(PASET);   // page start/end ram
		LCDData(x);
		LCDData(ENDPAGE);
  
		LCDCommand(CASET);   // column start/end ram
		LCDData(y);
		LCDData(ENDCOL);
  
		LCDCommand(RAMWR);    // write
		LCDData((color>>4)&0x00FF);
		LCDData(((color&0x0F)<<4)|(color>>8));
		LCDData(color&0x0FF);  	// nop(EPSON)		
	#endif
	#ifdef	PHILLIPS
		LCDCommand(PASETP);   // page start/end ram
		LCDData(x);
		LCDData(ENDPAGE);
  
		LCDCommand(CASETP);   // column start/end ram
		LCDData(y);
		LCDData(ENDCOL);
  
		LCDCommand(RAMWRP);    // write
		
		LCDData((unsigned char)((color>>4)&0x00FF));
		LCDData((unsigned char)(((color&0x0F)<<4)|0x00));
	#endif
}


