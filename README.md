Blue Pill GPIO Interface
This project transforms the STM32F103C8T6 (Blue Pill) microcontroller into a versatile GPIO interface controlled from a computer via USB. 
It allows you to manage all available pins and hardware protocols (GPIO, PWM, ADC, UART, I2C, SPI) using simple text commands sent over USB Serial. 
All logic resides on the computer, with the Blue Pill acting as a command executor, making it perfect for interfacing with Arduino modules and similar devices.

Key Features
- GPIO: Control 37 pins (digital input/output).
- PWM: Pulse-width modulation (0-255) on supported pins.
- ADC: Analog reading (0-4095) on pins PA0-PA7, PB0-PB1.
- UART: 3 ports (USART1, USART2, USART3) for serial communication.
- I2C: 2 ports (I2C1, I2C2) for two-wire devices.
- SPI: 2 ports (SPI1, SPI2) for synchronous communication.
- USB Serial: Easy PC connection without additional drivers.

Comparison with Similar Projects
This project combines the flexibility of Bus Pirate, the simplicity of Arduino Firmata, and the performance of STM32, 
offering more pins and protocols than most alternatives while keeping resource usage minimal.

Requirements
Hardware
- STM32F103C8T6 (Blue Pill) board.
- ST-Link V2 programmer for flashing.
- USB cable (micro-USB) for PC connection.
Software
- Arduino IDE with STM32 support (STM32duino).
- ST-Link Utility or STM32CubeProgrammer for flashing.
- Terminal (e.g., PuTTY) or Python with the pyserial library for control.
