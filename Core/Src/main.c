// Used as reference for SD card commands https://chlazza.nfshost.com/sdcardinfo.html

#include "main.h"
#include <stdio.h>

void SPI_Init();
void USART_Init();
uint8_t SPI_Transfer(uint8_t);
uint8_t SD_Init();
void SD_PowerUp();
uint8_t SD_SendCMD0();
uint8_t SD_SendCMD8();
uint8_t SD_SendCMD58();
uint8_t SD_SendCMD55();
uint8_t SD_SendACMD41();
uint8_t SD_Card_Init();
uint8_t SD_Init();

// Need to order new SD card modules, think I shorted them both :)
int main() {
	SPI_Init();
	USART_Init();
	printf("READY\r\n");

	// Should return 0 if sequence powers up correctly
	uint8_t miso_state = SPI_Transfer(0xFF);
	printf("Initial MISO: 0x%02X\n", miso_state);

	while (1) {

	}

}

void SPI_Init() {
	RCC -> AHB1ENR |= (1 << 0);															// GPOPA Clock
	RCC -> AHB1ENR |= (1 << 1);															// GPOPB Clock
	RCC -> APB2ENR |= (1 << 12);														// SPI1 Clock

	GPIOB -> MODER &= ~(3 << (2 * 6));													// PB6 for output/SD card and generates a high signal; bit amount * pin number
	GPIOB -> MODER |= (1 << (2 * 6));
	GPIOB -> ODR |= (1 << 6);

	GPIOA -> MODER &= ~((3 << (2 * 5)) | (3 << (2 * 6)) | (3 << (2 * 7))); 				// PA5, PA6 & PA7 set for Alternate Function
	GPIOA -> MODER |= ((2 << (2 * 5)) | (2 << (2 * 6)) | (2 << (2 * 7)));

	GPIOA->PUPDR &= ~(3 << (2 * 6));  													// Set pull-up
	GPIOA->PUPDR |= (1 << (2 * 6));

	GPIOA -> AFR[0] &= ~((15 << (4 * 5)) | (15 << (4 * 6)) | (15 << (4 * 7)));			// PA5, PA6 & PA7 set for AFR5; SPI1
	GPIOA -> AFR[0] |= (5 << (4 * 5)) | (5 << (4 * 6)) | (5 << (4 * 7));

	SPI1 -> CR1 = 0;																	// Resets SPI1
	SPI1 -> CR1 |= (1 << 2);															// Sets Master mode
	SPI1 -> CR1 |= (7 << 3);															// /256 for initialization of SD card
	SPI1 -> CR1 |= (1 << 8);															// Used to select any pin for NSS instead of hardware
	SPI1 -> CR1 |= (1 << 9);															// Enables SSM
	SPI1 -> CR1 |= (1 << 6);															// SPI1 Enable
}

// Actual SPI Data Transfer
uint8_t SPI_Transfer(uint8_t data) {
    while(!(SPI1->SR & (1 << 1)));    													// Waits for TXE bit
    SPI1->DR = data;                  													// Sends data from register
    while(!(SPI1->SR & (1 << 0)));   													// Wait for RXNE bit
    return SPI1->DR;                  													// Returns the received byte
}

void SD_Select() {
	GPIOB -> ODR &= ~(1 << 6);															// Generates a low output
}

void SD_Deselect() {
	GPIOB -> ODR |= (1 << 6);															// Generates a high output
}

// Allows the SD card to be put into SPI mode
void SD_PowerUp() {
	for (int i = 0; i < 10; i++) {
		SPI_Transfer(0xFF);																// Creates 8 clock pulses each iteration, needs about 74 to ensure wake up
	}
}

// Puts SD card into SPI mode, brings it to an idle state and returns response code
uint8_t SD_SendCMD0() {
    printf("Starting CMD0\r\n");
    SPI_Transfer(0xFF);

    SD_Select();
    printf("CS Low\r\n");

    SPI_Transfer(0x40);
    SPI_Transfer(0x00);
    SPI_Transfer(0x00);
    SPI_Transfer(0x00);
    SPI_Transfer(0x00);
    SPI_Transfer(0x95);
    printf("CMD0 sent\r\n");

    uint8_t response;
    for (int i = 0; i < 10; i++) {
        response = SPI_Transfer(0xFF);
        printf("Try %d: 0x%02X\r\n", i, response);
        if (response != 0xFF) break;
    }

    SD_Deselect();
    return response;
}

