#define DELTA_DELAY 100
#define DELAY_MAX 1000
#define DELAY_MIN 100

unsigned int delay_var = 250;

void main() {
    short dir = 1; //dir=1 direzione è SX=>DX
    unsigned short int i;
    TRISC = 0;
    LATC = 1;

    TRISD = 0;
    LATD = 0;

    ANSELB = 0b00; //enable input buffer

    INTCON |= 0b00001000; //enable RBIE
	INTCON &= 0b11111110; //reset RBIF
	IOCB |= 0b11000000; //enable interrupt on change a RB6 e RB7
	INTCON |= 0b10000000; //enable GIE

//	oppure:
//	IOCB = 0b11000000;
//  INTCON.RBIE = 1;
//  INTCON.RBIF = 0;
//  INTCON.GIE = 1;

    while(1){

        if (dir==1)
            LATC <<= 1;
        else
            LATC >>= 1;
        
        if (LATC == 1)
            dir = 1;
        else if (LATC == 128)
            dir = -1;

        vdelay_ms(delay_var);
        
    }



}

void interrupt(){
	if(INTCON.RBIF){
		
//		PORTB;

		if(PORTB & 0b10000000){
			delay_var += DELTA_DELAY;
			LATD++;
			if(delay_var>DELAY_MAX){
				delay_var = DELAY_MAX;
				LATD = 255;
			}
		}
		else if(PORTB & 0b01000000){
			delay_var -= DELTA_DELAY;
			LATD--;
			if(delay_var < DELAY_MIN){
				delay_var = DELAY_MIN;
				LATD = 0;
			}
		}

		INTCON.RBIF = 0; //NB resetto dopo perché resettare scollega d e q del registro di uscita ==> non leggo nulla
						 //lo faccio solo per la IOCB
	}
// oppure:
//	if(PORTB == 0b10000000){ }
//	else if(PORTB.RB6){ }
}