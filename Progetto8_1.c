void main() {

    

    // - Disaple CCP Out -
    TRISE.RE2 = 1;
	// -------------------
	
	
    // --- TMR2 - CCP5 ---
    CCPTMRS1.C5TSEL1 = 0;
    CCPTMRS1.C5TSEL0 = 0;
	// -------------------


    // --- Set Period ----
    PR2 = 255;
	// -------------------

    // --- PWM on CCP5 ---
    CCP5CON = 0b00001100;
    //CCP5CON.CCP5M3 = 1;
    //CCP5CON.CCP5M2 = 1;
	// -------------------

    // ---- Set Ton ------
	// d = CCPR5L /(PR2 +1)
    CCPR5L = 0;
	// -------------------


    // ---- Set TMR2 -----
    // TMR2 ON, Max Prescaler
    T2CON = 0b00000111;
	// -------------------



    // - Output Enable --
    TRISE.RE2 = 0;
	// -------------------

    while(1){
        
        
        Delay_ms(100);
        CCPR5L++;


    }


}