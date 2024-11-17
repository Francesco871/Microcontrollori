//variabili di conteggio tempo (us=microsecondi, ms=millisecondi)
volatile unsigned int tempo_us = 0;
volatile unsigned int tempo_ms = 0;
volatile unsigned int tempo_lampeggio = 0;

#define DELAY_ADC 1000 //Temporizzazione ADC 1s = 1000ms
#define DELAY_LAMPEGGIO 200 //temporizzazione lampeggio 5Hz

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
	unsigned short confronta; //variabile di confronto con PORTD per polling pulsanti
	unsigned short aspirazione_attiva = 0; //0 => spenta , 1 => accesa
	unsigned int adc_res; //variabile dove salvare il risultato della conversione ADC a 10bit
	unsigned short percentuale_riempimento; //8bit, 0% =>vuoto , 100% =>pieno
	unsigned short flag_serbatoio = 0; //se 1 serbatoio pieno
	unsigned short lampeggio5 = 0; //se 1 devo lampeggiare a 5Hz
	char display[17] = "Riempimento   %"; //stringa per gestione seconda riga Lcd
	char temp[7]; //stringa di supporto per utilizzo IntToStr
	unsigned short turbo_attivo = 0;

	//accensione buffer
	ANSELD = 0; //accendo buffer d'ingresso per polling pulsanti
	TRISC.RC0 = 0; //accendo buffer uscita led d'aspirazione
	TRISE.RE2 = 0; //accendo buffer uscita per motore
	TRISC.RC7 = 0; //accendo buffer uscita per led serbatoio
	TRISC.RC1 = 0; //buffer uscita led turbo
	
	//spengo buffer pin RA0 per poter usare l'ADC
	ANSELA.RA0 = 1;
	TRISA.RA0 = 1;

	//inizializzazione uscite
	LATC.RC0 = 0; //led d'aspirazione off
	LATE.RE2 = 0; //motore off
	LATC.RC7 = 0; //led serbatoio
	LATC.RC1 = 0; //led turbo off

	//setup TMR0 per temporizzazione acquisizione ADC (f=1Hz => T=1s)
	T0CON = 0b11000101; //prescaler = 64 => Toverflow = 8.192ms (PLL off)

	//setup ADC
	ADCON2 = 0b00111110; //Left justified, Tad = Fosc/64 = 8us , Tacqt = 20*Tad = 160us (>7.4us)
						 //ADC impostato con conversione lenta per efficienza (comunque <T=1s tempo con cui temporizzo conversioni con TMR0)
	ADCON0 = 0b00000001; //channel AN0 => RA0

	//setup PWM
	CCPTMRS1 = 0b00000000; //CCP5 => RE2 , TMR2
	PR2 = 1;
	CCP5CON = 0b00001100;
	CCP5RL = 0;
	T2CON = 0b00000100;

	//inizializzo variabili di confronto
	confronta = PORTD;

	//interrupt setup
	INTCON.TMR0IE = 1;
	INTCON.TMR0IF = 0;
	INTCON.GIE = 1;

	//start ADC conversion
	ADCON0.GO_NOT_DONE = 1;

	// -----------LCD Initialization---------
	Lcd_Init();		// Init the display library (This will set also the PORTB Configuration)
	
	Lcd_Cmd(_LCD_CLEAR);    	// Clear display
	Lcd_Cmd(_LCD_CURSOR_OFF);   // Cursor OFF
	// --------------------------------------

	Lcd_Out(1,1,"Aspirazione OFF");

	while(1){
		if(tempo_ms>=DELAY_ADC){ //passato T=1s
			tempo_ms-=DELAY_ADC;
			adc_res = (ADRESH<<2) + (ADRESL>>6); //salvo risultato conversione a 10bit
			percentuale_riempimento = adc_res/10; //converto il risultato %
			ADCON0.GO_NOT_DONE = 1;
			lampeggio5 = 0;
			if(percentuale_riempimento>90){
				aspirazione_attiva = 0;
				flag_serbatoio = 1;
				LATC.RC7 = 1;
				Lcd_Out(1,1,"Aspirazione OFF");
				Lcd_Out(2,1,"Serbatoio Pieno");
			}else{
				flag_serbatoio = 0;
				IntToStr(percentuale_riempimento,temp);
				display[13] = temp[5];
				display[12] = temp[4];
				Lcd_Out(2,1,display);
				if(percentuale_riempimento<=50){
					LATC.RC7 = 0;
				}else if(percentuale_riempimento<=75){
					LATC.RC7 = !LATC.RC7;
				}else if(percentuale_riempimento<=90){
					lampeggio5 = 1;
				}
			}
		}
		if(tempo_lampeggio>=DELAY_LAMPEGGIO){
			tempo_lampeggio-=DELAY_LAMPEGGIO;
			if(lampeggio5){
				LATC.RC7 = !LATC.RC7;
			}
		}
		if(((confronta & 0b00000011) != (PORTD & 0b00000011)) && (!flag_serbatoio)){ //polling RD0 e verifica che serbatoio "basso"
			confronta = PORTD; //aggiorno la variabile di confronto
			if(PORTD & 0b00000001){ //se premo RD0
				aspirazione_attiva = 1;
				Lcd_Out(1,1,"Aspirazione ON ");
				if(PORTD & 0b00000010){ //se premo RD1
					turbo_attivo = !turbo_attivo;
				}
			}else{
				aspirazione_attiva = 0;
				Lcd_Out(1,1,"Aspirazione OFF");
			}
		}
		if(aspirazione_attiva){ //uscite a seconda che l'aspirazione sia attiva o meno
			LATC.RC0 = 1;		//NB: comandi Lcd lasciati nel polling per permettere refresh più efficiente
			if(turbo_attivo){
				CCP5RL = 1;
			}else{
				CCP5RL = 2;
			}
		}else{
			LATC.RC0 = 0;
			turbo_attivo = 0;
			CCP5RL = 0;
		}
		LATC.RC1 = turbo_attivo;
	}

}

void interrupt(){
	if(INTCON.TMR0IF){ //NB: Toverflow = 8.192ms
		INTCON.TMR0IF = 0;
		tempo_ms+=8;
		tempo_us+=192;
		tempo_lampeggio+=8;
		if(tempo_us>=1000){
			tempo_ms++;
			tempo_us-=1000;
		}
	}
}