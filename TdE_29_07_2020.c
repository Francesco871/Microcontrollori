//Tde 29/07/2020
/*
Segnali d'ingresso:
	RC0 accendi/spegni
	RC3 incrementa velocità
	RC4 decrementa velocità
	RA0 sensore temperatura

Segnali d'uscita:
	RE2 driver motore
	RA4 led on/off
	RD0-7 vu meter led velocità motore
*/

volatile unsigned long int tempo_us = 0;
volatile unsigned long int tempo_ms = 0;
volatile unsigned short stato_accensione = 0; // 0 = spento , 1 = acceso

#define DELAY_SENSORE_T 1000 //1000ms = 1s 

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
	char temp[7];
	unsigned short confronta; //variabile di confronto per polling PORTC
	unsigned short velocita_motore = 0; //valori da 1 a 8
	unsigned int temperatura_mC = 0;


	ANSELC = 0; //accendo buffer ingresso C per polling pulsanti
	TRISA.RA4 = 0; //accendo buffer di uscita led on/off
//	TRISE.RE2 = 0; //accendo buffer di uscita led driver motore on/off
	TRISE.RE2 = 1; //spengo buffer di uscita driver motore per poter settare PWM
	TRISD = 0; //accendo buffer di uscita led velocità motore

	//spengo buffer RA0 per usarlo come analog (AN0) per ADC 
	TRISA.RA0 = 1;
	ANSELA.RA0 = 1;

	LATA.RA4 = 0; //led on/off inizialmente spento
//	LATE.RE2 = 0; //led driver motore inizialmente spento
	LATD = 0; //led velocità motore inizialmente spento

	//set PWM
	CCPTMRS1 = 0b00000000; //uso TMR2
	CCP5CON = 0b00001100; //set ccp in PWM mode
	T2CON = 0b00000110; //set TMR2 on, postscaler 1:1, prescaler 16
	PR2 = 7; //set periodo TMR2
	CCPR5L = 0; //inizializzo CCP5 (NB: duty_cycle = CCPR5L/(PR2+1))
	TRISE.RE2 = 0; //accendo buffer uscita driver motore per usare PWM

	//set TMR0
	T0CON = 0b11000101; //prescaler 64

	//set ADC
	ADCON2 = 0b00111110; //Tad = Fosc/64 = 8us , Tacqt = 20*Tad = 160us (>7,46us) --> Ttot = 11Tad + Tacqt = 284us (<1s)
	ADCON0 = 0b00000001;

	//start ADC conversion
	ADCON0.GO_NOT_DONE = 1;
	
	//set interrupt
	INTCON.TMR0IE = 1;
	INTCON.TMR0IF = 0;
	INTCON.GIE = 1;

	confronta = PORTC; //inizializzo variabile di confronto per polling

	// -----------LCD Initialization---------
    Lcd_Init();                // Init the display library (This will set also the PORTB Configuration)
    
    Lcd_Cmd(_LCD_CLEAR);            // Clear display
    Lcd_Cmd(_LCD_CURSOR_OFF);   // Cursor OFF
    // --------------------------------------

    Lcd_Out(1,1,"SPENTO");

    while(1){
    	if(PORTC != confronta){
    		confronta = PORTC;
    		if(PORTC & 0b00000001){
    			stato_accensione = !stato_accensione;
		    	if(stato_accensione){
		    		LATA.RA4 = 1;
//		    		LATE.RE2 = 1;
		    		velocita_motore = 1;
		    		strcpy(display,"ACCESO");
		    		Lcd_Out(1,1,display);
		    		INTCON.TMR0IE = 1;
		    	}else{
		    		LATA.RA4 = 0;
//		    		LATE.RE2 = 0;
		    		velocita_motore = 0;
					strcpy(display,"SPENTO");
		    		Lcd_Out(1,1,display);
		    		Lcd_Out(2,1,"          ");
		    		INTCON.TMR0IE = 0;
		    		tempo_us = 0;
		    		tempo_ms = 0;
		    	}
    		}else if(PORTC & 0b00001000){
    			if(velocita_motore<8 && stato_accensione)
    				velocita_motore++;
    		}else if(PORTC & 0b00010000){
    			if(velocita_motore>1 && stato_accensione)
    				velocita_motore--;
    		}
    	}
    	if(tempo_ms>=DELAY_SENSORE_T && stato_accensione){
    		tempo_ms-=DELAY_SENSORE_T;
    		temperatura_mC = (ADRESH*0.195)*8 +100;
    		strcpy(display, "  ,  C");
    		IntToStr(temperatura_mC, temp);
    		display[1] = temp[4];
            if(temp[3] != ' ')
                display[0] = temp[3];
            else
                display[0] = '0';
    		display[3] = temp[5];
    		Lcd_Out(2,1,display);
    		ADCON0.GO_NOT_DONE = 1;
    	}
    	LATD = (1<<velocita_motore)-1;
    	CCPR5L = velocita_motore;
    }

}

void interrupt(){
	if(INTCON.TMR0IF){ //Toverflow = 4/8MHz * 64 * 256 = 8,192ms
		INTCON.TMR0IF = 0;
		if(stato_accensione){
			tempo_ms += 8;
			tempo_us += 192;
			if(tempo_us>=1000){
				tempo_ms++;
				tempo_us -= 1000;
			}
		}
	}
}