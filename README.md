# SPI-based MicroSD Card Interface

## Project Overview
Developed an embedded system to interface with a MicroSD card via the SPI protocol. This project demonstrates low-level SPI communication, card initialization, and the implementation of custom commands required for initializing and configuring the SD card for use. The system is designed for reliable data transfer with an STM32 microcontroller.

## Technical Implementation
- Implemented SPI driver for communication with the MicroSD card
- Developed low-level SD card initialization commands (CMD0, CMD8, CMD55, ACMD41, CMD58)
- Implemented power-up sequence to bring the SD card into SPI mode
- Retargeted UART for debug output to provide real-time feedback on SD card initialization
- Created custom error handling and response checking for SD card commands
- Ensured compatibility with SD cards operating at different voltage levels (2.7V-3.6V)

## Hardware Components
- STM32F446RE 
- MicroSD Card (SPI interface)
- Communication: SPI1 for SD card, USART2 for debug output

## Key Features
- Full SPI-based SD card initialization
- Detailed debug outputs via UART
- Support for standard SD card commands (CMD0, CMD8, CMD55, ACMD41, CMD58)
- Power-up sequence ensuring correct card initialization
- Error handling and response verification for SD card commands

## Pin Configuration
- **SPI Pins**:
  - SCK (PA5), MISO (PA6), MOSI (PA7)
  - CS (PA4)
- **Power**: 3.3V (for STM32 and SD card)
- **Debug**: UART TX/RX via PA2/PA3 for USART2


## Functionality Overview

### SPI Initialization
The SPI interface is set up for communication between the STM32F446RE and the SD card. The SPI bus is configured for master mode, with specific settings for clock polarity, phase, and speed to ensure proper communication with the SD card.

### SD Card Power-Up and Command Sequence
- **Power-up**: The SD card requires an initial wake-up sequence of 80 clock pulses to ensure it enters SPI mode.
- **CMD0 (GO_IDLE_STATE)**: Resets the SD card and puts it in idle state.
- **CMD8 (SEND_IF_COND)**: Checks the SD card voltage and version for compatibility.
- **CMD55**: Signals that the next command will be an application-specific command.
- **ACMD41 (SD_SEND_OP_COND)**: Initiates the SD card's operation and sets the host capacity.
- **CMD58**: Retrieves the SD card's OCR (Operating Condition Register), verifying its voltage and status.

### Error Handling
Each SD card command includes a response check to ensure the proper operation. If any step fails, error codes are returned, allowing for debugging and corrections.

### USART Debugging
Real-time UART output is configured to monitor the progress of SD card initialization. This allows for easy debugging and confirmation of the initialization sequence.

## Example of SPI Data Transfer:
The system includes a function `SPI_Transfer(uint8_t data)` for sending and receiving data over SPI, ensuring the SD card receives valid commands and responds with the correct data.

```c
uint8_t SPI_Transfer(uint8_t data) {
    while (!(SPI1->SR & SPI_SR_TXE));     // Wait for TXE bit
    SPI1->DR = data;                     // Send data
    while (!(SPI1->SR & SPI_SR_RXNE));   // Wait for RXNE bit
    return SPI1->DR;                     // Return received byte
}
```

## Future Enhancements
- Implement read/write functionality to interact with files on the SD card.
- Add support for higher-speed SD card operations.
- Implement a file system interface (e.g., FAT16/32) for easy file management on the SD card.


