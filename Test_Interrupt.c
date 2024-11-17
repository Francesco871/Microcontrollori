void main() {
	TRISD = 0;
	ANSELB = 0;

	INTCON |= 0b00001000; //enable RBIE
	INTCON &= 0b11111110; //reset RBIF
	IOCB |= 0b11000000; //enable interrupt on change a RB6 e RB7
	INTCON |= 0b10000000; //enable GIE

}

void interrupt(){
	if(INTCON.RBIF){
		PORTB;
		INTCON.RBIF = 0;

		if(PORTB & 0b10000000)
			LATD = 0b10000000;
		else if(PORTB & 0b01000000)
			LATD = 0b01000000;
	}
}