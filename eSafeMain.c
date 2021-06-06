/*
 * File:   eSafeMain.c
 * Author: ipek
 *
 * Created on June 5, 2021, 10:40 AM
 */

#ifndef ESAFEMAIN_C
#define ESAFEMAIN_C

#include "eSafeMain.h"


//Function prototypes
void __interrupt() my_ISR(void);
void init_Clock(void);
void displayNumbers (unsigned int, byte);
void init_PORTB (void);
int  passwordCheck(void);

//Global variables in header file//


void main(void) {
  
  byte  digit1, digit2, digit3, digit4;
  byte  heaterTemperature;
  byte  attemption;
  int   isValid;

  //Initializations
  init_Clock();
  InitLCD();
  init_ADC_Thermometer();
  init_ADC_Potentiometer();// Set ADC channel AN0 and enable conversion
  init_PORTB();//Set RB0 and RB4 as input and enable interrupts
  init_TIMER2();
  
  safeState = 1;//State=1
    while (1){
      PORTB = 0; //--
      PORTD = 0; //-- 
      INT0IF = 0;//Clear RB0 interrupt flag
      
      /************************************************************************/
      /* ------ 2.2.1 START-UP STATE -----------------------------------------*/
      safeState = 1;//State=1
      LCDCmd(LCD_CLEAR);
      LCDGoto(1, 1);
      LCDStr("SuperSecureSafe!");
      delayWithTimer2 (2000);// Wait 3 secs
      INT0IE = 1;//Enable RB0 interrupt
      
      
      /************************************************************************/
      /* ------- 2.2.2 PASSWORD SET STATE ------------------------------------*/
      safeState = 2;//State=2
      //Prepare LCD
      LCDCmd(LCD_CLEAR);
      LCDGoto(1, 1);
      LCDStr("Set Password:");
      LCDGoto(1, 2);
      LCDStr("__-__-__");
      
      result_ADC = 0;
      digitEntryCount = 0;
      // "digitEntryCount" will be incremented in RB0 ISR (After confirmation)
      while (digitEntryCount < 4){
          ADCON0 |= 0x02; // set the Go bit and start conversion
          __delay_us(20); // ADC values will be read in ADC ISR 
          displayNumbers (result_ADC, digitEntryCount);
      }
      
      //Save combination
      for (int i = 0;i < 3; i++){
          passwordsSet[i] = passwordsAttempt[i];
      }
 
      
      /************************************************************************/
      //------- 2.2.3 PASSWORD CHECK STATE -----------------------------------*/

      safeState = 3;
      
      init_TIMER0();//Timer-0 is used as a 1 sec. time base for down counting 90 sec. 
      init_TIMER1();//Timer-1 is used for refreshing 7-seg display every 20 ms

      //Set-up 7-seg display and show 90 sec and attempting number of 3
      downCounter90s = 90;
      //Write counter value to data array: "digit1 | digit2 | digit3 | digit4"
      displayData[0] = 10;//Blank
      displayData[1] = 11;//3 line
      displayData[2] = 9; 
      displayData[3] = 0;
      TMR1ON = 1;//Start Timer 1 for refresing 7-seg. display

      //Display Check State on LCD
      LCDCmd(LCD_CLEAR);
      LCDGoto(1, 1);
      LCDStr("Input Password:");
      LCDGoto(1, 2);
      LCDStr("__-__-__");
   
      //User now tries to make inputs for the right combination
      //Wait for any input from RB0 or ADC
      start3 = 0;
      digitEntryCount = 0;
      while (!start3){
          ADCON0 |= 0x02; // set the Go bit and start conversion
          __delay_us(20); // ADC values will be read in ADC ISR 
          INT0IE = 1;
      }
      
      digitEntryCount = 1;
      attemption = 3;//There will be 3 attemption right for user
      while (attemption){
          //In a failed condition take an action
          switch(attemption) {
              case 1  ://Third attempt
                  displayData[0] = 10;//Blank
                  displayData[1] = 13;//1 line
                  //Turn on heater, read temperature
////////////>>>>>>>>                  RC5 = 1; //Turn on heater              
              break; 
             
              case 2  ://Second attempt
                  blinkEnable = 1; //Blink PortB leds
                  displayData[0] = 10;//Blank
                  displayData[1] = 12;//2 line
              break; 
                        
              case 3  : //First attempt
                  TMR0ON = 1;//Start 90 sec Timer 0 
              break; 
             
              default : 
              break;
          } 
                  
          //Get the passwords
          
          digitEntryCount = 1;
          while (digitEntryCount < 4){
               config_ADC_Potentiometer();//Prepare channel-0 for potentiometer reading
               ADCON0 |= 0x02; // set the Go bit and start conversion
               __delay_us(20); // ADC values will be read in ADC ISR 
                 // INT0IE = 1;//Interrupt enable for RB0 pin
                 // __delay_us(20);
               displayNumbers (result_ADC, digitEntryCount);
               
               //Check if temperature is over
               if (attemption == 1) {
                  config_ADC_Thermometer ();//Prepare channel-2 for temperature reading
                  ADCON0 |= 0x02; // set the Go bit and start conversion
                  __delay_us(20); 
                  heaterTemperature = ((result_ADC * 5) / 1024) * 100;
                  if (heaterTemperature > 40){
                      //Failed
                      attemption = 0;
                      safeState = 5;
                  }
                  //Check if 90-sec timer is over
                  if (downCounter90s == 0){
                      //Failed
                      attemption = 0;
                      safeState = 5;
                  }
              }
          } 
          
          //Check if combination is valid.
          isValid = passwordCheck();
          if (isValid){
              //Combination is valid! Continue to "SUCCESS" state
              attemption = 0; //For exiting while loop
              safeState = 4; //"SUCCESS" state
          }
          else{
              //Failed. Continue to other attemptions
              attemption--;
              LCDCmd(LCD_CLEAR);
              LCDGoto(1, 1);
              //LCDStr("Input Password:");
              LCDStr("Try Again:");
              LCDGoto(1, 2);
              LCDStr("__-__-__");  
          }
      }  //End of attemptions loop
      
      //Completed... Init all the devices
      //Stop counter and blank 7-segment display
      TMR0ON = 0;
      displayData[0] = 10;
      displayData[1] = 10;
      displayData[2] = 10;
      displayData[3] = 10;
      //Stop blinking and turn off RB leds
      blinkEnable = 0;
      PORTB &= 0x00;//Leds off
      TRISB = 0x11;//Set RB0 and RB4 pins as input
      //Turn off heater
      RC5 = 0;
      //Disable interrupts           
      ADIE = 0;
      INT0IE = 0;
      RBIE = 0;
      TMR0IE = 0;
      TMR1IE = 0;
      TMR0ON = 0;
      TMR0ON = 0;
      
      if (safeState == 4){
          LCDCmd(LCD_CLEAR);
          LCDGoto(1, 1);
          LCDStr("Unlocked; Press");
          LCDGoto(1, 2);
          LCDStr("RB1 to lock!");
      }
      else {
          //"FAILED" state
          LCDCmd(LCD_CLEAR);
          LCDGoto(1, 1);
          LCDStr("You Failed!");
      }
      
    //  delayWithTimer2 (3000);
      
      lockAgain = 0;
      TRISB = 0x13;//RB0, RB1 and RB4 enable
      //RBIE  = 1; //Interrupt Enable for RB4-RB7 pins
      INT1IE = 1;//Ext. int 1 enable (Port B pin 1)
      INT2IE = 1;//Ext. int 1 enable (Port B pin 2)
      //Wait for the user for locking the safe
      while(!lockAgain){
      }
     
      // Start from beginning..
      LCDCmd(LCD_CLEAR);
      LCDGoto(1, 1);
      LCDStr("Safe is Locked!");
      delayWithTimer2 (3000);
      
  }
  return;
}
  


