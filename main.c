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
#include <stdio.h>
#include "my_init/supc.h"
#include "my_init/nvmctrl.h"
#include "my_init/oscctrl.h"
#include "my_init/nvic.h"
#include "my_init/gclk.h"
#include "my_init/port.h"
#include "my_init/tc.h"
#include "my_init/tsens.h"
#include "my_init/sercom.h"
#include "my_init/adc.h"
#include "my_init/dmac.h"
#include "my_init/evsys.h"
#include "my_init/dsu.h"
#include "utils/print.h"
#include "utils/delay.h"

int32_t temp;

int main(void) {
	SUPC_init();
	NVMCTRL_init();
	OSCCTRL_init();
	NVIC_init();
	GCLK_init();
	PORT_init();
	TC_init();
	TSENS_init();
	SERCOM4_init();
    ADC_init();
    DMAC_init();
    EVSYS_init();
	print_init();
    DSU_init();
	
	printf("Hello C21N World!\r\n");
    
    for (int i = 0; i < 5; i++)
        adc_result[i] = 0x1234;
    
    while (1) {	
		
        PORT_REGS->GROUP[2].PORT_OUTTGL = (1 << 5);
		delay_ms(1000);
        temp = getInternalTemperatureFiltered();
		printf("TSENS Temperature: %d\r\n", temp);
        
        printf("%04x %04x %04x %04x %04x\r\n", adc_result[0], adc_result[1], adc_result[2], adc_result[3],  adc_result[4]);
        
        // trigger AD conversion by software
        ADC1_REGS->ADC_SWTRIG = 1;
        // wait until trigger is synchronised
        while(ADC1_REGS->ADC_SYNCBUSY & ADC_SYNCBUSY_SWTRIG(1));
        // wait until trigger gets cleared by hardware
        // gets cleared if conversion has been started
        while(ADC1_REGS->ADC_SWTRIG & ADC_SWTRIG_START(1));

    }
}

void SYSTEM_Handler() {
	while(1);
}

void HardFault_Handler() {
    while(1);
}

void ADC0_Handler() {
    if(ADC0_REGS->ADC_INTFLAG & ADC_INTFLAG_OVERRUN(1)) {
        ADC0_REGS->ADC_INTFLAG = ADC_INTFLAG_OVERRUN(1);
    }
}   

void ADC1_Handler() {
    if(ADC1_REGS->ADC_INTFLAG & ADC_INTFLAG_OVERRUN(1)) {
        ADC1_REGS->ADC_INTFLAG = ADC_INTFLAG_OVERRUN(1);
    }
}

void DMAC_Handler() {
_nop();
}