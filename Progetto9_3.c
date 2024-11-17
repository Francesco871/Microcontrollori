//STEP SKILL BOOST: uso tmr1 per far scorrere kittcar su LATD e per far acquisizione ADC ogni secondo

volatile unsigned int dist=0;
volatile unsigned int ta=0;
volatile unsigned int tb=0;
volatile unsigned short end_capt=0;

volatile unsigned long int tempo_us = 0;
volatile unsigned long int tempo_ms = 0;

#define DELAY_MS 1000 //1000ms = 1s

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

void main ()
{
    unsigned int confronta = 0;
    char display[8];
    unsigned int t=0;
    short dir = 1; /*dir = 1 riempiamo, dir = 0 svuotiamo*/

    TRISC.RC2 = 1;
    ANSELC.RC2 = 0;

    TRISC.RC3 = 1;
    ANSELC.RC3 = 1;

    TRISC.RC6 = 0;
    ANSELC.RC6 = 1;

    TRISD = 0;
    LATD = 0;

    ADCON0 = 0b00111101;             //an15 = rc3
    ADCON2 = 0b00100001;             //left justified , TAD = 8TOSC = 1us , TACQT = 8 TAD = 8us

    //CCP 
    CCP1CON = 0b00000101;   //capture mode , every rising edge

    //timer 1 as source
    CCPTMRS0.C1TSEL0=0;     
    CCPTMRS0.C1TSEL1=0;

    //fosc/4 as source
    T1CON.TMR1CS0= 0;     
    T1CON.TMR1CS1= 0;
	//prescaler 2
    T1CON.T1CKPS0=1;
    T1CON.T1CKPS1=0;
    //T= 4/FCLK * PRESCALER = 1us
    T1CON.T1RD16=1; //16bit mode
    T1CON.TMR1ON=1; //tmr1 on

    INTCON.PEIE = 1;

    PIE1.ADIE=1;
    PIR1.ADIF=0;

    PIE1.CCP1IE=1;
    PIR1.CCP1IF=0;

    //interrupt TMR1
    PIE1.TMR1IE = 1;
    PIR1.TMR1IF = 0;

    //LCD
    Lcd_Init();     // Init the display library (This will set also the PORTB Configuration)
    
    Lcd_Cmd(_LCD_CLEAR);        // Clear display
    Lcd_Cmd(_LCD_CURSOR_OFF);   // Cursor OFF

    INTCON.GIE=1;
    ADCON0.GO_NOT_DONE = 1;

    LATC.RC6=1;  //freerun
    while(1)
    {
    	if(confronta!=dist){
    		confronta = dist;
        	intToStr(dist*5,display); //LSB 5mV
        	strcat(display, " mm");
        	Lcd_out(1,1,display+2);
        }

        if(end_capt)
        {
            end_capt=0;
            t = tb-ta; //NB: il compilatore casta automaticamente ta a cpl2 per fare la differenza => non ho problemi se ta>tb
            intToStr(t,display);
        	strcat(display, " mm");
        	Lcd_out(2,1,display+2);
        }
       if(tempo_ms>=DELAY_MS){
       	tempo_ms-=DELAY_MS;
           if(dir){
               if(!LATD){
                   LATD = 0x81;
               }else{
                   LATD = (((LATD & 0xf0) >> 1) + ((LATD & 0x0f) << 1)) + 0x81;
               }
           }else{
               LATD = (((LATD & 0xf0) << 1) + ((LATD & 0x0f) >> 1));
           }
           if(LATD == 0x00){
               dir = 1;
           }else if(LATD == 0xff){
               dir = 0;
           }
       	ADCON0.GO_NOT_DONE = 1;
       }
    }
}

void interrupt()
{
    if(PIR1.CCP1IF)
    {
        PIR1.CCP1IF=0;

        if(CCP1CON.CCP1M0) //if 1 = rising , 0 = falling
            ta=(CCPR1H<<8)+CCPR1L;
        else
        {
            tb=(CCPR1H<<8)+CCPR1L;
            end_capt=1;
        }
            
        CCP1CON.CCP1M0=!CCP1CON.CCP1M0; //flip from rising to falling
    }
    if(PIR1.TMR1IF){ //T_overflowTMR1 = 4/Tclk * prescaler * 2^16 = 4/8MHz*2 * 2^16 = 1us * 2^16 = 65536us
    	PIR1.TMR1IF = 0;
    	tempo_ms += 65;
    	tempo_us += 536;
        if(tempo_us>=1000){
            tempo_ms++;
            tempo_us-=1000;
        }
    }
    if(PIR1.ADIF)
    {
        PIR1.ADIF = 0;
        dist= (ADRESH<<2) + (ADRESL>>6);
        ADCON0.GO_NOT_DONE = 1;
    }
}