// Functions-------------------------------------------------------------------------------------------------------


int passwordCheck(void){
    byte n;
    
    for (n = 0; n < 3; n++){
        if (passwordsSet[n] != passwordsAttempt[n]){
            return 0;
        }
    }
    return 1;
}

// This function displays the numbers on LCD that confirmed by the user
void displayNumbers (unsigned int value, byte digitNo){
    byte digitH, digitL;
    
    TRISD = 0;
    //Control the number limits
    if (value >= 100){
        // Display X
        digitH = 'X';
        digitL = 'X';
    }
    else {
        //Convert hex value to ascii character
        digitH = (byte)value / 10;//Get the MSB digit
        digitL = (byte)(value - (digitH * 10));//Get the LSB digit
        digitH += 0x30;//Convert number to ASCII
        digitL += 0x30;//Convert number to ASCII
    }
    //Display the number at proper position
    switch(digitNo) {
     /* case 0  :
         LCDGoto(1,2); //First digit
         LCDDat(digitH);
         LCDDat(digitL);
         break;  */
      case 1  :
         LCDGoto(1,2); //First digit
         LCDDat(digitH);
         LCDDat(digitL);
         break; 
      case 2  :
         LCDGoto(4,2); //Second digit
         LCDDat(digitH);
         LCDDat(digitL);
         break; 
      case 3  :
         LCDGoto(7,2); //Third digit
         LCDDat(digitH);
         LCDDat(digitL);
         break; 
      default : 
         break;
    }
   return;
}


void init_PORTB (void){
    //TRISB = 0x11; // Both RB0 and RB4 pins are input
    TRISB = 0xFF;
    RBPU = 1; //0:PortB pull-ups enable 
    INTEDG0 = 0;//0:Externel Interrupt 0 on falling edge
    INTEDG1 = 0;//0:External Interrupt 1 on falling edge
    RBIP = 1; //RB port change interrupt high pri.
    INT0IE = 1;//Interrupt enable for RB0 pin
    RBIE = 0; //Interrupt disable for RB4-RB7 pins
    GIE = 1; //Enable all interrupts
    PEIE = 1;//Enable all peripheral interrupts
    
    INT1IE = 0;//test ext int 1 disable; PortB-pin 1
    return;
}


void init_Clock(void){
     PLLEN = 0;//PLL disabled...
     //8MHZ
     IRCF2 = 1;
     IRCF1 = 1;
     IRCF0 = 1;
  }

#endif  /*ESAFEMAIN_C*/
