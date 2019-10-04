// DOM-IGNORE-BEGIN
/*
    (c) 2019 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
 */
// DOM-IGNORE-END

#include <xc.h>
#include "adc.h"

// missing in header files
#define ADC0_GCLK_ID                33       // index of Generic Clock
#define ADC1_GCLK_ID                34       // index of Generic Clock
#define ADC_INPUTCTRL_MUXNEG_GND_Val _U_(0x18) // select GND as negative input
#define ADC_CAL_ADDR 0x806020 // NVM area where factory calibration is stored

void ADC_init(void) {
    // connect clocks to ADC0 and ADC1
    MCLK_REGS->MCLK_APBCMASK |= MCLK_APBCMASK_ADC0(1);
    MCLK_REGS->MCLK_APBCMASK |= MCLK_APBCMASK_ADC1(1);
	GCLK_REGS->GCLK_PCHCTRL[ADC0_GCLK_ID] = GCLK_PCHCTRL_GEN_GCLK2 | GCLK_PCHCTRL_CHEN(1);
    GCLK_REGS->GCLK_PCHCTRL[ADC1_GCLK_ID] = GCLK_PCHCTRL_GEN_GCLK2 | GCLK_PCHCTRL_CHEN(1);
	
	// do a software reset of the modules (write-synchronized)
	ADC0_REGS->ADC_CTRLA = ADC_CTRLA_SWRST(1);
	while (ADC0_REGS->ADC_SYNCBUSY & ADC_SYNCBUSY_SWRST(1));
    ADC1_REGS->ADC_CTRLA = ADC_CTRLA_SWRST(1);
	while (ADC1_REGS->ADC_SYNCBUSY & ADC_SYNCBUSY_SWRST(1));
    
    // load calibration values from NVM
    uint64_t reg = *(uint64_t *) ADC_CAL_ADDR;
    reg &= 0x000000000007FFFF;
    ADC0_REGS->ADC_CALIB =
        ADC_CALIB_BIASREFBUF((reg >> 0) & 0x03) |
        ADC_CALIB_BIASCOMP((reg >> 3) & 0x03);
    ADC1_REGS->ADC_CALIB =
        ADC_CALIB_BIASREFBUF((reg >> 6) & 0x03) |
        ADC_CALIB_BIASCOMP((reg >> 9) & 0x03);
    
    // minimum sampling time according to characteristics in 45.10.4
    // assuming Rsource = 10k
    // maximum values for Rsample = 1715, Csample = 4.5p
    // 12 bit conversion
    // tsamplehold >= 512 ns    
    ADC0_REGS->ADC_SAMPCTRL = 
        ADC_SAMPCTRL_SAMPLEN(0) |
        ADC_SAMPCTRL_OFFCOMP(1);
    while(ADC0_REGS->ADC_SYNCBUSY & ADC_SYNCBUSY_SAMPCTRL(1));
    ADC0_REGS->ADC_CTRLB =
        ADC_CTRLB_PRESCALER(ADC_CTRLB_PRESCALER_DIV2_Val);
    ADC1_REGS->ADC_SAMPCTRL = 
        ADC_SAMPCTRL_SAMPLEN(0) |
        ADC_SAMPCTRL_OFFCOMP(1);
    while(ADC1_REGS->ADC_SYNCBUSY & ADC_SYNCBUSY_SAMPCTRL(1));
    ADC1_REGS->ADC_CTRLB =
        ADC_CTRLB_PRESCALER(ADC_CTRLB_PRESCALER_DIV2_Val);
    
    // configure reference for ADC
    ADC0_REGS->ADC_REFCTRL =
        ADC_REFCTRL_REFCOMP(1) |
        ADC_REFCTRL_REFSEL(ADC_REFCTRL_REFSEL_INTREF_Val);
    ADC1_REGS->ADC_REFCTRL =
        ADC_REFCTRL_REFCOMP(1) |
        ADC_REFCTRL_REFSEL(ADC_REFCTRL_REFSEL_INTREF_Val);
    
    // configure negative ADC input
    // differential measurement between AIN 4 and 5
    ADC0_REGS->ADC_INPUTCTRL =
        ADC_INPUTCTRL_MUXNEG(0x18) |
        ADC_INPUTCTRL_MUXPOS(ADC_INPUTCTRL_MUXPOS_AIN8);
    while(ADC0_REGS->ADC_SYNCBUSY & ADC_SYNCBUSY_INPUTCTRL(1));
    // single-ended measurement
    // 0x18 = internal ground
    ADC1_REGS->ADC_INPUTCTRL =
        ADC_INPUTCTRL_MUXNEG(ADC_INPUTCTRL_MUXNEG_AIN4_Val) |
        ADC_INPUTCTRL_MUXPOS(ADC_INPUTCTRL_MUXPOS_AIN5_Val);
    while(ADC1_REGS->ADC_SYNCBUSY & ADC_SYNCBUSY_INPUTCTRL(1));
    
    // configure ADC mode
    ADC0_REGS->ADC_CTRLC =
        ADC_CTRLC_DUALSEL(ADC_CTRLC_DUALSEL_BOTH_Val) |
        ADC_CTRLC_WINMODE(ADC_CTRLC_WINMODE_DISABLE_Val) |
        ADC_CTRLC_R2R(0) |
        ADC_CTRLC_RESSEL(ADC_CTRLC_RESSEL_12BIT_Val) |
        ADC_CTRLC_CORREN(0) |
        ADC_CTRLC_FREERUN(0) |
        ADC_CTRLC_LEFTADJ(0) |
        ADC_CTRLC_DIFFMODE(0);
    while(ADC0_REGS->ADC_SYNCBUSY & ADC_SYNCBUSY_CTRLC(1));
    ADC1_REGS->ADC_CTRLC =
        ADC_CTRLC_DUALSEL(ADC_CTRLC_DUALSEL_BOTH_Val) |
        ADC_CTRLC_WINMODE(ADC_CTRLC_WINMODE_DISABLE_Val) |
        ADC_CTRLC_R2R(0) |
        ADC_CTRLC_RESSEL(ADC_CTRLC_RESSEL_12BIT_Val) |
        ADC_CTRLC_CORREN(0) |
        ADC_CTRLC_FREERUN(0) |
        ADC_CTRLC_LEFTADJ(0) |
        ADC_CTRLC_DIFFMODE(1);
    while(ADC1_REGS->ADC_SYNCBUSY & ADC_SYNCBUSY_CTRLC(1));
    
    // configure averaging
    ADC0_REGS->ADC_AVGCTRL =
        ADC_AVGCTRL_ADJRES(0) |
        ADC_AVGCTRL_SAMPLENUM(ADC_AVGCTRL_SAMPLENUM_1);
    while(ADC0_REGS->ADC_SYNCBUSY & ADC_SYNCBUSY_AVGCTRL(1));
    ADC1_REGS->ADC_AVGCTRL =
        ADC_AVGCTRL_ADJRES(0) |
        ADC_AVGCTRL_SAMPLENUM(ADC_AVGCTRL_SAMPLENUM_1);
    while(ADC1_REGS->ADC_SYNCBUSY & ADC_SYNCBUSY_AVGCTRL(1));
    
    // configure automatic sequencing
    // sequential measurement for AIN 8, 9, 10, 11
    ADC0_REGS->ADC_SEQCTRL = (1<<8) | (1<<9) | (1<<10) | (1<<11);
        
    // enable interrupts for result
    ADC0_REGS->ADC_INTENSET = 
        ADC_INTENSET_OVERRUN(1);
    ADC1_REGS->ADC_INTENSET = 
        ADC_INTENSET_OVERRUN(1);

    // end of single measurement on ADC1 will start sequence on ADC0
    // activate ADC1 RESRDY event output
    ADC1_REGS->ADC_EVCTRL = ADC_EVCTRL_RESRDYEO(1);
    // activate ADC0 START event input
    ADC0_REGS->ADC_EVCTRL = ADC_EVCTRL_STARTEI(1);
    
    // enable ADC0 and ADC1
    ADC0_REGS->ADC_CTRLA = ADC_CTRLA_ENABLE(1);
	while (ADC0_REGS->ADC_SYNCBUSY & ADC_SYNCBUSY_ENABLE(1));
    ADC1_REGS->ADC_CTRLA = ADC_CTRLA_ENABLE(1);
	while (ADC1_REGS->ADC_SYNCBUSY & ADC_SYNCBUSY_ENABLE(1));
    
}
