#include "ch32fun.h"
#include <stdio.h>

// MOSI   PC6
// SCK    PC5
// OE/LATCH* PC2

// UART RX PD6
// LED  PC1

#define LED (1<<1)
#define OE (1<<2)

#define PAGESIZE (13*4)

#define pulseLength 2000
#define tempdelay 500



// 150/(470+150) = 0.242
// 3S lipo, cut out at 9.0V
// 9.0*150/(470+150) *1023/3.3 = 675 

#define VBAT_LOW       650 // ~9.0
#define VBAT_RELEASE   700 // ~9.6

static inline uint16_t read_adc(){
	ADC1->CTLR2 |= ADC_SWSTART;
	while(!(ADC1->STATR & ADC_EOC));
	return ADC1->RDATAR;//[0...1023]
}

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
	GPIOC->OUTDR &= ~LED;

	for (int i=0;i<13*4;i++) {
		// wait for TXE
		while(!(SPI1->STATR & SPI_STATR_TXE));
		// Send byte
		SPI1->DATAR = data[i];
		while(SPI1->STATR & SPI_STATR_BSY);
	}

	// best time to check battery is right before a pulse
	if (read_adc() < VBAT_LOW ) {
		do {
			Delay_Ms(500);
			GPIOC->OUTDR |= LED;
			Delay_Ms(500);
			GPIOC->OUTDR &= ~LED;
			//printf("adc %d\n",read_adc());
		} while (read_adc()<VBAT_RELEASE);
	} else {
		GPIOC->OUTDR &= ~OE;
		Delay_Us(pulseLength);
		GPIOC->OUTDR |= OE;
	}
	GPIOC->OUTDR |= LED;
}

void repulse_delay(int d, int s){
	for (int i=0;i<d;i+=s) {
		Delay_Ms(s);
		GPIOC->OUTDR &= ~OE;
		Delay_Us(pulseLength);
		GPIOC->OUTDR |= OE;
	}
}

void init_adc()
{
	RCC->APB2PCENR |= RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1;

	// PA1 is analog input chl 1
	GPIOA->CFGLR &= ~(0xf<<(4*1));	// CNF = 00: Analog, MODE = 00: Input

	RCC->APB2PRSTR |= RCC_APB2Periph_ADC1;
	RCC->APB2PRSTR &= ~RCC_APB2Periph_ADC1;

	RCC->CFGR0 &= ~(0x1F<<11); //RCC_ADCPRE = 0
	ADC1->RSQR1 = 0;
	ADC1->RSQR2 = 0;
	ADC1->RSQR3 = 1;
	// sampling time
	ADC1->SAMPTR2 &= ~(ADC_SMP0<<(3*7));
	ADC1->SAMPTR2 |= 7<<(3*7);

	// turn on, set sw trig
	ADC1->CTLR2 |= ADC_ADON | ADC_EXTSEL;
	// reset calibration
	ADC1->CTLR2 |= ADC_RSTCAL;
	while(ADC1->CTLR2 & ADC_RSTCAL);
	// calibrate
	ADC1->CTLR2 |= ADC_CAL;
	while(ADC1->CTLR2 & ADC_CAL);
}

uint8_t page[PAGESIZE] = {};

int main()
{
	SystemInit();


	RCC->APB2PCENR |= RCC_APB2Periph_GPIOC | RCC_APB2Periph_SPI1 | RCC_APB2Periph_GPIOD | RCC_APB2Periph_USART1;

	GPIOC->CFGLR &= ~((0xf<<(4*6)) | (0xf<<(4*5)) | (0xf<<(4*2)) | (0xf<<(4*1)) );
	GPIOC->CFGLR |=((GPIO_Speed_10MHz | GPIO_CNF_OUT_PP_AF)<<(4*5))
				 | ((GPIO_Speed_10MHz | GPIO_CNF_OUT_PP_AF)<<(4*6))
				 | ((GPIO_Speed_10MHz | GPIO_CNF_OUT_PP)<<(4*1))
				 | ((GPIO_Speed_10MHz | GPIO_CNF_OUT_PP)<<(4*2));

	GPIOC->OUTDR |= OE;

	// ID pins on D0,D2,D3,D4,D5
	GPIOD->CFGLR &= ~((0xf<<(4*0)) | (0xf<<(4*2)) | (0xf<<(4*3)) | (0xf<<(4*4)) | (0xf<<(4*5)) );
	GPIOD->CFGLR |=((GPIO_Speed_In | GPIO_CNF_IN_PUPD)<<(4*0))
				 | ((GPIO_Speed_In | GPIO_CNF_IN_PUPD)<<(4*2))
				 | ((GPIO_Speed_In | GPIO_CNF_IN_PUPD)<<(4*3))
				 | ((GPIO_Speed_In | GPIO_CNF_IN_PUPD)<<(4*4))
				 | ((GPIO_Speed_In | GPIO_CNF_IN_PUPD)<<(4*5));
	GPIOD->BSHR = GPIO_BSHR_BS0 |
				  GPIO_BSHR_BS2 |
				  GPIO_BSHR_BS3 |
				  GPIO_BSHR_BS4 |
				  GPIO_BSHR_BS5;

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
	USART1->BRR = 48; // 1000kbps
	USART1->CTLR1 |= CTLR1_UE_Set;

	init_adc();

	GPIOC->OUTDR |= LED;

	int frame = ((GPIOD->INDR) &1) | (((GPIOD->INDR) >>1)&0b11110);


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

}
