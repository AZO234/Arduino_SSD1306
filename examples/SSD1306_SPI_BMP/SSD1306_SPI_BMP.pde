#include <stdint.h>
#include <stdbool.h>

#if defined(__AVR__)
#include <avr/pgmspace.h>
#elif defined(ESP8266)
#include <pgmspace.h>
#else
#define PROGMEM
#endif

#include <SPI.h>

#include <ssd1306_spi.h>
#include "bmp.h"

/* if You wanna change Width and Height, don't forget to change ssd1306.h too. */

extern BMP_t tLala;

void* pLock = NULL;

SSD1306_SPI oMyDisplay;

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

static void SSD1306_SPI_DelayMS(const uint16_t u16MS) {
  delay(u16MS);
}

static void SSD1306_SPI_MemoryBarrier(void) {
}

void DisplayImage(SSD1306_SPI* poDisplay, const int8_t i8X, const int8_t i8Y, const BMP_t* ptImage) {
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
#if SSD1306_SPI_WIRE == 4
  oMyDisplay.initialize(
    SSD1306_SPI_SetPinIO,
    SSD1306_SPI_PinOutput,
    SSD1306_SPI_Transfer,
    SSD1306_SPI_DelayMS,
    SSD1306_SPI_MemoryBarrier,
    &pLock,
    10, 9, 8, SSD1306_LCD_WIDTH, SSD1306_LCD_HEIGHT, 0xFF
  );
#else
  oMyDisplay.initialize(
    SSD1306_SPI_SetPinIO,
    SSD1306_SPI_PinOutput,
    SSD1306_SPI_Transfer,
    SSD1306_SPI_DelayMS,
    SSD1306_SPI_MemoryBarrier,
    &pLock,
    10, 9,    SSD1306_LCD_WIDTH, SSD1306_LCD_HEIGHT, 0xFF
  );
#endif

  SPI.begin();
  SPI.setDataMode(SPI_MODE3);

  oMyDisplay.initDevice();
  oMyDisplay.clear();
  DisplayImage(&oMyDisplay, 0, 0, &tLala);
  oMyDisplay.refresh();
}

void loop() {
}

