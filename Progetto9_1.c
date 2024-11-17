/*
 Step 1: lettura ao con adc 1mv=1mm isr, , out su lcd   RC3
 
 Step 2: lettura pwo 1mm=1us
 CCP1CON select rising o falling TIMER1  RC2
 
 RC6 START
*/

volatile unsigned int dist=0;

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

    //ADC
    TRISC.RC3 = 1;
    ANSELC.RC3 = 1;

    ADCON0 = 0b00111101;             //an15 = rc3
    ADCON2 = 0b00100001;             //left justified , TAD = 8TOSC = 1us , TACQT = 8 TAD = 8us

    INTCON.PEIE = 1;
    PIE1.ADIE=1;
    PIR1.ADIF=0;

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
    }
}

void interrupt()
{
    if(PIR1.ADIF)
    {
        PIR1.ADIF = 0;
        dist= (ADRESH<<2) + (ADRESL>>6);
        ADCON0.GO_NOT_DONE = 1;
    }
}
