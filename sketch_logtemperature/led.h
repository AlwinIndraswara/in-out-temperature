#ifndef Led_h
#define Led_h




class LedClass {

  public:
  
    // Constructor  
    LedClass();  

    /*
     * Turns the LED on/off
     */
    void light(bool turnOn);

    /*
     * Blinks the LED for a specified interval
     */
    void blinkMillis(int ms);

    /*
     * Blinks the LED once
     */
    void blink();

    /*
     * Blinks the LED to signify success 
     */
    void blinkSuccess();

    /*
     * Blinks the LED to signify failure 
     */
    void blinkFailure();
    
    /*
     * Blinks the LED really quickly to signify that the program is running 
     */
    void blinkHeartbeat();
    
};


extern LedClass Led;

#endif
