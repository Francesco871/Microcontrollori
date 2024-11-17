unsigned int time = 0;
signed int delay = 125;

void main() {
    short dir = 1;
    unsigned short int i;
    TRISC = 0;
    LATC = 1;

	TRISD = 0;
    LATD = 0;

//    T0CON |= 0b11000101;
//    T0CON &= ~0b00101010; //NB: tilde = alt+126

//    INTCON |= 0b00100000;
//    INTCON &= ~(0b00000100);
    T0CON = 0b11000101; //NB: prescaler a 64
    INTCON = 0b00100000;

    INTCON.GIE = 1;

    ANSELB = 0b00; //NB: potevo anche non attivarli perché sono sempre accesi in quanto RB6 e RB7 non hanno l'ADC

    IOCB = 0b11000000;
    INTCON.RBIE = 1;
    INTCON.RBIF = 0;

    TMR0L = 6;

    while(1){
		if(time>=delay){

			time = 0; //oppure tempo-=delay e tolgo TMR0L=6
	
	        if (dir==1)
	            LATC <<= 1;
	        else
	            LATC >>= 1;
	        
	        if (LATC == 1)
	            dir = 1;
	        else if (LATC == 128)
	            dir = -1;
	    }
	}

}

void interrupt(){
    if(INTCON.TMR0IF){
    	TMR0L = 6;
    	INTCON.TMR0IF = 0;
    	time++; //oppure lo incremento di quando è il periodo di TMR0L
    }
    else if(INTCON.RBIF){
        if(PORTB == 0b10000000){
            delay += 100;
            LATD++;
            if(delay > 1000){
                delay = 1000;
                LATD = 255;
            }
        }
        else if(PORTB.RB6){
            delay -= 100;
            LATD--;
            if(delay < 100){
                delay = 100;
                LATD = 0;
            }
        }

        INTCON.RBIF = 0;
    }
}