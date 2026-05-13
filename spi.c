#include "ch32fun.h"
#include <stdio.h>

// MOSI   PC6
// SCK    PC5
// LATCH  PC1
// OE/LATCH* PC2

// UART RX PD6

#define LATCH (1<<1)
#define OE (1<<2)

#define PAGESIZE (13*4)

#define pulseLength 2000
#define tempdelay 500


uint8_t uart_rx(){
	while (!(USART1->STATR & USART_STATR_RXNE));
	return USART1->DATAR;
}

void send_spi(uint8_t data) {
	GPIOC->OUTDR |= OE;

	// wait for TXE
	while(!(SPI1->STATR & SPI_STATR_TXE));
	// Send byte
	SPI1->DATAR = data;

	while(SPI1->STATR & SPI_STATR_BSY);

	GPIOC->OUTDR &= ~OE;
	Delay_Us(pulseLength);
	GPIOC->OUTDR |= OE;
}

void send_page(uint8_t * data){

	for (int i=0;i<13*4;i++) {
		// wait for TXE
		while(!(SPI1->STATR & SPI_STATR_TXE));
		// Send byte
		SPI1->DATAR = data[i];
		while(SPI1->STATR & SPI_STATR_BSY);
	}

	GPIOC->OUTDR &= ~OE;
	Delay_Us(pulseLength);
	GPIOC->OUTDR |= OE;

}

void repulse_delay(int d, int s){
	for (int i=0;i<d;i+=s) {
		Delay_Ms(s);
		GPIOC->OUTDR &= ~OE;
		Delay_Us(pulseLength);
		GPIOC->OUTDR |= OE;
	}
}


uint8_t page[PAGESIZE] = {};

int main()
{
	SystemInit();


	RCC->APB2PCENR |= RCC_APB2Periph_GPIOC | RCC_APB2Periph_SPI1 | RCC_APB2Periph_GPIOD | RCC_APB2Periph_USART1;

	GPIOC->CFGLR &= ~((0xf<<(4*6)) | (0xf<<(4*5)) | (0xf<<(4*2)) | (0xf<<(4*1)));
	GPIOC->CFGLR |=((GPIO_Speed_10MHz | GPIO_CNF_OUT_PP_AF)<<(4*5))
				 | ((GPIO_Speed_10MHz | GPIO_CNF_OUT_PP_AF)<<(4*6))
				 | ((GPIO_Speed_10MHz | GPIO_CNF_OUT_PP)<<(4*2))
				 | ((GPIO_Speed_10MHz | GPIO_CNF_OUT_PP)<<(4*1));

	// Configure SPI 
	SPI1->CTLR1 = 
		SPI_NSS_Soft | SPI_CPHA_1Edge | SPI_CPOL_Low | SPI_DataSize_8b |
		SPI_Mode_Master | SPI_Direction_1Line_Tx |
		SPI_BaudRatePrescaler_8;

	// enable SPI port
	SPI1->CTLR1 |= CTLR1_SPE_Set;

	// enable UART
	USART1->CTLR1 = USART_WordLength_8b | USART_Parity_No | USART_Mode_Rx;
	USART1->CTLR2 = USART_StopBits_1;
	USART1->CTLR3 = USART_HardwareFlowControl_None;
	USART1->BRR = 96; // 500kbps
	USART1->CTLR1 |= CTLR1_UE_Set;


	GPIOC->OUTDR |= OE;

	const frame = 1;


	sync:
	if (uart_rx() !='M') goto sync;
	if (uart_rx() !='a') goto sync;
	if (uart_rx() !='g') goto sync;
	if (uart_rx() !='i') goto sync;
	uint8_t totalFrames = uart_rx();


	for (int i=0;i<frame * PAGESIZE;i++) {
		uart_rx();
	}

	for (int i=0;i<PAGESIZE;i++) {
		page[i]=uart_rx();
	}

	for (int i=0;i<(totalFrames-frame-1) * PAGESIZE;i++) {
		uart_rx();
	}

	if (uart_rx() !='\n') goto sync; // maybe between each frame too?

	send_page(page);
	goto sync;


	while(1) {

/*
		send_page(p1);
		Delay_Ms(300);
		send_page(p2);
		Delay_Ms(300);
		send_page(p1);
		Delay_Ms(300);
		send_page(p2);
		Delay_Ms(300);

		send_page(p3);
		repulse_delay(tempdelay, 50);
		send_page(p4);
		repulse_delay(tempdelay, 50);

		send_page(p3);
		repulse_delay(tempdelay, 50);
		send_page(p4);
		repulse_delay(tempdelay, 50);

		send_page(p3);
		repulse_delay(tempdelay, 50);
		send_page(p4);
		repulse_delay(tempdelay, 50);
*/

//		Delay_Ms(500);
	}
}
