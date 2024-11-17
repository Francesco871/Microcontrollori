/*KittCar 2.0 dinamic delay e stopwatch*/

volatile unsigned int tempo = 0;

#define DELAY 1000
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

void main() {

    short dir = 1; /*dir = 1 riempiamo, dir = 0 svuotiamo*/
    unsigned short counter = 0;
    unsigned short confronta;
    char display[NCHAR] = "     0";
    unsigned delay = DELAY;
    unsigned short status_watch = 0;

    ANSELA = 0;
    confronta = PORTA;
    
    TRISD = 0;
    LATD = 0;
    TRISC = 0;
    LATC = 0;

    T0CON   = 0b11000111;
    INTCON  = 0b10100000;

        // -----------LCD Initialization---------
    Lcd_Init();     // Init the display library (This will set also the PORTB Configuration)
    
    Lcd_Cmd(_LCD_CLEAR);        // Clear display
    Lcd_Cmd(_LCD_CURSOR_OFF);   // Cursor OFF
    // --------------------------------------
    
    Lcd_Out(1,1,display);

    while(1){
        LATC = delay/100;
        if((PORTA & 0x1f) != (confronta & 0x1f)){
            confronta = PORTA;
            if(PORTA & 0b00001000){
                if(delay >= 1000)
                    delay=1000;
                else{
                    delay+=100;
                }
            }else if(PORTA & 0b00010000){
                if(delay <= 100)
                    delay=100;
                else{
                    delay-=100;
                }
            }else if(PORTA & 0b00000001){
                status_watch = 1;
            }else if(PORTA & 0b00000010){
                status_watch = 0;
            }else if(PORTA & 0b00000100){
                counter = 0;
                status_watch = 0;
                Lcd_Out(1,1,"     0");
            }
        }
        if(tempo >= delay){
            tempo=0;                                    /*reset tempo*/  
            if(status_watch){
                counter++;                              /*Incremento il counter*/
                IntToStr(counter, display);
                Lcd_Out(1,1,display);
            }
            
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
        }
        
    }
}

void interrupt(){
    if(INTCON.TMR0IF){
        INTCON.TMR0IF = 0;
        tempo+=33;
    }
}