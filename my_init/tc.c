// DOM-IGNORE-BEGIN
/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries. 
    
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
#include <../include/proc/SAMC21/instance/tc4.h>
#include "tc.h"

/** 
 * init the TC module to generate PWM signal
 */
void TC_init(void) {
	MCLK_REGS->MCLK_APBCMASK = MCLK_APBCMASK_TC4(1);
    
	GCLK_REGS->GCLK_PCHCTRL[TC4_GCLK_ID] = GCLK_PCHCTRL_GEN_GCLK0 | GCLK_PCHCTRL_CHEN(1);
	
	// do a software reset of the module (write-synchronized)
	TC4_REGS->COUNT16.TC_CTRLA = TC_CTRLA_SWRST(1);
	while (TC4_REGS->COUNT16.TC_SYNCBUSY & TC_SYNCBUSY_SWRST(1));
	
	TC4_REGS->COUNT16.TC_CTRLA = 
		  TC_CTRLA_MODE(TC_CTRLA_MODE_COUNT16_Val)
		| TC_CTRLA_PRESCALER_DIV1;
	TC4_REGS->COUNT16.TC_WAVE = TC_WAVE_WAVEGEN_NPWM_Val;
	
	TC4_REGS->COUNT16.TC_CC[1] = 32767;
	while (TC4_REGS->COUNT16.TC_SYNCBUSY & TC_SYNCBUSY_CC1(1));
	
	TC4_REGS->COUNT16.TC_CTRLA |= TC_CTRLA_ENABLE(1);
	while (TC4_REGS->COUNT16.TC_SYNCBUSY & TC_SYNCBUSY_ENABLE(1));
}