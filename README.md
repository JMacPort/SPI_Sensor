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


## 12/17: MISO Line is still driven low as soon as the module is plugged in. This leads me to believe that the modules were soldered incorrectly at the factory so they would have to be fixed. Regardless
the external power module did not fix anything in either project so I am left wondering where my issues stem from. 

## Update: Receiving an external power module as the on board power supply may not have been stable enough for the current use case. Should arrive on 12/17 and allow me to get a better test going.
The module would get a lot warmer than you would expect during when plugging the module in so I believe it is the same issue as the ESP-01 was having. If it doesn't work now, I would assume it is
faulty as well. 


## 12/19: Module works, supply 5v satisfied the module. It has an onboard regulator to convert the 5v into 3.3v and supplies enough current. Module starts up and initializes perfectly now and gives
the correct command returns. I will be able to (re)begin implementing the reading and writing function, and then using the module to store sensor data and more. Now I can also possibly implement	
some machine learning methods for SD data but that wont be for a long time.

