/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

#ifndef ADC_H
#define ADC_H

#include "eSafeMain.h"


void init_ADC_Potentiometer (void){
    TRISA = 0x01; // Port A0 is input
    ADCON0 = 0x01;// Converting Channel=AN0, enabled
    ADCON1 = 0x0E;// AN0 channel is analogue
    ADCON2 = 0xBE;// Right justified, 20*TAD, Fosc/64
    ADRESH = 0; //Result register high bits
    ADRESL = 0; //Result register Low bits
   
    GIE  = 1; //Enable all interrupts
    PEIE = 1; //Enable all peripheral interrupts
    IPEN = 0; //Disable interrupt priority levels            
    ADIE = 1; // Enable ADC interrupt
    ADIP = 1; // High priority
    ADIF = 0; //Clear interrupt flag
    return;
}

void config_ADC_Potentiometer (void){
    TRISA  &= 0xFA;// Reset RA0 and RA4 bits
    TRISA  |= 0x01; // Port A0 is input
    ADCON0 = 0x01;// Converting Channel=AN0, enabled
    ADCON1 = 0x0E;// AN0 channel is analogue
    return;
}

void config_ADC_Thermometer (void){
    TRISA  &= 0xFA;// Reset RA0 and RA4 bits
    TRISA  = 0x04; // Port A2 is input
    ADCON0 = 0x09;// Converting Channel=AN2, enabled
    ADCON1 = 0x0A;// AN0-AN1-AN2 channels are analogue
    return;
}

void init_ADC_Thermometer (void){
    TRISA = 0x04; // Port A2 is input
    ADCON0 = 0x09;// Converting Channel=AN2, enabled
    ADCON1 = 0x0A;// AN0-AN1-AN2 channels are analogue
    ADCON2 = 0xBE;// Right justified, 20*TAD, Fosc/64
    ADRESH = 0; //Result register high bits
    ADRESL = 0; //Result register Low bits

    GIE  = 1; //Enable all interrupts
    PEIE = 1;//Enable all peripheral interrupts
    IPEN = 0;//Disable interrupt priority levels
    ADIE = 1; // Enable ADC interrupt
    ADIP = 1; // High priority
    ADIF = 0; //Clear interrupt flag
    return;
}

// External functions prototypes
void init_ADC_Thermometer (void);
void init_ADC_Potentiometer (void);
void config_ADC_Potentiometer (void);
void config_ADC_Thermometer (void);


#endif	/* XC_HEADER_TEMPLATE_H */


