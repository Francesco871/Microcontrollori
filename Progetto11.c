volatile unsigned int tempo_ms = 0;
volatile unsigned int tempo_us = 0;

#define DELAY_CLK 1000

// Lcd module connections
sbit LCD_RS at LATB4_bit;
sbit LCD_EN at LATB5_bit;
sbit LCD_D4 at LATB0_bit;
sbit LCD_D5 at LATB1_bit;
sbit LCD_D6 at LATB2_bit;
sbit LCD_D7 at LATB3_bit;

sbit LCD_RS_Direction at TRISB4_bit;
sbit LCD_EN_Direction at TRISB5_bit;
sbit LCD_D4_Direction at TRISB0_bit;
sbit LCD_D5_Direction at TRISB1_bit;
sbit LCD_D6_Direction at TRISB2_bit;
sbit LCD_D7_Direction at TRISB3_bit;
// End Lcd module connections

void main() {

	unsigned short sec_clk = 0;
	unsigned short min_clk = 0;
	unsigned short ore_clk = 0;
	char display[9] = "00:00:00";
	char temp[7];
	unsigned short confronta; //variabile confronto per polling

	ANSELC = 0b11111000; //accendo buffer ingresso per polling

	T0CON = 0b11000101; //TMR0 setup: Tof = 8,192ms (prescaler=64, No PLL)

	//isr setup
	INTCON.TMR0IE = 1;
	INTCON.TMR0IF = 0;
	INTCON.GIE = 1;

	confronta = PORTC;

	// -----------LCD Initialization---------
	Lcd_Init();		// Init the display library (This will set also the PORTB Configuration)
	
	Lcd_Cmd(_LCD_CLEAR);    	// Clear display
	Lcd_Cmd(_LCD_CURSOR_OFF);   // Cursor OFF
	// --------------------------------------

	Lcd_Out(1,5,display);

	while(1){

		if((confronta&0b00000111)!=(PORTC&0b00000111)){
			confronta=PORTC;
			if(PORTC&0b00000001)
				sec_clk++;
			else if(PORTC&0b00000010)
				min_clk++;
			else if(PORTC&0b00000100)
				ore_clk++;
		}

		if(tempo_ms>=DELAY_CLK){
			tempo_ms-=DELAY_CLK;
			sec_clk++;
			if(sec_clk>=60){
				sec_clk-=60;
				min_clk++;
			}
			if(min_clk>=60){
				min_clk-=60;
				ore_clk++;
			}
			if(ore_clk>=24)
				ore_clk=0;

			IntToStr(ore_clk,temp);
			display[1]=temp[5];
			if(temp[4]==' ')
				display[0]='0';
			else
				display[0]=temp[4];
			IntToStr(min_clk,temp);
			display[4]=temp[5];
			if(temp[4]==' ')
				display[3]='0';
			else
				display[3]=temp[4];
			IntToStr(sec_clk,temp);
			display[7]=temp[5];
			if(temp[4]==' ')
				display[6]='0';
			else
				display[6]=temp[4];
			Lcd_Out(1,5,display);
		}
	}

}

void interrupt(){
	if(INTCON.TMR0IF){
		INTCON.TMR0IF = 0;
		tempo_ms+=8;
		tempo_us+=192;
		if(tempo_us>=1000){
			tempo_ms++;
			tempo_us-=1000;
		}
	}
}