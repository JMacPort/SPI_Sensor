# STM32 SD Card FAT32 Logger
Implements SD card communication via SPI protocol and FAT32 filesystem support using FatFS. Creates and writes to files on standard SD cards that can be read on any computer.

## Hardware Requirements
- STM32F446RE Nucleo Board
- SD Card Module
- SD Card - FAT32 formatted
- Jumper wires

## Pin Connections
### SD Card Module
- VCC → 3.3V
- GND → GND
- SCK → PA5
- MISO → PA6
- MOSI → PA7
- CS → PB6

### Debug UART
- TX → PA2
- RX → PA3

## Software Components
- SPI communication protocol
- FAT32 filesystem using FatFS
- SD card command interface
- File operations

### Implementation Details:
- SD Card initialization sequence
- Block read/write operations
- FatFS disk I/O layer
- File system mounting
- Directory creation
- File write operations
- Debug output via UART

### FAT32 Integration
- Complete FatFS implementation
- Directory support
- File creation and writing
- Proper mount/unmount sequence

## Features
- FAT32 filesystem support
- File and directory creation
- PC-readable files
- Status feedback via UART
- Error checking and reporting
- Non-blocking operations

## File Operations
1. Mount filesystem
2. Create directories
3. Create and write files
4. Properly close files
5. Unmount filesystem

## Usage
1. Format SD card as FAT32
2. Connect SD card module according to pin configuration
3. Upload program to STM32
4. Program will create a test file in LOGDIR folder
5. Remove SD card and read files on any computer


