#include "stm32f4xx.h"
#include "system_stm32f4xx.h"

/*!< defines_main */
//#define    DWT_CYCCNT    *(volatile unsigned long *)0xE0001004	/*!<*/
//#define    DWT_CONTROL   *(volatile unsigned long *)0xE0001000	/*!<*/
//#define    SCB_DEMCR     *(volatile unsigned long *)0xE000EDFC	/*!<*/
	
/*!< variablen_main */
uint32_t cycles_count = 0x00 ;	/*!< Anzahl der CPU Takten */
uint32_t grenze = 0x00 ;				/*!< Testfunktion zur Grenze in for Schleife */
double time_in_sek = 0.00 ; 		/*!< Zeitabschnitt in Sekunden */

/*!< funktion_prototyps_main */
void ClockInit( void ) ;

int main(void) {
	SystemInit() ;
	ClockInit() ;
	
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk; /*!< Trace enable */

	while(1) {
		//SystemCoreClockUpdate();
		DWT->CYCCNT = 0; 													/*!< Cycle Counter = 0 */
		DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk ;			/*!< Control Register -> Cycle Count enable */
		
		for( uint16_t i = 0 ; i < grenze ; i ++ ) { __NOP( ) ; }
		
		cycles_count = DWT->CYCCNT; /*!< Cycle Counter in cycles_count uebertragen */
		time_in_sek = cycles_count / 168000000.0f ;	/*!< Zeitabschnitt in Sekunden */
	}
}

void ClockInit( void ) {

	RCC->CR |= (RCC_CR_HSEON); 							//Enable HSE

	while( !(RCC->CR & RCC_CR_HSERDY) ) { __nop(); }; //ready to start HSE



	//FLASH

	FLASH->ACR |= FLASH_ACR_LATENCY 		//clock  flash memory

	| FLASH_ACR_PRFTEN

	| FLASH_ACR_ICEN

	| FLASH_ACR_DCEN;

		

	//PLL - HSE

	RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_HSE; //set HSE as PLL source

	RCC->CR &= ~(RCC_CR_PLLON); 						//disable PLL before changes

	

	//PLL /M 4

	RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLM); 		//clear all PLLM bits

	RCC->PLLCFGR |= RCC_PLLCFGR_PLLM_2; 		//set PLLM = 4 (100)



	//PLL /P 2

	RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLP); 		//main PLL division PLLP = 2: 00

	

	//PLL *N 168

	RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLN); 		//clear all PLLN bits with mask

	RCC->PLLCFGR |= RCC_PLLCFGR_PLLN_3; 		//set PLLN = 168 (1010 1000) 

	RCC->PLLCFGR |= RCC_PLLCFGR_PLLN_5; 		//for 8 or 16 MHz HSE

	RCC->PLLCFGR |= RCC_PLLCFGR_PLLN_7; 		//

	

	//PLL Q 7 (0111)

	RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLQ);

	RCC->PLLCFGR |= (RCC_PLLCFGR_PLLQ_0

								 | RCC_PLLCFGR_PLLQ_1

								 | RCC_PLLCFGR_PLLQ_2);

	

	//AHB Prescaler 1

	RCC->CFGR &= ~(RCC_CFGR_HPRE); 					//Prescaler 1

	RCC->CFGR |= RCC_CFGR_HPRE_DIV1; 				//AHB = SYSCLK/1

	

	//APB1 Prescaler 4 

	RCC->CFGR &= ~(RCC_CFGR_PPRE1);

	RCC->CFGR |= RCC_CFGR_PPRE1_DIV4;

	

	//APB2 Prescaler 2

	RCC->CFGR &= ~(RCC_CFGR_PPRE2);

	RCC->CFGR |= RCC_CFGR_PPRE2_DIV2;

	

	//PLL enable

	RCC->CR |= RCC_CR_PLLON; 								//enalbe PLL

	while((RCC->CR & RCC_CR_PLLRDY) == 0) { __nop( ) ; } 	//wait for PLL is ready

	

	//PLL System

	//RCC->CFGR &= ~RCC_CFGR_SW;

	RCC->CFGR |= RCC_CFGR_SW_PLL; 					//PLL selected as system clock

	while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) { __nop( ) ; } 	//wait for PLL is used

	//return 0x0000 ;
}
