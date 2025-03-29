#include <Wire.h>  // Для I2C
#include <SPI.h>   // Для SPI

// Массив всех доступных пинов (исключая USB: PA11, PA12)
const int availablePins[] = {
  PA0, PA1, PA2, PA3, PA4, PA5, PA6, PA7, PA8, PA9, PA10,
  PA13, PA14, PA15, PB0, PB1, PB3, PB4, PB5, PB6, PB7, PB8, PB9, PB10, PB11, PB12, PB13, PB14, PB15,
  PC13, PC14, PC15
};
const int numPins = sizeof(availablePins) / sizeof(availablePins[0]);

// Буфер для команд
char buffer[64];
int bufferIndex = 0;

// Структуры для отслеживания состояния интерфейсов
struct UARTConfig {
  HardwareSerial* serial;
  bool active;
} uartConfigs[] = {
  { &Serial1, false }, // USART1: PA9 (TX), PA10 (RX)
  { &Serial2, false }, // USART2: PA2 (TX), PA3 (RX)
  { &Serial3, false }  // USART3: PB10 (TX), PB11 (RX)
};

struct I2CConfig {
  TwoWire* i2c;
  bool active;
} i2cConfigs[] = {
  { &Wire, false },   // I2C1: PB6 (SCL), PB7 (SDA)
  { &Wire1, false }   // I2C2: PB10 (SCL), PB11 (SDA)
};

struct SPIConfig {
  SPIClass* spi;
  bool active;
} spiConfigs[] = {
  { &SPI, false },    // SPI1: PA5 (SCK), PA6 (MISO), PA7 (MOSI)
  { &SPI2, false }    // SPI2: PB13 (SCK), PB14 (MISO), PB15 (MOSI)
};

void setup() {
  Serial.begin(115200); // USB Serial для команд
  for (int i = 0; i < numPins; i++) {
    pinMode(availablePins[i], INPUT);
  }
}

void loop() {
  while (Serial.available() > 0) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') {
      buffer[bufferIndex] = '\0';
      processCommand(buffer);
      bufferIndex = 0;
    } else if (bufferIndex < sizeof(buffer) - 1) {
      buffer[bufferIndex++] = c;
    }
  }
}

