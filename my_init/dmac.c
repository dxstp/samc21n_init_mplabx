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

#include <sam.h>
#include "dmac.h"

volatile dmac_descriptor_registers_t DMAC_descriptor[12];
volatile dmac_descriptor_registers_t DMAC_wrb_descriptor[12];

volatile uint16_t adc_result[10];

static void DMAC_init_channels(void);
static void DMAC_init_descriptors(void);


void DMAC_init() {
    // connect MCLK to DMAC
    MCLK_REGS->MCLK_AHBMASK |= MCLK_AHBMASK_DMAC(1);
    
    // do a software reset (no sync needed, but must be disabled first)
    DMAC_REGS->DMAC_CTRL &= ~DMAC_CTRL_DMAENABLE_Msk;
    while(DMAC_REGS->DMAC_CTRL & DMAC_CTRL_DMAENABLE(1));
    DMAC_REGS->DMAC_CTRL &= ~DMAC_CTRL_CRCENABLE_Msk;
    DMAC_REGS->DMAC_CTRL = DMAC_CTRL_SWRST(1);
    while(DMAC_REGS->DMAC_CTRL & DMAC_CTRL_SWRST(1));
    
    // all priority levels run with static arbitration scheme
    DMAC_REGS->DMAC_PRICTRL0 = 
        DMAC_PRICTRL0_LVLPRI0(0) |
        DMAC_PRICTRL0_RRLVLEN0(0) |
        DMAC_PRICTRL0_LVLPRI1(0) |
        DMAC_PRICTRL0_RRLVLEN1(0) |
        DMAC_PRICTRL0_LVLPRI2(0) |
        DMAC_PRICTRL0_RRLVLEN2(0) |
        DMAC_PRICTRL0_LVLPRI3(0) |
        DMAC_PRICTRL0_RRLVLEN3(0);
    
    // set base address for DMA descriptors
    DMAC_REGS->DMAC_BASEADDR = (uint32_t) DMAC_descriptor;
    
    // set address for DMA write-back descriptors
    DMAC_REGS->DMAC_WRBADDR = (uint32_t) DMAC_wrb_descriptor;
    
    // activate DMA controller
    DMAC_REGS->DMAC_CTRL = DMAC_CTRL_DMAENABLE(1);

    // before enabling channels, first transfer descriptors must be configured
    DMAC_init_descriptors();
    
    // configure channels and activate them
    DMAC_init_channels();
    
    // allow level 3 requests
    DMAC_REGS->DMAC_CTRL |= DMAC_CTRL_LVLEN3(1);
        
    _nop();
    
}

static void DMAC_init_channels() {
    // configure channel 0
    // trigger a beat on ADC0 RESRDY
    DMAC_REGS->DMAC_CHID = 0;
    DMAC_REGS->DMAC_CHCTRLB = 
        DMAC_CHCTRLB_TRIGSRC(DMAC_CHCTRLB_TRIGSRC_ADC0_RESRDY_Val) |
        DMAC_CHCTRLB_TRIGACT(DMAC_CHCTRLB_TRIGACT_BEAT_Val) |
        DMAC_CHCTRLB_LVL(DMAC_CHCTRLB_LVL_LVL3_Val);
    DMAC_REGS->DMAC_CHINTENSET = DMAC_CHINTENCLR_TERR(1);
    DMAC_REGS->DMAC_CHCTRLA = DMAC_CHCTRLA_ENABLE(1);
    
    
    // configure channel 1
    // trigger a beat on ADC1 RESRDY
    DMAC_REGS->DMAC_CHID = 1;
    DMAC_REGS->DMAC_CHCTRLB = 
        DMAC_CHCTRLB_TRIGSRC(DMAC_CHCTRLB_TRIGSRC_ADC1_RESRDY_Val) |
        DMAC_CHCTRLB_TRIGACT(DMAC_CHCTRLB_TRIGACT_BEAT_Val) |
        DMAC_CHCTRLB_LVL(DMAC_CHCTRLB_LVL_LVL3_Val);
    DMAC_REGS->DMAC_CHINTENSET = DMAC_CHINTENCLR_TERR(1);
    DMAC_REGS->DMAC_CHCTRLA = DMAC_CHCTRLA_ENABLE(1);

}
    
static void DMAC_init_descriptors() {
    // initialize descriptor for channel 0
    // to copy ADC results to RAM, we need word sized beats
    // destination address has to be increased
    // source address (=ADC result register) remains constant
    // after completion, channel will be disabled
    DMAC_descriptor[0].DMAC_BTCTRL =
        DMAC_BTCTRL_STEPSIZE(DMAC_BTCTRL_STEPSIZE_X1_Val) |
        DMAC_BTCTRL_STEPSEL(DMAC_BTCTRL_STEPSEL_DST_Val) |
        DMAC_BTCTRL_DSTINC(1) |
        DMAC_BTCTRL_SRCINC(0) |
        DMAC_BTCTRL_BEATSIZE(DMAC_BTCTRL_BEATSIZE_HWORD_Val) |
        DMAC_BTCTRL_BLOCKACT(DMAC_BTCTRL_BLOCKACT_NOACT_Val) |
        DMAC_BTCTRL_EVOSEL(DMAC_BTCTRL_EVOSEL_DISABLE_Val) |
        DMAC_BTCTRL_VALID(1);
    // we have 4 AD channels to store, then cycle repeats
    DMAC_descriptor[0].DMAC_BTCNT = 4;
    // source address is ADC0 result register
    DMAC_descriptor[0].DMAC_SRCADDR = (uint32_t) (&(ADC0_REGS->ADC_RESULT));
    // destination address is an array in RAM
    DMAC_descriptor[0].DMAC_DSTADDR = (uint32_t) (adc_result+5);
    // no link to next descriptor
    DMAC_descriptor[0].DMAC_DESCADDR = 0;
        
    // initialize descriptor for channel 1
    // to copy ADC results to RAM, we need word sized beats
    // destination address has to be increased
    // source address (=ADC result register) remains constant
    // after completion, channel will be disabled
    DMAC_descriptor[1].DMAC_BTCTRL =
        DMAC_BTCTRL_STEPSIZE(DMAC_BTCTRL_STEPSIZE_X1_Val) |
        DMAC_BTCTRL_STEPSEL(DMAC_BTCTRL_STEPSEL_DST_Val) |
        DMAC_BTCTRL_DSTINC(1) |
        DMAC_BTCTRL_SRCINC(0) |
        DMAC_BTCTRL_BEATSIZE(DMAC_BTCTRL_BEATSIZE_HWORD_Val) |
        DMAC_BTCTRL_BLOCKACT(DMAC_BTCTRL_BLOCKACT_NOACT_Val) |
        DMAC_BTCTRL_EVOSEL(DMAC_BTCTRL_EVOSEL_DISABLE_Val) |
        DMAC_BTCTRL_VALID(1);
    // we have 1 AD channel to store, then cycle repeats
    DMAC_descriptor[1].DMAC_BTCNT = 1;
    // source address is ADC1 result register
    DMAC_descriptor[1].DMAC_SRCADDR = (uint32_t) (&(ADC1_REGS->ADC_RESULT));
    // destination address is an array in RAM
    DMAC_descriptor[1].DMAC_DSTADDR = (uint32_t) (adc_result+1);
    // no link to next descriptor
    DMAC_descriptor[1].DMAC_DESCADDR = 0;
}
