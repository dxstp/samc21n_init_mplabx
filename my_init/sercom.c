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
#include <../include/proc/SAMC21/instance/sercom4.h>
#include "sercom.h"

/** 
 * init the SERCOM2 module to 115200 baud, 8N1
 */
void SERCOM4_init(void) {

	// unmask SERCOM4 in MCLK to enable clock to user interface
	MCLK_REGS->MCLK_APBCMASK |= MCLK_APBCMASK_SERCOM4(1);
	
	// connect GLCK0 with SERCOM4 module (core clock)
	GCLK_REGS->GCLK_PCHCTRL[SERCOM4_GCLK_ID_CORE] = GCLK_PCHCTRL_GEN_GCLK0 | GCLK_PCHCTRL_CHEN(1);
	
	// do a software reset of the module (write-synchronized)
	SERCOM4_REGS->USART_INT.SERCOM_CTRLA = SERCOM_USART_INT_CTRLA_SWRST(1);
	while (SERCOM4_REGS->USART_INT.SERCOM_SYNCBUSY & SERCOM_USART_INT_SYNCBUSY_SWRST(1));

	// configure UART mode and bit order
	SERCOM4_REGS->USART_INT.SERCOM_CTRLA =
		  SERCOM_USART_INT_CTRLA_MODE(1)
		| SERCOM_USART_INT_CTRLA_RXPO(3)
		| SERCOM_USART_INT_CTRLA_TXPO(1)
		| SERCOM_USART_INT_CTRLA_DORD(1);

	// enable transmitter and receiver
	SERCOM4_REGS->USART_INT.SERCOM_CTRLB =
		  SERCOM_USART_INT_CTRLB_TXEN(1)
		| SERCOM_USART_INT_CTRLB_RXEN(1);
	while(SERCOM4_REGS->USART_INT.SERCOM_SYNCBUSY & SERCOM_USART_INT_SYNCBUSY_CTRLB(1));

	// setup baud rate
	SERCOM4_REGS->USART_INT.SERCOM_BAUD = CONF_SERCOM_4_USART_BAUD_RATE_REGISTER_VAL;

	// enable UART
	SERCOM4_REGS->USART_INT.SERCOM_CTRLA |= SERCOM_USART_INT_CTRLA_ENABLE(1);
	while(SERCOM4_REGS->USART_INT.SERCOM_SYNCBUSY & SERCOM_USART_INT_SYNCBUSY_ENABLE(1));
}

int32_t SERCOM4_write(const char *const buf, const uint32_t length) {
	uint32_t offset = 0;
	
	while(!(SERCOM4_REGS->USART_INT.SERCOM_INTFLAG & SERCOM_USART_INT_INTFLAG_DRE(1)));
	
	do {
		SERCOM4_REGS->USART_INT.SERCOM_DATA = buf[offset];
		while(!(SERCOM4_REGS->USART_INT.SERCOM_INTFLAG & SERCOM_USART_INT_INTFLAG_DRE(1)));
	} while (++offset < length);
	
	while(!(SERCOM4_REGS->USART_INT.SERCOM_INTFLAG & SERCOM_USART_INT_INTFLAG_TXC(1)));
	
	return (int32_t)offset;
}

int32_t SERCOM4_read(char *const buf, const uint32_t length) {
	uint32_t offset = 0;
	
	do {
		while(!(SERCOM4_REGS->USART_INT.SERCOM_INTFLAG & SERCOM_USART_INT_INTFLAG_RXC(1)));
		buf[offset] = SERCOM4_REGS->USART_INT.SERCOM_DATA;
	} while (++offset < length);

	return (int32_t)offset;
}

int32_t SERCOM4_IsDataAvailable(void)
{
	return (SERCOM4_REGS->USART_INT.SERCOM_INTFLAG & SERCOM_USART_INT_INTFLAG_RXC(1)) ? 1 : 0;
}