void processCommand(char* cmd) {
  char* token = strtok(cmd, " ");
  if (token == NULL) {
    Serial.println("ERR: Empty command");
    return;
  }

  // MODE <pin> <mode> — установка режима пина
  if (strcmp(token, "MODE") == 0) {
    char* pinStr = strtok(NULL, " ");
    char* modeStr = strtok(NULL, " ");
    if (!pinStr || !modeStr) {
      Serial.println("ERR: Usage: MODE <pin> <IN/OUT/PWM/ANALOG>");
      return;
    }
    int pin = findPin(pinStr);
    if (pin == -1) {
      Serial.println("ERR: Invalid pin");
      return;
    }
    if (strcmp(modeStr, "IN") == 0) {
      pinMode(pin, INPUT);
      Serial.print("OK: ");
      Serial.print(pinStr);
      Serial.println(" INPUT");
    } else if (strcmp(modeStr, "OUT") == 0) {
      pinMode(pin, OUTPUT);
      Serial.print("OK: ");
      Serial.print(pinStr);
      Serial.println(" OUTPUT");
    } else if (strcmp(modeStr, "PWM") == 0) {
      pinMode(pin, OUTPUT);
      Serial.print("OK: ");
      Serial.print(pinStr);
      Serial.println(" PWM");
    } else if (strcmp(modeStr, "ANALOG") == 0) {
      pinMode(pin, INPUT_ANALOG);
      Serial.print("OK: ");
      Serial.print(pinStr);
      Serial.println(" ANALOG");
    } else {
      Serial.println("ERR: Invalid mode");
    }
  }
  // SET <pin> <value> — установка значения
  else if (strcmp(token, "SET") == 0) {
    char* pinStr = strtok(NULL, " ");
    char* valueStr = strtok(NULL, " ");
    if (!pinStr || !valueStr) {
      Serial.println("ERR: Usage: SET <pin> <value>");
      return;
    }
    int pin = findPin(pinStr);
    if (pin == -1) {
      Serial.println("ERR: Invalid pin");
      return;
    }
    int value = atoi(valueStr);
    if (value < 0 || value > 255) {
      Serial.println("ERR: Value must be 0-255");
      return;
    }
    analogWrite(pin, value); // Работает для OUT и PWM
    Serial.print("OK: ");
    Serial.print(pinStr);
    Serial.print(" set to ");
    Serial.println(value);
  }
  // READ <pin> — чтение значения
  else if (strcmp(token, "READ") == 0) {
    char* pinStr = strtok(NULL, " ");
    if (!pinStr) {
      Serial.println("ERR: Usage: READ <pin>");
      return;
    }
    int pin = findPin(pinStr);
    if (pin == -1) {
      Serial.println("ERR: Invalid pin");
      return;
    }
    Serial.print("OK: ");
    Serial.print(pinStr);
    Serial.print(" ");
    if (digitalRead(pin) == -1) { // Проверка на аналоговый режим
      Serial.println(analogRead(pin));
    } else {
      Serial.println(digitalRead(pin));
    }
  }
  // UART <port> <baud> <command> — управление UART
  else if (strcmp(token, "UART") == 0) {
    char* portStr = strtok(NULL, " ");
    char* baudStr = strtok(NULL, " ");
    char* uartCmd = strtok(NULL, " ");
    if (!portStr || !baudStr) {
      Serial.println("ERR: Usage: UART <1/2/3> <baud> [WRITE <data>]");
      return;
    }
    int port = atoi(portStr) - 1;
    if (port < 0 || port > 2) {
      Serial.println("ERR: Invalid UART port (1-3)");
      return;
    }
    long baud = atol(baudStr);
    if (!uartConfigs[port].active) {
      uartConfigs[port].serial->begin(baud);
      uartConfigs[port].active = true;
    }
    if (uartCmd && strcmp(uartCmd, "WRITE") == 0) {
      char* data = strtok(NULL, "");
      uartConfigs[port].serial->print(data);
      Serial.print("OK: UART");
      Serial.print(port + 1);
      Serial.print(" sent: ");
      Serial.println(data);
    }
  }
  // I2C <port> <command> — управление I2C
  else if (strcmp(token, "I2C") == 0) {
    char* portStr = strtok(NULL, " ");
    char* i2cCmd = strtok(NULL, " ");
    if (!portStr || !i2cCmd) {
      Serial.println("ERR: Usage: I2C <1/2> <SCAN/WRITE <addr> <data>/READ <addr> <bytes>>");
      return;
    }
    int port = atoi(portStr) - 1;
    if (port < 0 || port > 1) {
      Serial.println("ERR: Invalid I2C port (1-2)");
      return;
    }
    if (!i2cConfigs[port].active) {
      i2cConfigs[port].i2c->begin();
      i2cConfigs[port].active = true;
    }
    if (strcmp(i2cCmd, "SCAN") == 0) {
      Serial.print("OK: I2C");
      Serial.print(port + 1);
      Serial.println(" scan:");
      for (int addr = 1; addr < 127; addr++) {
        i2cConfigs[port].i2c->beginTransmission(addr);
        if (i2cConfigs[port].i2c->endTransmission() == 0) {
          Serial.print(" - 0x");
          Serial.println(addr, HEX);
        }
      }
    } else if (strcmp(i2cCmd, "WRITE") == 0) {
      char* addrStr = strtok(NULL, " ");
      char* data = strtok(NULL, "");
      int addr = strtol(addrStr, NULL, 16);
      i2cConfigs[port].i2c->beginTransmission(addr);
      i2cConfigs[port].i2c->write(data);
      i2cConfigs[port].i2c->endTransmission();
      Serial.print("OK: I2C");
      Serial.print(port + 1);
      Serial.print(" wrote to 0x");
      Serial.println(addr, HEX);
    } else if (strcmp(i2cCmd, "READ") == 0) {
      char* addrStr = strtok(NULL, " ");
      char* bytesStr = strtok(NULL, " ");
      int addr = strtol(addrStr, NULL, 16);
      int bytes = atoi(bytesStr);
      i2cConfigs[port].i2c->requestFrom(addr, bytes);
      Serial.print("OK: I2C");
      Serial.print(port + 1);
      Serial.print(" read: ");
      while (i2cConfigs[port].i2c->available()) {
        Serial.print(i2cConfigs[port].i2c->read(), HEX);
        Serial.print(" ");
      }
      Serial.println();
    }
  }
  // SPI <port> <command> — управление SPI
  else if (strcmp(token, "SPI") == 0) {
    char* portStr = strtok(NULL, " ");
    char* spiCmd = strtok(NULL, " ");
    if (!portStr || !spiCmd) {
      Serial.println("ERR: Usage: SPI <1/2> <WRITE <data>>");
      return;
    }
    int port = atoi(portStr) - 1;
    if (port < 0 || port > 1) {
      Serial.println("ERR: Invalid SPI port (1-2)");
      return;
    }
    if (!spiConfigs[port].active) {
      spiConfigs[port].spi->begin();
      spiConfigs[port].active = true;
    }
    if (strcmp(spiCmd, "WRITE") == 0) {
      char* data = strtok(NULL, "");
      Serial.print("OK: SPI");
      Serial.print(port + 1);
      Serial.print(" sent: ");
      for (int i = 0; i < strlen(data); i++) {
        spiConfigs[port].spi->transfer(data[i]);
        Serial.print(data[i], HEX);
        Serial.print(" ");
      }
      Serial.println();
    }
  } else {
    Serial.println("ERR: Unknown command");
  }
}

// Поиск пина по имени
int findPin(char* pinStr) {
  for (int i = 0; i < numPins; i++) {
    char pinName[6];
    pinToString(availablePins[i], pinName);
    if (strcmp(pinStr, pinName) == 0) {
      return availablePins[i];
    }
  }
  return -1;
}

// Преобразование пина в строку
void pinToString(int pin, char* output) {
  if (pin >= PA0 && pin <= PA15) {
    sprintf(output, "PA%d", pin - PA0);
  } else if (pin >= PB0 && pin <= PB15) {
    sprintf(output, "PB%d", pin - PB0);
  } else if (pin >= PC13 && pin <= PC15) {
    sprintf(output, "PC%d", pin - PC13 + 13);
  }
}