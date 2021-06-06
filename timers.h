/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef TIMERS_H
#define TIMERS_H

#include "eSafeMain.h"

extern unsigned int TMR2Counter;
extern long TIM0LOAD;
extern long TIM1LOAD;

//Timer 0 : For down counter by 0.5 sec
//Timer reload parameters: f_osc = 10 Mhz-> 0,1 us clock
//Timer 0 is configured as 16-bit timer.
//Timer 0 prescaler 1:256 => (0,1 us * 256) => f_timer0 = 25.6 us
//Count for 0.5 sec = 500000/25.6 = 19531 clock cycles
//Timer 0 counters will be loaded = 65536 - 19536 = 46000
//#define TIM0LOAD 46000 //orj
// TIM0LOAD = 30000 //At Picsimlab, time durations differ?


//TIMER 1 : For refreshing 7-seg display every 20 ms
//Timer 1 is a 16-bit timer. Clock is 0.1 us 
//Timer 1 prescaler 1:8max => (0,1 us * 8) => f_timer1 = 0.8 us
//Count for 20 ms = 20000/0.8 = 25000 clock cycles
//Timer 1 counters will be loaded = 65536 - 25000 = 40536
//#define TIM1LOAD 40536 //orj
// TIM1LOAD = 50000 //At Picsimlab, time durations differ?


//For down counter by 1 sec
//presc 1:128; load 25000 -> 0.5s
void init_TIMER0 (void){
    T0CON = 0;
    TMR0ON  = 0;//stop TIMER0 counter    
    T08BIT = 0;//16-bit counter
    T0CS = 0;//Internal clock
    PSA = 0;//Prescaler enable
    T0PS0 = 0;//
    T0PS1 = 1;//
    T0PS2 = 1;// 1:128 pre-scaler=110
    TMR0IE = 1;//Timer0 interrupt enable
    GIE = 1; //Enable all interrupts
    PEIE = 1;//Enable all peripheral interrupts
    //Load timer register
    TMR0H = (unsigned char)(TIM0LOAD >> 8);//(divide by 256)
    TMR0L = (unsigned char)(TIM0LOAD - (TMR1H << 8));
    return;
}


//This timer is used for refreshing 7-seg display every 20 ms
void init_TIMER1 (void){
    
    T1CON = 0;//Clear register
    TMR1ON  = 0;//stop TIMER0 counter    
    T1OSCEN = 0;//Timer1 oscillator is off
    TMR1CS = 0;//Internal clock enabled:f_OSC/4
    //T0CON |= 0x30;  // 1:8 pre-scaler
    T1CKPS0 = 0;// 1:8>1:1
    T1CKPS1 = 1;
    TMR1IE = 1;//Timer0 interrupt enable
    GIE = 1; //Enable all interrupts
    PEIE = 1;//Enable all peripheral interrupts
    //Load timer registers
//    TMR1H = (unsigned char)(TIM1LOAD >> 8);//(divide by 256)
//    TMR1L = (unsigned char)(TIM1LOAD - (TMR1H << 8));
    
    return;
}


void delayWithTimer2 (unsigned int milliseconds){
    unsigned int currentVal = 0;
    //start the timer2
    TMR2Counter = 0; //the counter is reset.
    TMR2ON = 1;
    //TIMER2 Interrupt increments TMR2Counter by 1 every 1 millisecond
    do{
        currentVal = TMR2Counter;
    }while(currentVal < milliseconds);
    TMR2ON = 0;      //stop the timer
}

void init_TIMER2 (void){
    //Set Timer2: TMR2ON=off, T2CKPS=1:16, T2OUTPS=1:16
    T2CON = 0x7B;  // 01111011
    PR2 = 0x28;    // PR2=40 
    TMR2 = 0x00;
    PIR1bits.TMR2IF = 0; // Clear IF flag
    TMR2ON = 0;//Stop timer
    TMR2IE = 1;//Timer2 interrupt enable
    GIE = 1; //Enable all interrupts
    PEIE = 1;//Enable all peripheral interrupts
    TMR2Counter = 0; //initialize global variable 
}



// External functions prototypes
void init_TIMER0(void);
void init_TIMER1 (void);
void init_TIMER2 (void);
void delayWithTimer2 (unsigned int);






#endif	/* XC_HEADER_TEMPLATE_H */

