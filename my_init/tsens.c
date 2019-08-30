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
#include "tsens.h"

// configure damping of the measurement
static const int32_t tsensFilterStrength = 128;

static int32_t tsensTemperatureFiltered, tsensFilterSum;

static int32_t convert_signed_24_bit(int32_t value);

void TSENS_init(void) {
	// unmask TSENS in MCLK to enable clock to user interface
	MCLK->APBAMASK.reg |= MCLK_APBAMASK_TSENS;
	 
	// connect GLCK0 with TSENS module (core clock)
	GCLK->PCHCTRL[TSENS_GCLK_ID].reg = GCLK_PCHCTRL_GEN_GCLK0 | GCLK_PCHCTRL_CHEN;
	 
	// do a software reset of the module (write-synchronized)
	TSENS->CTRLA.reg = TSENS_CTRLA_SWRST;
	while (TSENS->SYNCBUSY.bit.SWRST);
	 
	// load factory calibration values into TSENS registers
	// TSENS is calibrated for usage with undivided OSC48M as GCLK source
	TSENS->GAIN.reg = 
		((*(uint32_t *) TSENS_FUSES_GAIN_0_ADDR & TSENS_FUSES_GAIN_0_Msk) >> TSENS_FUSES_GAIN_0_Pos) |
		((*(uint32_t *) TSENS_FUSES_GAIN_1_ADDR & TSENS_FUSES_GAIN_1_Msk) >> TSENS_FUSES_GAIN_1_Pos);
	TSENS->OFFSET.reg =
		((*(uint32_t *) TSENS_FUSES_OFFSET_ADDR & TSENS_FUSES_OFFSET_Msk) >> TSENS_FUSES_OFFSET_Pos);
	TSENS->CAL.reg =
		TSENS_CAL_FCAL((*(uint32_t *) TSENS_FUSES_FCAL_ADDR & TSENS_FUSES_FCAL_Msk) >> TSENS_FUSES_FCAL_Pos) |
		TSENS_CAL_TCAL((*(uint32_t *) TSENS_FUSES_TCAL_ADDR & TSENS_FUSES_TCAL_Msk) >> TSENS_FUSES_TCAL_Pos);
	
	// configure TSENS to run in free running mode
	TSENS->CTRLC.reg |= TSENS_CTRLC_FREERUN;
	
	// enable TSENS
	TSENS->CTRLA.reg |= TSENS_CTRLA_ENABLE;
	while(TSENS->SYNCBUSY.bit.ENABLE);
	 
	// start measurement
	TSENS->CTRLB.reg = TSENS_CTRLB_START;
	
	// get first measurement to preload filter
	while(!TSENS->INTFLAG.bit.RESRDY);
	tsensFilterSum = convert_signed_24_bit(TSENS->VALUE.reg) * tsensFilterStrength;
	tsensTemperatureFiltered = convert_signed_24_bit(TSENS->VALUE.reg);
	
	// enable TSENS interrupts
	TSENS->INTFLAG.reg = TSENS_INTFLAG_RESRDY;
	TSENS->INTENSET.reg = TSENS_INTFLAG_RESRDY;
	 
}

void TSENS_Handler() {
	if(TSENS->INTFLAG.bit.RESRDY) {
		// moving exponential average to get stable result
		tsensFilterSum -= tsensFilterSum / tsensFilterStrength;
		tsensFilterSum += convert_signed_24_bit(TSENS->VALUE.reg);
		tsensTemperatureFiltered = tsensFilterSum / tsensFilterStrength;
		
		// clear interrupt
		TSENS->INTFLAG.reg = TSENS_INTFLAG_RESRDY;
	}
}

double getInternalTemperatureFiltered(void) {
	return (double) tsensTemperatureFiltered / 100.0;
}

static int32_t convert_signed_24_bit(int32_t value) {
	if(value & 0x800000) {
		value |= 0xFF000000;
	}
	
	return value;
}