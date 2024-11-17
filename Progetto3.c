#define DELAY 500

void main() {
        short i, andata, andata_v2; // nb: 8bit
        TRISB = TRISC = TRISD = TRISE = 0;
        andata = 1;
        andata_v2 = 0b00000111;
        i=0;
        LATB = LATC = LATD = 0x01; // == =1 == =0b1 == 0x1 == 0b00000001
        LATE = 0;

        while(1){
//CODICE ORIGINARIO: funziona ma bloccante
//                for(i=0; i<7; i++){
//                        delay_ms(250);
//                        LATC = LATC<<1; // oppure LATC <<= 1
//                }
//                for(; i>0; i--){
//                        delay_ms(250);
//                        LATC = LATC>>1;
//                }

//PRIMA MODIFICA: metodo non bloccante (un solo delay)
//                delay_ms(250);
//                if(andata){
//                        LATC = LATC<<1;
//                        if(LATC == 0x80){
//                                andata = 0;
//                        }
//                }else{
//                        LATC = LATC>>1;
//                        if(LATC == 0x01){
//                                andata = 1;
//                        }
//                }

//SECONDA MODIFICA: funziona con tutte le porte a delay diversi
                delay_ms(DELAY);
                i++;
                if(andata_v2 & 0b00000100){
                        LATB <<= 1;                        
                }else{
                        LATB >>= 1;
                }
                if(LATB == 0x80 || LATB == 0x01){
                        andata_v2 = andata_v2^0b00000100; //nb ^ == XOR
                }
                if(!(i%2)){ //con la i rallento l'aggiornamento delle altre porte (cambio delay) qui delay = 2*DELAY
                        if(andata_v2 & 0b00000010){
                                LATC <<= 1;                        
                        }else{
                                LATC >>= 1;
                        }
                        if(LATC == 0x80 || LATC == 0x01){
                                andata_v2 = andata_v2^0b00000010;
                        }                        
                }
                if(!(i%4)){ //qui delay = 4*DELAY
                        if(andata_v2 & 0b00000001){
                                LATD <<= 1;                        
                        }else{
                                LATD >>= 1;
                        }
                        if(LATD == 0x80 || LATD == 0x01){
                                andata_v2 = andata_v2^0b00000001;
                        }                        
                }
                if(i==4){
                        i=0;
                }
                LATE = andata_v2 & 0b00000111;
        }
}