#include <stdint.h>
#include <stdbool.h>

#if defined(ESP8266)
//#define ESP01
#endif

#define SSD1306_USE_I2C
//#define SSD1306_USE_SPI

#if defined(SSD1306_USE_I2C)
//#define SSD1306_I2C_USE_TINYWIREM

#if !defined(SSD1306_I2C_USE_TINYWIREM)
#include <Wire.h>
#else
#include <TinyWireM.h>
#endif  // SSD1306_I2C_USE_TINYWIREM
#elif defined(SSD1306_USE_SPI)
#include <SPI.h>
#else
#error choose I2C or SPI.
#endif

#include <ssd1306_cpp.h>

// if You wanna change Width and Height, don't forget to change ssd1306.h too.

void* pLock = NULL;

#if defined(SSD1306_USE_I2C)
SSD1306_I2C oMyDisplay;
#elif defined(SSD1306_USE_SPI)
SSD1306_SPI oMyDisplay;
#endif

#if defined(SSD1306_USE_I2C)
static void SSD1306_I2C_BeginTransmission(const uint8_t u8I2CAddress) {
#if !defined(SSD1306_I2C_USE_TINYWIREM)
  Wire.beginTransmission(u8I2CAddress);
#else
  TinyWireM.beginTransmission(u8I2CAddress);
#endif  // SSD1306_I2C_USE_TINYWIREM
}

static void SSD1306_I2C_Write(const uint8_t u8Value) {
#if !defined(SSD1306_I2C_USE_TINYWIREM)
  Wire.write(u8Value);
#else
  TinyWireM.send(u8Value);
#endif  // SSD1306_I2C_USE_TINYWIREM
}

static void SSD1306_I2C_EndTransmission(void) {
#if !defined(SSD1306_I2C_USE_TINYWIREM)
  Wire.endTransmission();
#else
  TinyWireM.endTransmission();
#endif  // SSD1306_I2C_USE_TINYWIREM
}
#elif defined(SSD1306_USE_SPI)
static void SSD1306_SPI_SetPinIO(const uint8_t u8Pin, const uint8_t u8IOMode) {
  switch(u8IOMode) {
  case 0:
    pinMode(u8Pin, OUTPUT);
    break;
  case 1:
    pinMode(u8Pin, INPUT);
    break;
  case 2:
    pinMode(u8Pin, INPUT_PULLUP);
    break;
  }
}

static void SSD1306_SPI_PinOutput(const uint8_t u8Pin, const bool bHigh) {
  digitalWrite(u8Pin, bHigh ? HIGH : LOW);
}

static void SSD1306_SPI_Transfer(const uint8_t u8Data) {
  SPI.transfer(u8Data);
}

static void SSD1306_DelayMS(const uint16_t u16MS) {
  delay(u16MS);
}
#endif

static void SSD1306_MemoryBarrier(void) {
}

void setup() {
#if defined(SSD1306_USE_I2C)
  oMyDisplay.initialize(
    SSD1306_I2C_BeginTransmission,
    SSD1306_I2C_Write,
    SSD1306_I2C_EndTransmission,
    SSD1306_MemoryBarrier,
    &pLock,
    0x3C, SSD1306_LCD_WIDTH, SSD1306_LCD_HEIGHT, 0xFF
  );
#elif defined(SSD1306_USE_SPI)
#if SSD1306_SPI_WIRE == 4
  oMyDisplay.initialize(
    SSD1306_SPI_SetPinIO,
    SSD1306_SPI_PinOutput,
    SSD1306_SPI_Transfer,
    SSD1306_DelayMS,
    SSD1306_MemoryBarrier,
    &pLock,
    10, 9, 8, SSD1306_LCD_WIDTH, SSD1306_LCD_HEIGHT, 0xFF
  );
#else
  oMyDisplay.initialize(
    SSD1306_SPI_SetPinIO,
    SSD1306_SPI_PinOutput,
    SSD1306_SPI_Transfer,
    SSD1306_DelayMS,
    SSD1306_MemoryBarrier,
    &pLock,
    10, 9,    SSD1306_LCD_WIDTH, SSD1306_LCD_HEIGHT, 0xFF
  );
#endif
#endif

#if defined(SSD1306_USE_I2C)
#if !defined(SSD1306_I2C_USE_TINYWIREM)
#if defined(ESP01)
  Wire.begin(0, 2);
#else
  Wire.begin();
#endif
  Wire.setClock(400000);
#else
  TinyWireM.begin();
#endif  // SSD1306_I2C_USE_TINYWIREM
#elif defined(SSD1306_USE_SPI)
  SPI.begin();
  SPI.setDataMode(SPI_MODE3);
#endif

  oMyDisplay.initDevice();
  oMyDisplay.clear();
}

void loop() {
  switch(0) {
  case 0:
    oMyDisplay.drawPixel(
      random(SSD1306_LCD_WIDTH), random(SSD1306_LCD_HEIGHT),
      random(2)
    );
    break;
  case 1:
    oMyDisplay.drawLine(
      random(SSD1306_LCD_WIDTH * 2) - SSD1306_LCD_WIDTH / 2, random(SSD1306_LCD_HEIGHT * 2) - SSD1306_LCD_HEIGHT / 2,
      random(SSD1306_LCD_WIDTH * 2) - SSD1306_LCD_WIDTH / 2, random(SSD1306_LCD_HEIGHT * 2) - SSD1306_LCD_HEIGHT / 2,
      random(2)
    );
    break;
  case 2:
    oMyDisplay.drawRectangle(
      random(SSD1306_LCD_WIDTH * 2) - SSD1306_LCD_WIDTH / 2, random(SSD1306_LCD_HEIGHT * 2) - SSD1306_LCD_HEIGHT / 2,
      random(SSD1306_LCD_WIDTH * 2) - SSD1306_LCD_WIDTH / 2, random(SSD1306_LCD_HEIGHT * 2) - SSD1306_LCD_HEIGHT / 2,
      random(2),
      false,
      0
    );
    break;
  case 3:
    oMyDisplay.drawRectangle(
      random(SSD1306_LCD_WIDTH * 2) - SSD1306_LCD_WIDTH / 2, random(SSD1306_LCD_HEIGHT * 2) - SSD1306_LCD_HEIGHT / 2,
      random(SSD1306_LCD_WIDTH * 2) - SSD1306_LCD_WIDTH / 2, random(SSD1306_LCD_HEIGHT * 2) - SSD1306_LCD_HEIGHT / 2,
      random(2),
      true,
      random(2)
    );
    break;
  case 4:
    oMyDisplay.drawCircle(
      random(SSD1306_LCD_WIDTH * 2) - SSD1306_LCD_WIDTH / 2, random(SSD1306_LCD_HEIGHT * 2) - SSD1306_LCD_HEIGHT / 2,
      random(SSD1306_LCD_HEIGHT),
      random(2),
      false,
      0
    );
    break;
  case 5:
    oMyDisplay.drawCircle(
      random(SSD1306_LCD_WIDTH * 2) - SSD1306_LCD_WIDTH / 2, random(SSD1306_LCD_HEIGHT * 2) - SSD1306_LCD_HEIGHT / 2,
      random(SSD1306_LCD_HEIGHT),
      random(2),
      true,
      random(2)
    );
    break;
  }

  oMyDisplay.refresh();
}

