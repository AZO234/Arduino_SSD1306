#include <stdint.h>
#include <stdbool.h>

//#define SSD1306_I2C_USE_TINYWIREM

#if !defined(SSD1306_I2C_USE_TINYWIREM)
#include <Wire.h>
#else
#include <TinyWireM.h>
#endif  /* SSD1306_I2C_USE_TINYWIREM */

#include <ssd1306_i2c.h>

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

static uint16_t u16DrawCount = 0;

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
}

void loop() {
  switch(0) {
  case 0:
    oMyDisplay.drawPixel(
      random(128), random(64),
      random(2)
    );
    break;
  case 1:
    oMyDisplay.drawLine(
      random(256) - 64, random(128) - 32,
      random(256) - 64, random(128) - 32,
      random(2)
    );
    break;
  case 2:
    oMyDisplay.drawRectangle(
      random(256) - 64, random(128) - 32,
      random(256) - 64, random(128) - 32,
      random(2),
      false,
      0
    );
    break;
  case 3:
    oMyDisplay.drawRectangle(
      random(256) - 64, random(128) - 32,
      random(256) - 64, random(128) - 32,
      random(2),
      true,
      random(2)
    );
    break;
  case 4:
    oMyDisplay.drawCircle(
      random(256) - 64, random(128) - 32,
      random(64),
      random(2),
      false,
      0
    );
    break;
  case 5:
    oMyDisplay.drawCircle(
      random(256) - 64, random(128) - 32,
      random(64),
      random(2),
      true,
      random(2)
    );
    break;
  }

  u16DrawCount++;
  if(u16DrawCount % 25 == 0) {
    oMyDisplay.refresh();
  }
}

