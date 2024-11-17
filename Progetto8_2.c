void main() {

    // Fast ADC Settings 

    //Fosc = 8MHz
    ADCON2 = 0b00100010;
    //ADCON1 = 0;
    ADCON0=0b00000001;

    // - Disaple CCP Out -
    TRISE.RE2 = 1;	
	
    // --- TMR2 - CCP5 ---
    CCPTMRS1.C5TSEL1 = 0;
    CCPTMRS1.C5TSEL0 = 0;

    // --- Set Period ----
    PR2 = 255;

    // --- PWM on CCP5 ---
    CCP5CON = 0b00001100;

    // ---- Set Ton ------
	// d = CCPR5L /(PR2 +1)
    CCPR5L = 0;

    // ---- Set TMR2 -----
    // TMR2 ON, Max Prescaler
    T2CON = 0b00000111;

    // - Output Enable --
    TRISE.RE2 = 0;

    // Start Conv.
    ADCON0.GO_NOT_DONE = 1;
    
    //Accensione interrupt dell'ADC
    PIE1.ADIE   = 1;
    PIR1.ADIF   = 0;
    //Accensione interrupt in INTCON (ADC e GLOBAL)
    INTCON.PEIE = 1;
    INTCON.GIE  = 1;

}

void interrupt(){
    if(PIR1.ADIF){
        PIR1.ADIF = 0;
        CCPR5L = ADRESH;
        ADCON0.GO_NOT_DONE = 1;
    }
}