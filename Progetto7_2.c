void main()
{
    // ----- Output ------
    TRISC = 0;
    TRISD = 0;

    // -- Set CHS (AN0) --
    ADCON0.CHS0 = 0;
    ADCON0.CHS1 = 0;
    ADCON0.CHS2 = 0;
    ADCON0.CHS3 = 0;
    ADCON0.CHS4 = 0;

    //Fosc = 8MHz TAD 
    ADCON2.ADCS0 = 1;
    ADCON2.ADCS1 = 0;
    ADCON2.ADCS2 = 0;

    // --- Set ACQT -----
    ADCON2.ACQT0 = 0;
    ADCON2.ACQT1 = 0;
    ADCON2.ACQT2 = 1;

    // --- Just. Left ---
    ADCON2.ADFM = 0;

    // ----- ADC ON -------
    ADCON0.ADON = 1;

    // Start Conv.
    ADCON0.GO_NOT_DONE = 1;
    
    //Accensione interrupt dell'ADC
    PIE1.ADIE   = 1;
    PIR1.ADIF   = 0;
    //Accensione interrupt in INTCON (ADC e GLOBAL)
    INTCON.PEIE = 1;
    INTCON.GIE  = 1;
    
}

void interrupt()
{
    if(PIR1.ADIF)
    {
        PIR1.ADIF = 0;
        if(ADCON0.CHS0 == 0){
            LATC = ADRESH; //AN0
        }else{
            LATD = ADRESH; //AN1
        }
        ADCON0.CHS0=!ADCON0.CHS0;
        ADCON0.GO_NOT_DONE = 1;
    }
}