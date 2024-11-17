#define DELTA_DELAY 100
#define DELAY_MAX 1000
#define DELAY_MIN 100

unsigned int delay_var = 250;

void main() {
    short dir = 1; //dir=1 direzione è SX=>DX
    unsigned short int i;

    short confronta;

    TRISC = 0;
    LATC = 1;

    TRISD = 0;
    LATD = 0;

    ANSELB = 0b00;
    confronta = PORTB;

    while(1){
    	if(confronta!=PORTB){
    		confronta = PORTB;
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
		}

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