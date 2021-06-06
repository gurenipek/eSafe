/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

#ifndef ISR_H
#define ISR_H

#include "eSafeMain.h"
#include "LCD.h"

extern long TIM0LOAD;
extern long TIM1LOAD;
extern byte  digitEntryCount;
//byte  digitEntryCount;
extern byte  result_ADC;
extern byte  passwordsAttempt[];
extern byte  passwordsSet[];
extern byte  segmentData[];
extern byte  displayData[];
extern byte  downCounter90s;
extern byte  safeState;
extern byte  firstTime;
extern byte  start3;
extern byte  blinkEnable;
extern byte   lockAgain;
extern unsigned int TMR2Counter;


// Interrupt Service Routines 
void __interrupt() my_ISR(void){
    float temperatureCelcius;
    byte portData;
    static byte halfSec = 0;//For blinking
    unsigned short read_ADC_Raw;
    byte  before;
    static byte flip = 0;//For state 3
    
    // The source of interrupt ADC   
/*
    if ( ADIF == 1){        
        //Read ADC data registers
        read_ADC_Raw = 0; 
        read_ADC_Raw = ADRESH;
        read_ADC_Raw = read_ADC_Raw << 8; //Shift 8 bits left
        read_ADC_Raw += ADRESL; // Add low byte
        read_ADC_Raw &= 0x03FF; // Use 10-bit    
        if (digitEntryCount == 2){
            //Potentiometer is in reverse action
            read_ADC_Raw = (1024 - read_ADC_Raw);
        }
        //Convert result to 2-digit number. 
        //Each digit is 8-bit uint, so only 8-bit value returned.
        if (read_ADC_Raw < 1000){
            result_ADC = (char)(read_ADC_Raw / 10); //Erase last digit
        }
        else {
            result_ADC = 100;
        }
        //adc data ready 
        
        switch(safeState) {
            case 1  ://Do nothing
            break; 
      
            case 2  :
                if (firstTime == 0){
                   firstTime = 1;
                   before = result_ADC;
                }
                if ((before != result_ADC) && (digitEntryCount == 0)){
                   digitEntryCount = 1;
                }
            break; 
      
            case 3  :
                if (firstTime == 0){
                   firstTime = 1;
                   before = result_ADC;
                }
                if ((before != result_ADC) && (digitEntryCount == 0)){
                   digitEntryCount = 1;
                   start3 = 1;
                }
            break; 
      
            default : 
            break;
        }        
    }
*/
//old version
    if ( ADIF == 1){
        
        //Read ADC value registers
        read_ADC_Raw = 0; 
        read_ADC_Raw = ADRESH;
        read_ADC_Raw = read_ADC_Raw << 8; //Shift 8 bits left
        read_ADC_Raw += ADRESL; // Add low byte
        read_ADC_Raw &= 0x03FF; // Use 10-bit
        
        if (digitEntryCount == 2){
             //Potentiometer is in reverse action
             read_ADC_Raw = (1024 - read_ADC_Raw);
        }
        //Convert result to 2-digit number. 
        //Each digit is 8-bit uint, so only 8-bit value returned.
        if (read_ADC_Raw < 1000){
            result_ADC = (char)(read_ADC_Raw / 10); //Erase last digit
        }
        else {
            result_ADC = 100;
        }
                        
        //State 3 controls
        if ((firstTime == 0) && (safeState == 3)){
             firstTime = 1;
             before = result_ADC;
            // digitEntryCount = 1;
        }
        else if (before != result_ADC){
            start3 = 1;//Start state 3
        }
       
        ADIF = 0; // Clear ADC interrupt flag
    }
  
   //Source of interrupt is external interrupt-0 = RB0 button
   if (INT0IF == 1){
     switch(safeState) {
        case 1  :
        break; 
      
        case 2  :
        //Check if input is valid
        if(result_ADC < 100){
            //Save the last reading value and increment counter      
            passwordsAttempt[digitEntryCount-1] = result_ADC;// [0-1-2] <= [1-2-3]
            digitEntryCount++;
        }
        break; 
      
        case 3  :
        if (digitEntryCount == 0){
            start3 = 1;
            digitEntryCount = 1;
        } 
        if(result_ADC < 100){
            //Save the last reading value and increment counter      
            passwordsAttempt[digitEntryCount-1] = result_ADC;// [0-1-2] <= [1-2-3]
            digitEntryCount++;
        }
        break; 
      
        default : 
         break;
     }        

        INT0IF = 0; //Clear external interrupt 0 flag
        return;
    }
/*
    //Source of interrupt is external interrupt-0 = RB0 button
    if (INT0IF == 1){
      switch(safeState) {
        case 1  :
        break; 
      
        case 2  :
            //Check if input is valid
            if (digitEntryCount == 0){
               digitEntryCount = 1;
            } 
            else if(result_ADC < 100){
               //Save the last reading value and increment counter      
               passwordsAttempt[digitEntryCount-1] = result_ADC;// [0-1-2] <= [1-2-3]
               digitEntryCount++;
            }
        break; 
      
        case 3  :
        if (digitEntryCount == 0){
            start3 = 1;
            digitEntryCount = 1;
        } else if(result_ADC < 100){
            start3 = 1;
            //Save the last reading value and increment counter      
            passwordsAttempt[digitEntryCount-1] = result_ADC;// [0-1-2] <= [1-2-3]
            digitEntryCount++;
        }
        break; 
      
        default : 
        break;
      }        
      INT0IF = 0; //Clear external interrupt 0 flag
    }
*/    
    //Source of interrupt is RB1 pin
    if (INT1IF == 1){
        lockAgain = 1;
        INT1IF = 0;
        //return;
    }
    
    //Source of interrupt is RB2 pin
    if (INT2IF == 1){
        lockAgain = 1;
        INT2IF = 0;
        //return;
    }
    
    //Source of interrupt is one of RB4-RB7 pins
    if (RBIF == 1){
        lockAgain = 1;//Lock the safe again
        RBIF = 0; //Clear external int. flag
        //return;
    }
    
    //TIMER 0 : For down counting by 1 sec
    if(TMR0IF == 1){
        //Write counter value to data array: "digit3 | digit4"
        displayData[2] = downCounter90s / 10;//Get the MSB digit
        displayData[3] = downCounter90s - (displayData[2] * 10);//Get the LSB digit
        
        halfSec= ~(halfSec); //& 0x01;//
        if (halfSec){
            //Decrement counter
            if (downCounter90s != 0) {
            downCounter90s--;//Decrement counter
            }
        }
        
        if (blinkEnable){
            INT0IE = 0;//Interrupt disable for RB0 pin
            if (halfSec){
                TRISB = 0xFF; //Set all pins as output
                PORTB = 0xFF;//Leds on
            }
            else{
                PORTB &= 0x00;//Leds off
                TRISB = 0x11;//Set RB0 and RB4 pins as input
            } 
            //Restore pin states and enable interrupt
           // 
            INT0IE = 1;//Interrupt enable for RB0 pin
        }
        
        //Load timer registers
        TMR0H = (char)(TIM0LOAD >> 8); //(divide by 256)
        TMR0L = (char)(TIM0LOAD - (TMR0H << 8));

        TMR0IF = 0;//Clear interupt flag
    }
    
//    #define bitset(var, bitno)    ((var) |= 1UL << (bitno))
//    #define bitclr(var, bitno)    ((var) &= ~(1UL << (bitno)))
    
    //TIMER 1 is used for refreshing 7-seg display every 20 ms
    if (TMR1IF == 1) {
        
        // Display data   
        TRISD = 0x00; //Set Port D as output
        TRISA = 0xC3; //Set RA2-RA5 as output
        PORTA &= 0; // None of digits selected

                
        //Output data array to 7-seg. display (old display7Seg(void) function)
        //RE1 = 0;//LCD Disable
        PORTE &= ~(1UL << 1);//Reset RE1 bit

        PORTD = segmentData[displayData[0]];
        PORTA |= 0x04;//Set RA2 : Digit-1
        //__delay_ms(10);
        //delayWithTimer2 (8);
        PORTA &= 0;
      
        PORTD = segmentData[displayData[1]];
        PORTA |= 0x08;//Set RA3 : Digit-2
        __delay_ms(10);
        //delayWithTimer2 (8);
        PORTA &= 0;
       
        PORTD = segmentData[displayData[2]];
        PORTA |= 0x10;//Set RA4 : Digit-3
        __delay_ms(10);
        //delayWithTimer2 (8);
        PORTA &= 0;
       
        PORTD = segmentData[displayData[3]];
        PORTA |= 0x20;//Set RA2 : Digit-4
        __delay_ms(10);
        //delayWithTimer2 (8);
        PORTA &= 0;
        //Load timer registers
        TMR1H = (char)(TIM1LOAD >> 8);//(divide by 256)
        TMR1L = (char)(TIM1LOAD - (TMR1H << 8));
        
        //PORTE |= 1UL << 1;//Set RE1 bit : Don't set RE1 bit!!!
        TMR1IF = 0; // Clear Timer-1 interrupt flag
    }
    
    //TIMER 2 is used for delays
    if(TMR2IF == 1){
        TMR2Counter++;
        TMR2IF = 0; //clear interrupt flag
    }
    
    
    return;
}


#endif	/* XC_HEADER_TEMPLATE_H */