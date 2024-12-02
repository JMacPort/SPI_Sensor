// Used as reference for SD card commands https://chlazza.nfshost.com/sdcardinfo.html

#include "main.h"

void SPI_Init();

int main() {

}

void SPI_Init() {
	RCC -> AHB1ENR |= (1 << 0);															// GPOPA Clock
	RCC -> APB2ENR |= (1 << 12);														// SPI1 Clock

	GPIOA -> MODER &= ~(3 << (2 * 4));													// PA4 for output/SD card and generates a high signal; bit amount * pin number
	GPIOA -> MODER |= (1 << (2 * 4));
	GPIOA -> ODR |= (1 << 4);

	GPIOA -> MODER &= ~((3 << (2 * 5)) | (3 << (2 * 6)) | (3 << (2 * 7))); 				// PA5, PA6 & PA7 set for Alternate Function
	GPIOA -> MODER |= ((2 << (2 * 5)) | (2 << (2 * 6)) | (2 << (2 * 7)));

	GPIOA -> AFR[0] &= ~((15 << (4 * 5)) | (15 << (4 * 6)) | (15 << (4 * 7)));			// PA5, PA6 & PA7 set for AFR5; SPI1
	GPIOA -> AFR[0] |= (5 << (4 * 5)) | (5 << (4 * 6)) | (5 << (4 * 7));

	SPI1 -> CR1 = 0;																	// Resets SPI1
	SPI1 -> CR1 |= (1 << 2);															// Sets Master mode
	SPI1 -> CR1 |= (5 << 3);															// /64 for initialization of SD card
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
	GPIOA -> ODR &= ~(1 << 4);															// Generates a low output
}

void SD_Deselect() {
	GPIOA -> ODR |= (1 << 4);															// Generates a high output
}

// Allows the SD card to be put into SPI mode
void SD_PowerUp() {
	for (int i = 0; i < 10; i++) {
		SPI_Transfer(0xFF);																// Creates 8 clock pulses each iteration, needs about 74 to ensure wake up
	}
}

// Puts SD card into SPI mode, brings it to an idle state and returns response code
uint8_t SD_SendCMD0() {

	SPI_Transfer(0xFF);																	// Extra clock cycle

	SD_Select();																		// Selects SD card

	SPI_Transfer(0x40);																	// CMD0 begin
	SPI_Transfer(0x00);
	SPI_Transfer(0x00);
	SPI_Transfer(0x00);
	SPI_Transfer(0x00);
	SPI_Transfer(0x95);																	// CRC required for CMD0

	uint8_t response;

	// Gives the SD card 80 cycles to respond
	for (int i = 0; i < 10; i++) {
		response = SPI_Transfer(0xFF);
		if (response != 0xFF) break;
	}

	SD_Deselect();																		// Deselects SD card and runs a cycle
	SPI_Transfer(0xFF);

	return response;																	// Should be 0x01 if successful
}

uint8_t SD_SendCMD8() {

	SD_Select();

	SPI_Transfer(0x48);																	// CMD8 begin 72 + 0 + 0 + 1 + 170
	SPI_Transfer(0x00);
	SPI_Transfer(0x00);
	SPI_Transfer(0x01);
	SPI_Transfer(0xAA);
	SPI_Transfer(0x87);

	uint8_t response[5];

	for (int i = 0; i < 5; i++) {
		response[i] = SPI_Transfer(0xFF);
	}

	SD_Deselect();
	SPI_Transfer(0xFF);

	return response[0];
}