// Checks if SD card can operate at current device voltage; required for v2+
// Implement error checking to make sure the check pattern and voltage is enough
uint8_t SD_SendCMD8() {

	SPI_Transfer(0xFF);

	SD_Select();

	SPI_Transfer(0x48);																	// CMD8
	SPI_Transfer(0x00);																	// Reserved
	SPI_Transfer(0x00);																	// Reserved
	SPI_Transfer(0x01);																	// Voltage level 2.7-3.6V
	SPI_Transfer(0xAA);																	// Check pattern
	SPI_Transfer(0x87);																	// CRC

	uint8_t response[5];																// R7 Format - Returns voltage info and check pattern

	for (int i = 0; i < 5; i++) {
		response[i] = SPI_Transfer(0xFF);
	}

	SD_Deselect();																		// Deselects SD card and runs a cycle
	SPI_Transfer(0xFF);

	printf("CMD8: %d\r\n",response[1]);
	return response[1];																	// Should be 0x01 if successful
}

// Returns a 5 byte response containing voltage and capacity information
uint8_t SD_SendCMD58() {

	SPI_Transfer(0xFF);

	SD_Select();

	SPI_Transfer(0x7A);    																// CMD58
	SPI_Transfer(0x00);    																// Reserved
	SPI_Transfer(0x00);    																// Reserved
	SPI_Transfer(0x00);    																// Reserved
	SPI_Transfer(0x00);    																// Reserved
	SPI_Transfer(0xFF);    																// No CRC required

	uint8_t response[5];

	for (int i = 0; i < 5; i++) {
		response[i] = SPI_Transfer(0xFF);
	}

	SD_Deselect();
	SPI_Transfer(0xFF);

	return response[0];																	// Should be 0x01
}

// Tells card that next command is application specific
// Implement error checking
uint8_t SD_SendCMD55() {

	SPI_Transfer(0xFF);

	SD_Select();

	SPI_Transfer(0x77);																	// CMD55
	SPI_Transfer(0x00);																	// Reserved
	SPI_Transfer(0x00);																	// Reserved
	SPI_Transfer(0x00);																	// Reserved
	SPI_Transfer(0x00);																	// Reserved
	SPI_Transfer(0xFF);																	// No CRC required

	uint8_t response;

	for (int i = 0; i < 10; i++) {
		response = SPI_Transfer(0xFF);
		if (response != 0xFF) break;
		for (volatile int j = 0; j < 10000; j++);
	}

	SD_Deselect();
	SPI_Transfer(0xFF);

	printf("CMD55: %d\r\n",response);
	return response;																	// Should be 0x01
}

// Activates SD card initialization process; must follow CMD55 for each attempt
uint8_t SD_SendACMD41() {

	SPI_Transfer(0xFF);

	SD_Select();

	SPI_Transfer(0x69);																	// ACMD41
	SPI_Transfer(0x40);																	// HCS bit
	SPI_Transfer(0x00);																	// Reserved
	SPI_Transfer(0x00);																	// Reserved
	SPI_Transfer(0x00);																	// Reserved
	SPI_Transfer(0xFF);																	// No CRC required

	uint8_t response;

	for (int i = 0; i < 50; i++) {
		response = SPI_Transfer(0xFF);
		if (response != 0xFF) break;
		for (volatile int j = 0; j < 100000; j++);
	}

	SD_Deselect();
	SPI_Transfer(0xFF);

	printf("ACMD41: %d\r\n",response);
	return response;
}

// Initializes the SD card and keeps sending the commands until initialization completes
uint8_t SD_Card_Init() {
	uint8_t response;

	do {
		response = SD_SendCMD55();
		if (response != 0x01) return response;

		response = SD_SendACMD41();
	} while (response == 0x01);

	return response;
}

// Full SD card setup
uint8_t SD_Init() {
	uint8_t response;

	SD_PowerUp();

	response = SD_SendCMD0();
	if(response != 0x01) return response;

	response = SD_SendCMD8();
	if (response != 0x01) return response;

	response = SD_Card_Init();
	if (response != 0x00) return response;

	return 0;
}

void USART_Init() {
	RCC -> APB1ENR |= (1 << 17);															// USART2 Clock
	RCC -> AHB1ENR |= (1 << 0);																// GPIOA Clock; should be already enabled

	GPIOA -> MODER &= ~((3 << (2 * 2)) | (3 << (2 * 3)));									// PA2 & PA3 to Alternate Function
	GPIOA -> MODER |= (2 << (2 * 2)) | (2 << (2 * 3));

	GPIOA -> AFR[0] |= (7 << (4 * 2)) | (7 << (4 * 3));										// Sets alternate function for USART2

	USART2 -> BRR = 0x0683;																	// Baud rate set to 9600

	USART2 -> CR1 |= (1 << 2) | (1 << 3);													// Receiver & Transmitter Enabled
	USART2 -> CR1 |= (1 << 13);																// USART2 Enabled

	printf("USART configured...........\r\n");
}

// printf() retargeting
int __io_putchar(int c) {
	while(!(USART2 -> SR & (1 << 7)));
	USART2 -> DR = c;

	while(!(USART2 -> SR & (1 << 6)));
	USART2 -> SR &= ~(1 << 6);
	return c;
}
