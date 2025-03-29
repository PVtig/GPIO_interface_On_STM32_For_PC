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


Installation
1. Set Up Arduino IDE:
- Install Arduino IDE.
- In File → Preferences, add this URL to "Additional Boards Manager URLs":
https://github.com/stm32duino/BoardManagerFiles/raw/main/package_stmicroelectronics_index.json
- In Tools → Board → Boards Manager, install "STM32 MCU based boards".

2. Compile the Code:
- Open the bluepill_gpio_interface.ino file in Arduino IDE.
- Select the board: Tools → Board → Generic STM32F103C series.
- Export the binary: Sketch → Export compiled Binary.

3. Flash via ST-Link:
- Connect ST-Link V2 to Blue Pill (SWDIO → PA13, SWCLK → PA14, GND, 3.3V).
- Open the .bin file in ST-Link Utility and click Target → Program.

4. Connect to PC:
- Plug Blue Pill into your computer via USB.
- Identify the COM port in Device Manager (Windows) or /dev/ttyACMx (Linux).


Usage
Once connected to the PC via USB, you can send commands through a terminal (e.g., Arduino Serial Monitor) or a Python script.

Command Format
GPIO:
- MODE <pin> <IN/OUT/PWM/ANALOG> — Set pin mode.
- SET <pin> <value> — Set value (0-255).
- READ <pin> — Read value (0-1 for IN, 0-4095 for ANALOG).
UART:
- UART <port> <baud> WRITE <data> — Send data via UART (port: 1-3).
I2C:
- I2C <port> SCAN — Scan for devices (port: 1-2).
- I2C <port> WRITE <addr> <data> — Write data (addr in HEX).
- I2C <port> READ <addr> <bytes> — Read data.
SPI:
- SPI <port> WRITE <data> — Send data (port: 1-2).


Example Commands

1. Turn on an LED on PA0:
MODE PA0 OUT
SET PA0 1

2. Read an analog sensor on PA1:
MODE PA1 ANALOG
READ PA1

3. Send data via UART1:
UART 1 9600 WRITE Hello

4. Scan I2C1 bus:
I2C 1 SCAN

5. Send data via SPI1:
SPI 1 WRITE 0xAA


Python Example

import serial
import time

ser = serial.Serial('COM3', 115200, timeout=1)  # Replace COM3 with your port

def send_command(cmd):
    ser.write(f"{cmd}\n".encode())
    time.sleep(0.1)
    return ser.readline().decode().strip()

print(send_command("MODE PA0 OUT"))
print(send_command("SET PA0 1"))
print(send_command("READ PA1"))

ser.close()


Pin Mapping
- GPIO: All pins except PA11/PA12 (USB).
- PWM: PA0-PA3, PA6-PA10, PB0-PB1, PB6-PB9, etc.
- ADC: PA0-PA7, PB0-PB1.
- UART:
  1: PA9 (TX), PA10 (RX).
  2: PA2 (TX), PA3 (RX).
  3: PB10 (TX), PB11 (RX).
- I2C:
  1: PB6 (SCL), PB7 (SDA).
  2: PB10 (SCL), PB11 (SDA).
- SPI:
  1: PA5 (SCK), PA6 (MISO), PA7 (MOSI).
  2: PB13 (SCK), PB14 (MISO), PB15 (MOSI).

Limitations
- Pins used by an active protocol (e.g., UART1) cannot be reused for other functions simultaneously.
- Limited memory (64 KB Flash, 20 KB RAM).

Potential Improvements
- Add commands to disable interfaces (e.g., STOP UART 1).
- Implement buffering for UART/I2C/SPI data.
- Develop a Python library for streamlined control.

License
This project is licensed under the MIT License. Feel free to use and modify it!




