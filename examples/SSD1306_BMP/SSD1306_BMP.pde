#include <stdint.h>
#include <stdbool.h>

#if defined(ESP8266)
//#define ESP01
#endif

#define SSD1306_USE_I2C
//#define SSD1306_USE_SPI

#if defined(__AVR__)
#include <avr/pgmspace.h>
#elif defined(ESP8266)
#include <pgmspace.h>
#else
#define PROGMEM
#endif

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
#include "bmp.h"

// if You wanna change Width and Height, don't forget to change ssd1306.h too.

extern BMP_t tLala;

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


void DisplayImage(SSD1306_I2C* poDisplay, const int8_t i8X, const int8_t i8Y, const BMP_t* ptImage) {
  uint8_t u8X, u8Y, u8LineSize, u8Color;
  uint8_t u8ByteX;

  if(poDisplay && ptImage) {
    if(ptImage->u32Width % 8 == 0) {
      u8LineSize = ptImage->u32Width / 8;
    } else {
      u8LineSize = ptImage->u32Width / 8 + 1;
    }

    for(u8Y = 0; u8Y < ptImage->u32Height; u8Y++) {
      for(u8X = 0; u8X < ptImage->u32Width; u8X++) {
        if(u8X % 8 == 0) {
#if defined(__AVR__)
          u8ByteX = pgm_read_byte_near(ptImage->pu8ImageData + u8Y * u8LineSize + u8X / 8);
#elif defined(ESP8266)
          memcpy_P(&u8ByteX, &ptImage->pu8ImageData[u8Y * u8LineSize + u8X / 8], 1);
#else
          u8ByteX = ptImage->pu8ImageData[u8Y * u8LineSize + u8X / 8];
#endif
        }
        u8Color = (u8ByteX & 1 << (7 - (u8X % 8))) ? 1 : 0;
        poDisplay->drawPixel(i8X + u8X, i8Y + u8Y, u8Color);
      }
    }
  }
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
  DisplayImage(&oMyDisplay, 0, 0, &tLala);
  oMyDisplay.refresh();
}

void loop() {
}

