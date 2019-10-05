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
#include <proc/SAMC21/component/evsys.h>

// missing header symbols
#define EVSYS_GCLK_ID_0             6
#define EVSYS_GCLK_ID_1             7
    
void EVSYS_init(void) {
    // connect clocks to EVSYS
    MCLK_REGS->MCLK_APBCMASK |= MCLK_APBCMASK_EVSYS(1);
    
    // connect event channel 0 to GCLK0
    GCLK_REGS->GCLK_PCHCTRL[EVSYS_GCLK_ID_0] = GCLK_PCHCTRL_GEN_GCLK0 | GCLK_PCHCTRL_CHEN(1);
    
    // trigger a software reset (no sync necessary)
    EVSYS_REGS->EVSYS_CTRLA = EVSYS_CTRLA_SWRST(1);
    while(EVSYS_REGS->EVSYS_CTRLA & EVSYS_CTRLA_SWRST(1));
    
    // setup channel 0, generator SDADC RESRDY
    EVSYS_REGS->EVSYS_CHANNEL[0] = 
        EVSYS_CHANNEL_PATH(EVSYS_CHANNEL_PATH_ASYNCHRONOUS_Val) |
        EVSYS_CHANNEL_EDGSEL(EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT_Val) |
        EVSYS_CHANNEL_EVGEN(0x47);
    
    // setup user ADC1 for channel 0
    EVSYS_REGS->EVSYS_USER[30] = 0x01;
    
    // setup channel 1, generator ADC1 RESRDY
    EVSYS_REGS->EVSYS_CHANNEL[1] = 
        EVSYS_CHANNEL_PATH(EVSYS_CHANNEL_PATH_ASYNCHRONOUS_Val) |
        EVSYS_CHANNEL_EDGSEL(EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT_Val) |
        EVSYS_CHANNEL_EVGEN(0x45);
    
    // setup user ADC0 for channel 1
    EVSYS_REGS->EVSYS_USER[28] = 0x02;
        
}
