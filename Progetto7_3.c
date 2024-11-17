volatile unsigned short int adc_8bit;   //ADRESH
volatile int adc_10bit;                 //ADRESH*4 + ADSREL/64 => (ADRESH<<2) + (ADSREL >> 6)
volatile unsigned short int output_adc; //lo uso per l'output sui LED
volatile signed short int flag = -1;

#define NCHAR 7

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

void main()
{
	char display[17];
	char temp[7];

    // ----- Output ------
    TRISC = 0;
    TRISD = 0;

    // -- Set CHS (AN0) --
    ADCON0.CHS0 = 0;
    ADCON0.CHS1 = 0;
    ADCON0.CHS2 = 0;
    ADCON0.CHS3 = 0;
    ADCON0.CHS4 = 0;

    //Fosc = 8MHz TAD 
    ADCON2.ADCS0 = 1;
    ADCON2.ADCS1 = 0;
    ADCON2.ADCS2 = 0;

    // --- Set ACQT -----
    ADCON2.ACQT0 = 0;
    ADCON2.ACQT1 = 0;
    ADCON2.ACQT2 = 1;

    // --- Just. Left ---
    ADCON2.ADFM = 0;

    // ----- ADC ON -------
    ADCON0.ADON = 1;

    // Start Conv.
    ADCON0.GO_NOT_DONE = 1;
    
    //Accensione interrupt dell'ADC
    PIE1.ADIE   = 1;
    PIR1.ADIF   = 0;
    //Accensione interrupt in INTCON (ADC e GLOBAL)
    INTCON.PEIE = 1;
    INTCON.GIE  = 1;

        // -----------LCD Initialization---------
    Lcd_Init();     // Init the display library (This will set also the PORTB Configuration)
    
    Lcd_Cmd(_LCD_CLEAR);        // Clear display
    Lcd_Cmd(_LCD_CURSOR_OFF);   // Cursor OFF
    // --------------------------------------

    while(1){
        if(flag == 0){
            LATC = output_adc;
            strcpy(display,"RA0[mV]=");
            IntToStr(adc_10bit * 5, temp);
            strcat(display,temp);
            Lcd_Out(1,1,display);
            flag = -1;
            ADCON0.CHS0 = 1;
            ADCON0.GO_NOT_DONE = 1;

        }else if(flag == 1){
            LATD = output_adc;
            strcpy(display,"RA1[mV]=");
            IntToStr(adc_8bit * 20, temp);
            strcat(display,temp);
            Lcd_Out(2,1,display);
            flag = -1;
            ADCON0.CHS0 = 0;
            ADCON0.GO_NOT_DONE = 1;
        }
    }
    
}

void interrupt()
{
    if(PIR1.ADIF)
    {
        PIR1.ADIF = 0;
        if(ADCON0.CHS0 == 0){
            adc_10bit = (ADRESH<<2) + (ADRESL >> 6);
            output_adc = ADRESH;
            flag = 0;
        }else{
            adc_8bit = ADRESH;
            output_adc = ADRESH;
            flag = 1;
        }
    }
}