#include <stdint.h>
#include <stdbool.h>

#if defined(__AVR__)
#include <avr/pgmspace.h>
#elif defined(ESP8266)
#include <pgmspace.h>
#else
#define PROGMEM
#endif

//#define SSD1306_I2C_USE_TINYWIREM

#if !defined(SSD1306_I2C_USE_TINYWIREM)
#include <Wire.h>
#else
#include <TinyWireM.h>
#endif  /* SSD1306_I2C_USE_TINYWIREM */

#include <ssd1306_i2c.h>
#include "bmp.h"

extern BMP_t tLala;

void* pLock = NULL;

SSD1306_I2C oMyDisplay;

static void SSD1306_I2C_BeginTransmission(const uint8_t u8I2CAddress) {
#if !defined(SSD1306_I2C_USE_TINYWIREM)
  Wire.beginTransmission(u8I2CAddress);
#else
  TinyWireM.beginTransmission(u8I2CAddress);
#endif  /* SSD1306_I2C_USE_TINYWIREM */
}

static void SSD1306_I2C_Write(const uint8_t u8Value) {
#if !defined(SSD1306_I2C_USE_TINYWIREM)
  Wire.write(u8Value);
#else
  TinyWireM.send(u8Value);
#endif  /* SSD1306_I2C_USE_TINYWIREM */
}

static void SSD1306_I2C_EndTransmission(void) {
#if !defined(SSD1306_I2C_USE_TINYWIREM)
  Wire.endTransmission();
#else
  TinyWireM.endTransmission();
#endif  /* SSD1306_I2C_USE_TINYWIREM */
}

static void SSD1306_I2C_MemoryBarrier(void) {
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
  oMyDisplay.initialize(
    SSD1306_I2C_BeginTransmission,
    SSD1306_I2C_Write,
    SSD1306_I2C_EndTransmission,
    SSD1306_I2C_MemoryBarrier,
    &pLock,
    0x3C, 128, 64, 0xFF
  );

  Wire.begin();
  Wire.setClock(400000);

  oMyDisplay.initDevice();
  oMyDisplay.clear();
  DisplayImage(&oMyDisplay, 0, 0, &tLala);
  oMyDisplay.refresh();
}

void loop() {
}

