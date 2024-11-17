/*KittCar 2.0 dinamic delay e stopwatch preciso, display in formato hh:mm:ss:ms/10 */

//NB Setto prescaler a 64 e NON uso il PLL ==> TMR0 = 8192us

//RA0 => Start Stopwatch
//RA1 => Stop Stopwatch
//RA2 => Reset Stopwatch
//RA3 => Increase Kitt Delay
//RA4 => Reduce Kitt Delay

volatile unsigned long int tempo_us_kitt = 0;
volatile unsigned long int tempo_us_watch = 0;
volatile unsigned long int tempo_ms_kitt = 0;
volatile unsigned long int tempo_ms_watch = 0;

#define DELAY_KITT 1000 //NB in ms
#define DELAY_WATCH 10  //NB in ms
#define NCHAR_TEMP 6
#define NCHAR_DISPLAY 11

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
    unsigned short counter_10ms = 0;
    unsigned short counter_s = 0;
    unsigned short counter_m = 0;
    unsigned short counter_h = 0;
    unsigned short confronta;
    char display[NCHAR_DISPLAY+1] = "00:00:00:00";
    char temp[NCHAR_TEMP+1];
    unsigned delay_kitt = DELAY_KITT;
    unsigned short status_watch = 0;
    unsigned short i;

    ANSELA = 0;
    confronta = PORTA;
    
    TRISD = 0;
    LATD = 0;
    TRISC = 0;
    LATC = 0;

    T0CON   = 0b11000101; //Prescaler = 64
    INTCON  = 0b10100000;

        // -----------LCD Initialization---------
    Lcd_Init();                // Init the display library (This will set also the PORTB Configuration)
    
    Lcd_Cmd(_LCD_CLEAR);            // Clear display
    Lcd_Cmd(_LCD_CURSOR_OFF);   // Cursor OFF
    // --------------------------------------
    
    Lcd_Out(1,1,display);

    while(1){
        LATC = delay_kitt/100;
        if((PORTA & 0x1f) != (confronta & 0x1f)){
            confronta = PORTA;
            if(PORTA & 0b00001000){
                if(delay_kitt >= 1000)
                    delay_kitt=1000;
                else{
                    delay_kitt+=100;
                }
            }else if(PORTA & 0b00010000){
                if(delay_kitt <= 100)
                    delay_kitt=100;
                else{
                    delay_kitt-=100;
                }
            }else if(PORTA & 0b00000001){
                status_watch = 1;
            }else if(PORTA & 0b00000010){
                status_watch = 0;
            }else if(PORTA & 0b00000100){
                counter_10ms = 0;
                counter_s = 0;
                counter_m = 0;
                counter_h = 0;
                status_watch = 0;
                Lcd_Out(1,1,"00:00:00:00");
            }
        }
        if(tempo_ms_kitt >= delay_kitt){
            tempo_ms_kitt -= delay_kitt;
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
        if(tempo_ms_watch >= DELAY_WATCH){
            tempo_ms_watch -= DELAY_WATCH;
            if(status_watch){
                counter_10ms++;                      /*Incremento il counter dei ms/10 == decine di ms == centesimi di secondo (da displayare)*/
                while(counter_10ms>=100){
                    counter_s++;                     //counter dei secondi
                    counter_10ms -=100;
                }
                while(counter_s>=60){
                    counter_m++;
                    counter_s -=60;
                }
                while(counter_m>=60){
                    counter_h++;
                    counter_m -=60;
                }
                while(counter_h>=24)
                    counter_h = 0;
                IntToStr(counter_h, temp); //ore
                display[1] = temp[5];
                if(temp[4] != ' ')
                    display[0] = temp[4];
                else
                    display[0] = '0';
                IntToStr(counter_m, temp);   //minuti
                display[4] = temp[5];
                if(temp[4] != ' ')
                    display[3] = temp[4];
                else
                    display[3] = '0';
                IntToStr(counter_s, temp);      //secondi
                display[7] = temp[5];
                if(temp[4] != ' ')
                    display[6] = temp[4];
                else
                    display[6] = '0';
                IntToStr(counter_10ms, temp);   //decine di millisecondi (centesimi di secondo)
                display[10] = temp[5];
                if(temp[4] != ' ')
                    display[9] = temp[4];
                else
                    display[9] = '0';
                Lcd_Out(1,1,display);
            }
        }
        
    }
}

void interrupt(){
    if(INTCON.TMR0IF){
        INTCON.TMR0IF = 0;
        tempo_us_kitt+=192;
        tempo_us_watch+=192;
        tempo_ms_kitt+=8;
        tempo_ms_watch+=8;
        if(tempo_us_kitt>=1000){
            tempo_ms_kitt++;
            tempo_us_kitt-=1000;
        }
        if(tempo_us_watch>=1000){
            tempo_ms_watch++;
            tempo_us_watch-=1000;
        }
    }
}