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
#include "port.h"

enum gpio_port { GPIO_PORTA, GPIO_PORTB, GPIO_PORTC };

/**
 * init the GPIO module
 */
void PORT_init(void) {
	// turn on LED0
	// SAMC21N Xplained Pro:
	// PC05 = USER LED 0 
 	PORT->Group[GPIO_PORTC].DIRSET.reg = (1 << 5);
	PORT->Group[GPIO_PORTC].OUTCLR.reg = (1 << 5);
	
	// turn on SERCOM functionality
	// SAMC21N Xplained Pro:
	// PB10 = SERCOM4, Pad[2], SAMC21N UART TX line
	// PB11 = SERCOM4, Pad[3], SAMC21N UART RX line
	PORT->Group[GPIO_PORTB].WRCONFIG.reg =
		  PORT_WRCONFIG_PMUX(MUX_PB10D_SERCOM4_PAD2)
		| PORT_WRCONFIG_WRPMUX
	    | PORT_WRCONFIG_PMUXEN
		| PORT_WRCONFIG_WRPINCFG
	    | ((1 << 10) & 0xffff);
	PORT->Group[GPIO_PORTB].WRCONFIG.reg =
		  PORT_WRCONFIG_PMUX(MUX_PB11D_SERCOM4_PAD3)
        | PORT_WRCONFIG_WRPMUX
		| PORT_WRCONFIG_PMUXEN
		| PORT_WRCONFIG_WRPINCFG
		| ((1 << 11) & 0xffff);
		
	// turn on PWM functionality
	// SAMC21N Xplained Pro:
	// PB13 = TC4/WO[1] = EXT1, Pin 8
	PORT->Group[GPIO_PORTB].DIRSET.reg = (1 << 13);
	PORT->Group[GPIO_PORTB].WRCONFIG.reg =
		  PORT_WRCONFIG_PMUX(MUX_PB13E_TC4_WO1)
		| PORT_WRCONFIG_WRPMUX
		| PORT_WRCONFIG_PMUXEN
		| PORT_WRCONFIG_WRPINCFG
		| ((1 << 13) & 0xffff);
		
	// turn on GCLK1 output
	// SAMC21N Xplained Pro:
	// PB23 = EXT3, Pin 8
	PORT->Group[GPIO_PORTB].DIRSET.reg = (1 << 23);
	PORT->Group[GPIO_PORTB].WRCONFIG.reg =
		  PORT_WRCONFIG_PMUX(MUX_PB23H_GCLK_IO1)
		| PORT_WRCONFIG_WRPMUX
		| PORT_WRCONFIG_PMUXEN
		| PORT_WRCONFIG_WRPINCFG
		| PORT_WRCONFIG_HWSEL
		| ((1 << 7) & 0xffff);
}