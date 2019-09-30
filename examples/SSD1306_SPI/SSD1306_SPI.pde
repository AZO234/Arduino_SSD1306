#include <stdint.h>
#include <stdbool.h>

#include <SPI.h>

#include <ssd1306_spi.h>

void* pLock = NULL;

/* if change this sizes, don't forget to change ssd1306.h too. */
#define SSD1306_SPI_WIDTH   128
#define SSD1306_SPI_HEIGHT  64

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

static uint16_t u16DrawCount = 0;

void setup() {
#if SSD1306_SPI_WIRE == 4
  oMyDisplay.initialize(
    SSD1306_SPI_SetPinIO,
    SSD1306_SPI_PinOutput,
    SSD1306_SPI_Transfer,
    SSD1306_SPI_DelayMS,
    SSD1306_SPI_MemoryBarrier,
    &pLock,
    10, 9, 8, SSD1306_SPI_WIDTH, SSD1306_SPI_HEIGHT, 0xFF
  );
#else
  oMyDisplay.initialize(
    SSD1306_SPI_SetPinIO,
    SSD1306_SPI_PinOutput,
    SSD1306_SPI_Transfer,
    SSD1306_SPI_DelayMS,
    SSD1306_SPI_MemoryBarrier,
    &pLock,
    10, 9,    SSD1306_SPI_WIDTH, SSD1306_SPI_HEIGHT, 0xFF
  );
#endif

  SPI.begin();
  SPI.setDataMode(SPI_MODE3);

  oMyDisplay.initDevice();
  oMyDisplay.clear();
}

void loop() {
  switch(0) {
  case 0:
    oMyDisplay.drawPixel(
      random(SSD1306_SPI_WIDTH), random(SSD1306_SPI_HEIGHT),
      random(2)
    );
    break;
  case 1:
    oMyDisplay.drawLine(
      random(SSD1306_SPI_WIDTH * 2) - SSD1306_SPI_WIDTH / 2, random(SSD1306_SPI_HEIGHT * 2) - SSD1306_SPI_HEIGHT / 2,
      random(SSD1306_SPI_WIDTH * 2) - SSD1306_SPI_WIDTH / 2, random(SSD1306_SPI_HEIGHT * 2) - SSD1306_SPI_HEIGHT / 2,
      random(2)
    );
    break;
  case 2:
    oMyDisplay.drawRectangle(
      random(SSD1306_SPI_WIDTH * 2) - SSD1306_SPI_WIDTH / 2, random(SSD1306_SPI_HEIGHT * 2) - SSD1306_SPI_HEIGHT / 2,
      random(SSD1306_SPI_WIDTH * 2) - SSD1306_SPI_WIDTH / 2, random(SSD1306_SPI_HEIGHT * 2) - SSD1306_SPI_HEIGHT / 2,
      random(2),
      false,
      0
    );
    break;
  case 3:
    oMyDisplay.drawRectangle(
      random(SSD1306_SPI_WIDTH * 2) - SSD1306_SPI_WIDTH / 2, random(SSD1306_SPI_HEIGHT * 2) - SSD1306_SPI_HEIGHT / 2,
      random(SSD1306_SPI_WIDTH * 2) - SSD1306_SPI_WIDTH / 2, random(SSD1306_SPI_HEIGHT * 2) - SSD1306_SPI_HEIGHT / 2,
      random(2),
      true,
      random(2)
    );
    break;
  case 4:
    oMyDisplay.drawCircle(
      random(SSD1306_SPI_WIDTH * 2) - SSD1306_SPI_WIDTH / 2, random(SSD1306_SPI_HEIGHT * 2) - SSD1306_SPI_HEIGHT / 2,
      random(SSD1306_SPI_HEIGHT),
      random(2),
      false,
      0
    );
    break;
  case 5:
    oMyDisplay.drawCircle(
      random(SSD1306_SPI_WIDTH * 2) - SSD1306_SPI_WIDTH / 2, random(SSD1306_SPI_HEIGHT * 2) - SSD1306_SPI_HEIGHT / 2,
      random(SSD1306_SPI_HEIGHT),
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

