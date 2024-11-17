

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
	unsigned short confronta2; //variabile di confronto con PORTA per polling pulsante RA0
	unsigned short aspirazione_attiva = 0; //0 => spenta , 1 => accesa

	//accensione buffer
	ANSELD = 0; //accendo buffer d'ingresso per polling pulsanti
	ANSELA = 0; //accendo buffer d'ingresso per polling pulsante RA0
	TRISC.RC0 = 0; //accendo buffer uscita led d'aspirazione
	TRISE.RE2 = 0; //accendo buffer uscita per motore
	TRISC.RC7 = 0; //accendo buffer uscita per led serbatoio

	//inizializzazione uscite
	LATC.RC0 = 0; //led d'aspirazione off
	LATE.RE2 = 0; //motore off
	LATC.RC7 = 0; //led serbatoio


	//inizializzo variabili di confronto
	confronta = PORTD;
	confronta2 = PORTA;

	// -----------LCD Initialization---------
	Lcd_Init();		// Init the display library (This will set also the PORTB Configuration)
	
	Lcd_Cmd(_LCD_CLEAR);    	// Clear display
	Lcd_Cmd(_LCD_CURSOR_OFF);   // Cursor OFF
	// --------------------------------------

	Lcd_Out(1,1,"Aspirazione OFF");

	while(1){
		if((confronta2 & 0b00000001) != (PORTA & 0b00000001)){ //polling RA0
			confronta2 = PORTA; //aggiorno variabile di confronto
			if(PORTA & 0b00000001){ //se livello_serbatoio "alto"
				aspirazione_attiva = 0;
				LATC.RC7 = 1;
				Lcd_Out(1,1,"Aspirazione OFF");
				Lcd_Out(2,1,"Serbatoio Pieno");
			}else{
				LATC.RC7 = 0;
				Lcd_Out(2,1,"                ");
			}
		}
		if(((confronta & 0b00000001) != (PORTD & 0b00000001)) && (LATC.RC7 != 1)){ //polling RD0 e verifica che serbatoio "basso"
			confronta = PORTD; //aggiorno la variabile di confronto
			if(PORTD & 0b00000001){ //se premo RD0
				aspirazione_attiva = 1;
				Lcd_Out(1,1,"Aspirazione ON ");
			}else{
				aspirazione_attiva = 0;
				Lcd_Out(1,1,"Aspirazione OFF");
			}
		}
		if(aspirazione_attiva){ //uscite a seconda che l'aspirazione sia attiva o meno
			LATC.RC0 = 1;		//NB: comandi Lcd lasciati nel polling per permettere refresh più efficiente
			LATE.RE2 = 1;
		}else{
			LATC.RC0 = 0;
			LATE.RE2 = 0;
		}
	}

}