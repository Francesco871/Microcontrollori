volatile int adc_10bit;
volatile unsigned int dist=0;
volatile short adc_conv = 0; //se adc_conv == 1 --> conversione finita
volatile signed short int adc_flag = -1; //se flag==0 potenziometro (AN0) se flag==1 sonar (AN15)

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

unsigned short int binaryToThermo (unsigned short int); //prototipo funzione sezione C

void main() {
	unsigned short int count = 0;
	unsigned short confronta; //confronto PORTA per polling
	char display[17] = "0";
	char temp[7];

	ANSELA = 0;

	//spengo buffer RA0 per usarlo come analog (AN0) per ADC 
	TRISA.RA0 = 1;
	ANSELA.RA0 = 1;

	//sonar AVO
	TRISC.RC3 = 1;
    ANSELC.RC3 = 1;

    //sonar freerun
	LATC.RC6=1;

	//disabilito buffer RE2 per PWM
	TRISE.RE2 = 1;

	//accendo buffer uscita led D per visualizzare thermo sui led
	TRISD = 0;
	LATD = 0;

	//set ADC: left justified , Tad = Fosc/16 = 2us , Tacqt = 4Tad = 8us > 7.45us
	ADCON2 = 0b00010101;
	ADCON0 = 0b00000001;

	//inizializzo variabile confronto per polling
	confronta = PORTA;

	//set PWM
	//scelgo TMR6 per il CCP5
	CCPTMRS1.C5TSEL1 = 1;
	CCPTMRS1.C5TSEL0 = 0;
	PR6 = 255; //set periodo di TMR6
	CCP5CON = 0b00001100; //set CCP come PWM
	CCPR5L = 0; //imposto CCP5 NB: duty_cycle = CCPR5L /(PR2 +1)
	T6CON = 0b00000111; //set TMR6: postscaler 1:1 , prescaler 16
	TRISE.RE2 = 0; //buffer output PWM

	//start ADC conversion
	ADCON0.GO_NOT_DONE = 1;

	//set interrupt
	PIE1.ADIE = 1;
	PIR1.ADIF = 0;

	INTCON.PEIE = 1; //set peripheral interrupt
	INTCON.GIE = 1; //set global interrupt

	// -----------LCD Initialization---------
    Lcd_Init();                // Init the display library (This will set also the PORTB Configuration)
    
    Lcd_Cmd(_LCD_CLEAR);            // Clear display
    Lcd_Cmd(_LCD_CURSOR_OFF);   // Cursor OFF
    // --------------------------------------

    Lcd_Out(1,1,"Conteggio=   0");

	while(1){
		if((PORTA&0b00001110) != (confronta&0b00001110)){ //Polling
			confronta = PORTA;
			if(PORTA & 0b00001000){ //premo RA3 oppure PORTA.RA3
				if(count<255)
					count++;
			}else if(PORTA & 0b00000100){ //premo RA2
				count = 0;
			}else if(PORTA & 0b00000010){ //premo RA1
				if(count>0)
					count--;
			}
			strcpy(display,"Conteggio=");
			IntToStr(count, temp);
			strcat(display,temp+2);
			Lcd_Out(1,1,display);
			LATD = binaryToThermo(count);
		}
		if(adc_conv){
			adc_conv = 0;
			if(adc_flag==0){
				strcpy(display,"RA0[mV]=");
				IntToStr(adc_10bit*5,temp);
				strcat(display,temp);
				Lcd_Out(2,1,display);
				ADCON0 = 0b00111101; //cambio canale su AN15
			}else if(adc_flag==1){
				CCPR5L=dist;
				ADCON0 = 0b00000001; //cambio canale su AN0
				//LATD = dist; il debug torna: distanza minima: led 0b00001111 == 15 , 15*20 = 300mm giusto! (NB LSB=2)
			}
			ADCON0.GO_NOT_DONE = 1;
		}
	}

}

void interrupt(){
	if(PIR1.ADIF){
		PIR1.ADIF = 0;
		if(ADCON0.CHS0 == 0){ //channel sull'AN0
			adc_10bit = (ADRESH<<2) + (ADRESL>>6);
			adc_flag = 0;
		}else{
			dist = ADRESH;
			adc_flag = 1;
		}
		adc_conv = 1;
	}
}

unsigned short int binaryToThermo (unsigned short int binary) {
    unsigned short int thermo;
    thermo = (1<<((binary/32)+1))-1;
    return thermo;
}