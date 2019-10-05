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
#include <proc/SAMC21/component/sdadc.h>
#include "sdadc.h"

// missing in header files
#define SDADC_GCLK_ID               35       // Index of generic clock

void SDADC_init() {
    // connect clock to SDADC
    MCLK_REGS->MCLK_APBCMASK |= MCLK_APBCMASK_SDADC(1);
    GCLK_REGS->GCLK_PCHCTRL[SDADC_GCLK_ID] = 
        GCLK_PCHCTRL_GEN_GCLK0 |
        GCLK_PCHCTRL_CHEN(1);
    
    // do a software reset of SDADC (write-synchronized)
	SDADC_REGS->SDADC_CTRLA = SDADC_CTRLA_SWRST(1);
	while (SDADC_REGS->SDADC_SYNCBUSY & SDADC_SYNCBUSY_SWRST(1));
    while (SDADC_REGS->SDADC_CTRLA & SDADC_CTRLA_SWRST(1));
    
    // configure reference for SDADC
    SDADC_REGS->SDADC_REFCTRL =
        SDADC_REFCTRL_ONREFBUF(0) |
        SDADC_REFCTRL_REFRANGE(0) |
        SDADC_REFCTRL_REFSEL(SDADC_REFCTRL_REFSEL_INTREF_Val);
    
    // configure sampling
    // maximum sample clock speed with chopper on is 3 MHz
    // divide 48 MHz by 16, division factor is (n+1)*2
    SDADC_REGS->SDADC_CTRLB =
        SDADC_CTRLB_SKPCNT(4) |
        SDADC_CTRLB_OSR(SDADC_CTRLB_OSR_OSR64_Val) |
        SDADC_CTRLB_PRESCALER(7);
        
    // configure event output
    SDADC_REGS->SDADC_EVCTRL = SDADC_EVCTRL_RESRDYEO(1);
    
    // configure interrupts
    SDADC_REGS->SDADC_INTENSET = SDADC_INTENSET_OVERRUN(1);
    
    // select input
    SDADC_REGS->SDADC_INPUTCTRL =
        SDADC_INPUTCTRL_MUXSEL(SDADC_INPUTCTRL_MUXSEL_AIN1_Val);
    while(SDADC_REGS->SDADC_SYNCBUSY & SDADC_SYNCBUSY_INPUTCTRL(1));
    
    // configure single shot mode
    SDADC_REGS->SDADC_CTRLC = SDADC_CTRLC_FREERUN(0);
    while(SDADC_REGS->SDADC_SYNCBUSY & SDADC_SYNCBUSY_CTRLC(1));
    
    // turn on chopper mode to compensate offset
    SDADC_REGS->SDADC_ANACTRL = SDADC_ANACTRL_ONCHOP(1);
    while(SDADC_REGS->SDADC_SYNCBUSY & SDADC_SYNCBUSY_ANACTRL(1));
    
    // automatically shift by 8 to get 16 bit result
    SDADC_REGS->SDADC_SHIFTCORR = SDADC_SHIFTCORR_SHIFTCORR(8);
    while(SDADC_REGS->SDADC_SYNCBUSY & SDADC_SYNCBUSY_SHIFTCORR(1));
    
    // turn on SDADC
    SDADC_REGS->SDADC_CTRLA = SDADC_CTRLA_ENABLE(1);
    while(SDADC_REGS->SDADC_SYNCBUSY & SDADC_SYNCBUSY_ENABLE(1));
    
    
}