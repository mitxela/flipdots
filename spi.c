#include "ch32v003fun.h"
#include <stdio.h>

// MOSI   PC6
// SCK    PC5
// LATCH  PC1
// OE/LATCH* PC2

#define LATCH (1<<1)
#define OE (1<<2)

void send_spi(uint8_t data) {
	GPIOC->OUTDR &= ~LATCH;
	GPIOC->OUTDR |= OE;

	// wait for TXE
	while(!(SPI1->STATR & SPI_STATR_TXE));
	// Send byte
	SPI1->DATAR = data;

	while(SPI1->STATR & SPI_STATR_BSY);


	GPIOC->OUTDR |= LATCH;
	GPIOC->OUTDR &= ~OE;
	Delay_Us(5000);
	GPIOC->OUTDR &= ~LATCH;
	GPIOC->OUTDR |= OE;
}


int main()
{
	SystemInit();


	RCC->APB2PCENR |= RCC_APB2Periph_GPIOC | RCC_APB2Periph_SPI1;

	GPIOC->CFGLR &= ~((0xf<<(4*6)) | (0xf<<(4*5)) | (0xf<<(4*2)) | (0xf<<(4*1)));
	GPIOC->CFGLR |=((GPIO_Speed_10MHz | GPIO_CNF_OUT_PP_AF)<<(4*5))
				 | ((GPIO_Speed_10MHz | GPIO_CNF_OUT_PP_AF)<<(4*6))
				 | ((GPIO_Speed_10MHz | GPIO_CNF_OUT_PP)<<(4*2))
				 | ((GPIO_Speed_10MHz | GPIO_CNF_OUT_PP)<<(4*1));

	// Configure SPI 
	SPI1->CTLR1 = 
		SPI_NSS_Soft | SPI_CPHA_1Edge | SPI_CPOL_Low | SPI_DataSize_8b |
		SPI_Mode_Master | SPI_Direction_1Line_Tx |
		SPI_BaudRatePrescaler_32;

	// enable SPI port
	SPI1->CTLR1 |= CTLR1_SPE_Set;


	while(1) {
		Delay_Ms(500);
		send_spi(0xF5);
		Delay_Ms(500);
		send_spi(0x0A);

//		Delay_Ms(500);
	}
}
