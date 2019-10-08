#include <stdint.h>
#include <stdbool.h>

//#define SSD1306_I2C_USE_TINYWIREM

#if !defined(SSD1306_I2C_USE_TINYWIREM)
#include <Wire.h>
#else
#include <TinyWireM.h>
#endif  /* SSD1306_I2C_USE_TINYWIREM */

#include <ssd1306_i2c.h>

/* if change this sizes, don't forget to change ssd1306.h too. */
#define SSD1306_I2C_WIDTH   128
#define SSD1306_I2C_HEIGHT  64

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

void setup() {
  oMyDisplay.initialize(
    SSD1306_I2C_BeginTransmission,
    SSD1306_I2C_Write,
    SSD1306_I2C_EndTransmission,
    SSD1306_I2C_MemoryBarrier,
    &pLock,
    0x3C, SSD1306_I2C_WIDTH, SSD1306_I2C_HEIGHT, 0xFF
  );

#if !defined(SSD1306_I2C_USE_TINYWIREM)
  Wire.begin();
  Wire.setClock(400000);
#else
  TinyWireM.begin();
#endif  /* SSD1306_I2C_USE_TINYWIREM */

  oMyDisplay.initDevice();
  oMyDisplay.clear();
}

void loop() {
  switch(0) {
  case 0:
    oMyDisplay.drawPixel(
      random(SSD1306_I2C_WIDTH), random(SSD1306_I2C_HEIGHT),
      random(2)
    );
    break;
  case 1:
    oMyDisplay.drawLine(
      random(SSD1306_I2C_WIDTH * 2) - SSD1306_I2C_WIDTH / 2, random(SSD1306_I2C_HEIGHT * 2) - SSD1306_I2C_HEIGHT / 2,
      random(SSD1306_I2C_WIDTH * 2) - SSD1306_I2C_WIDTH / 2, random(SSD1306_I2C_HEIGHT * 2) - SSD1306_I2C_HEIGHT / 2,
      random(2)
    );
    break;
  case 2:
    oMyDisplay.drawRectangle(
      random(SSD1306_I2C_WIDTH * 2) - SSD1306_I2C_WIDTH / 2, random(SSD1306_I2C_HEIGHT * 2) - SSD1306_I2C_HEIGHT / 2,
      random(SSD1306_I2C_WIDTH * 2) - SSD1306_I2C_WIDTH / 2, random(SSD1306_I2C_HEIGHT * 2) - SSD1306_I2C_HEIGHT / 2,
      random(2),
      false,
      0
    );
    break;
  case 3:
    oMyDisplay.drawRectangle(
      random(SSD1306_I2C_WIDTH * 2) - SSD1306_I2C_WIDTH / 2, random(SSD1306_I2C_HEIGHT * 2) - SSD1306_I2C_HEIGHT / 2,
      random(SSD1306_I2C_WIDTH * 2) - SSD1306_I2C_WIDTH / 2, random(SSD1306_I2C_HEIGHT * 2) - SSD1306_I2C_HEIGHT / 2,
      random(2),
      true,
      random(2)
    );
    break;
  case 4:
    oMyDisplay.drawCircle(
      random(SSD1306_I2C_WIDTH * 2) - SSD1306_I2C_WIDTH / 2, random(SSD1306_I2C_HEIGHT * 2) - SSD1306_I2C_HEIGHT / 2,
      random(SSD1306_I2C_HEIGHT),
      random(2),
      false,
      0
    );
    break;
  case 5:
    oMyDisplay.drawCircle(
      random(SSD1306_I2C_WIDTH * 2) - SSD1306_I2C_WIDTH / 2, random(SSD1306_I2C_HEIGHT * 2) - SSD1306_I2C_HEIGHT / 2,
      random(SSD1306_I2C_HEIGHT),
      random(2),
      true,
      random(2)
    );
    break;
  }

  oMyDisplay.refresh();
}

