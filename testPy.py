import serial
import time

ser = serial.Serial('COMx', 115200, timeout=1)  # Замените COMx

def send_command(command):
    ser.write(f"{command}\n".encode())
    time.sleep(0.1)
    response = ser.readline().decode().strip()
    print(response)
    return response

# Управление светодиодом
send_command("MODE PA0 OUT")
send_command("SET PA0 1")

# Чтение аналогового датчика
send_command("MODE PA1 ANALOG")
value = send_command("READ PA1")

# UART: отправка данных
send_command("UART 1 9600 WRITE Test")

# I2C: сканирование
send_command("I2C 1 SCAN")

# SPI: отправка данных
send_command("SPI 1 WRITE 0xAA")

ser.close()