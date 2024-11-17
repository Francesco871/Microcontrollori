void main() {
     TRISC = 0; /*setto il pin*/
     
     while(1){
             LATC = 255; /*accendo il LED*/
             delay_ms(500);
             LATC = 0;   /*spengo  il LED*/
             delay_ms(500);
          }
}