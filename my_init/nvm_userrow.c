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

/* NVM User Row definitions
 */

/* WORD0: BOOTPROT, EEPROM, BOD and WDT */
#define NVM_USER_WORD_0                                                        \
	NVMCTRL_FUSES_BOOTPROT(7) |												   \
	(1 << 3) |																   \
	NVMCTRL_FUSES_EEPROM_SIZE(7) |							        		   \
	(1 << 7) |							        		                       \
	FUSES_BODVDDUSERLEVEL(48) |  						        		       \
	(0 << FUSES_BODVDD_DIS_Pos) |							        		   \
	FUSES_BODVDD_ACTION(2) |							        		       \
	(0xA8 << 17) |							        		                   \
	(0 << WDT_FUSES_ENABLE_Pos) |							        		   \
	(0 << WDT_FUSES_ALWAYSON_Pos) |							        		   \
	WDT_FUSES_PER(0xB)
	 
/* WORD1: WDT, BOD, LOCK */
#define NVM_USER_WORD_1                                                        \
	WDT_FUSES_WINDOW(0xB) |                                                    \
	WDT_FUSES_EWOFFSET(0xB) |                                                  \
	(0 << WDT_FUSES_WEN_Pos) |                                                 \
	(0 << FUSES_BODVDD_HYST_Pos) |                                             \
	(0 << 10) |                                                                \
	(0x1F << 11) |                                                             \
	NVMCTRL_FUSES_REGION_LOCKS(0xFFFF)

#if defined(__GNUC__) /* GCC */
/* "userrow" section should be correctly defined in your linker file */
__attribute__((section(".userrow"))) const uint32_t nvm_user_row[] =
#elif defined(__ICCARM__) /* IAR EWARM */
/* "nvm_user_page" section should be correctly defined in your linker file */
__root const uint32_t nvm_user_row[] @ ".nvm_user_page" =
#else
#warning "This IDE can't support USER Row fuse programming."
#endif
    {NVM_USER_WORD_0, NVM_USER_WORD_1};

