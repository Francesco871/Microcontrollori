volatile unsigned int tempo_ms = 0;
volatile unsigned int tempo_us = 0;

#define DELAY_ADC 1000 //ms = 1s

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
	char display[17];
	char display2[17] = "set: 25  t: "
	char temp[7];
	unsigned short confronta; //variabile di confronto per polling PORTC
	unsigned short stato_accensione = 0; // 0 = spento , 1 = acceso
	signed short setpoint_temperatura = 25;
	signed short temperatura;
	signed short scarto;

	ANSELC = 0; //accendo buffer ingresso C per polling pulsanti

	//led on/off
	TRISC.RC7 = 0;
	LATC.RC7 = 0;

	//led motore on/off
	TRISE.RE2 = 0;
	LATE.RE2 = 0;

	//spengo buffer per adc
	TRISA.RA0 = 1;
	ANSELA.RA0 = 1;

	//tmr0 setup prescaler=64 Tof=8,192ms
	T0CON = 0b11000101

	//adc setup Tad=Fosc/8=1us , Tacqt=8Tad=8us>7,4us
	ADCON2 = 0b00100001;
	ADCON0 = 0b00000001;
	ADCON0.GO_NOT_DONE = 1; //start conv

	//isr setup
	INTCON.TMR0IE = 1;
	INTCON.TMR0IF = 0;
	INTCON.GIE = 1;

	confronta = PORTC; //inizializzo variabile di confronto per polling

	// -----------LCD Initialization---------
	Lcd_Init();		// Init the display library (This will set also the PORTB Configuration)
	
	Lcd_Cmd(_LCD_CLEAR);    	// Clear display
	Lcd_Cmd(_LCD_CURSOR_OFF);   // Cursor OFF
	// --------------------------------------

	Lcd_Out(1,1,"SPENTO");
	Lcd_Out(2,1,display2);

    while(1){
    	if((PORTC&0b00011001) != (confronta&0b00011001)){
    		confronta = PORTC;
    		if(PORTC & 0b00000001){
    			stato_accensione = !stato_accensione;
		    	if(stato_accensione){
		    		strcpy(display,"ACCESO");
		    		Lcd_Out(1,1,display);
		    		LATC.RC7 = 1;
		    	}else{
					strcpy(display,"SPENTO");
		    		Lcd_Out(1,1,display);
		    		LATC.RC7 = 0;
		    		LATE.RE2 = 0;
		    	}
		    }else if((PORTC & 0b00001000) && stato_accensione){
		    	setpoint_temperatura++;
		    	IntToStr(setpoint_temperatura,temp);
		    	display2[5] = temp[5];
		    	display2[4] = temp[4];
		    	Lcd_Out(2,1,display2);
		    }else if((PORTC & 0b00010000) && stato_accensione){
		    	setpoint_temperatura--;
		    	IntToStr(setpoint_temperatura,temp);
		    	display2[5] = temp[5];
		    	display2[4] = temp[4];
		    	Lcd_Out(2,1,display2);
		    }
    	}
    	if(tempo_ms>=DELAY_ADC){
    		tempo_ms-=DELAY_ADC;
    		if(stato_accensione){
    			temperatura = (ADRESH*0.195)*8 +100;
    			if(setpoint_temperatura>=temperatura)
    				scarto = setpoint_temperatura - temperatura;
    			else
    				scarto = temperatura - setpoint_temperatura;
    			if(scarto>1)
    				LATE.RE2 = 1;
    			else
    				LATE.RE2 = 0;

    		}
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