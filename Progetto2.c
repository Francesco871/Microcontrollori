void main() {
             unsigned short int test = 0; //NB ho usato i decimali perché uso test per operazioni
             TRISC = 0x00;
             
             while(1){
                      LATC = 0xFF;
                      test = test +1; //oppure test++ oppure test+=1
                      if (test >= 128){
                         test = 0;
                      }
             }
}