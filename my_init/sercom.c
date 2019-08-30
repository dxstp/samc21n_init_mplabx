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
#include "sercom.h"

/** 
 * init the SERCOM2 module to 115200 baud, 8N1
 */
void SERCOM4_init(void) {

	// unmask SERCOM4 in MCLK to enable clock to user interface
	MCLK->APBCMASK.reg |= MCLK_APBCMASK_SERCOM4;
	
	// connect GLCK0 with SERCOM4 module (core clock)
	GCLK->PCHCTRL[SERCOM4_GCLK_ID_CORE].reg = GCLK_PCHCTRL_GEN_GCLK0 | GCLK_PCHCTRL_CHEN;
	
	// do a software reset of the module (write-synchronized)
	SERCOM4->USART.CTRLA.reg = SERCOM_USART_CTRLA_SWRST;
	while (SERCOM4->USART.SYNCBUSY.bit.SWRST);

	// configure UART mode and bit order
	SERCOM4->USART.CTRLA.reg =
		  SERCOM_USART_CTRLA_MODE(1)
		| SERCOM_USART_CTRLA_RXPO(3)
		| SERCOM_USART_CTRLA_TXPO(1)
		| SERCOM_USART_CTRLA_DORD;

	// enable transmitter and receiver
	SERCOM4->USART.CTRLB.reg =
		  SERCOM_USART_CTRLB_TXEN
		| SERCOM_USART_CTRLB_RXEN;
	while(SERCOM4->USART.SYNCBUSY.bit.CTRLB);

	// setup baud rate
	SERCOM4->USART.BAUD.reg = CONF_SERCOM_4_USART_BAUD_RATE_REGISTER_VAL;

	// enable UART
	SERCOM4->USART.CTRLA.reg |= SERCOM_USART_CTRLA_ENABLE;
	while(SERCOM4->USART.SYNCBUSY.bit.ENABLE);
}

int32_t SERCOM4_write(const char *const buf, const uint32_t length) {
	uint32_t offset = 0;
	
	while(!(SERCOM4->USART.INTFLAG.bit.DRE));
	
	do {
		SERCOM4->USART.DATA.reg = buf[offset];
		while(!(SERCOM4->USART.INTFLAG.bit.DRE));
	} while (++offset < length);
	
	while(!(SERCOM4->USART.INTFLAG.bit.TXC));
	
	return (int32_t)offset;
}

int32_t SERCOM4_read(char *const buf, const uint32_t length) {
	uint32_t offset = 0;
	
	do {
		while(!(SERCOM4->USART.INTFLAG.bit.RXC));
		buf[offset] = SERCOM4->USART.DATA.reg;
	} while (++offset < length);

	return (int32_t)offset;
}

int32_t SERCOM4_IsDataAvailable(void)
{
	return (SERCOM4->USART.INTFLAG.reg & SERCOM_USART_INTFLAG_RXC) ? 1 : 0;
}