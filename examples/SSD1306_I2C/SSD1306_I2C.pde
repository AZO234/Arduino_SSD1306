#if defined(__AVR__)
#include <avr/pgmspace.h>
#elif defined(ESP8266)
#include <pgmspace.h>
#else
#define PROGMEM
#endif
#include <SSD1306_I2C.h>
#include "bmp.h"

extern BMP_t tLala;

class SSD1306_I2C oMyDisplay(0x78, 128, 64);

void DisplayImage(class SSD1306_I2C* poDisplay, const int iX, const int iY, const BMP_t* ptImage) {
  unsigned int uiX, uiY, uiMode;
  unsigned int uiLineSize, uiByteX;
  unsigned int uiColor;

  if(!ptImage) {
    return;
  }

  if(ptImage->uiWidth % 8 == 0) {
    uiLineSize = ptImage->uiWidth / 8;
  } else {
    uiLineSize = ptImage->uiWidth / 8 + 1;
  }

  for(uiY = 0; uiY < ptImage->uiHeight; uiY++) {
    for(uiX = 0; uiX < ptImage->uiWidth; uiX++) {
      if(uiX % 8 == 0) {
#if defined(__AVR__)
        uiByteX = pgm_read_byte_near(ptImage->pucImageData + uiY * uiLineSize + uiX / 8);
#elif defined(ESP8266)
        uiByteX = 0;
        memcpy_P(&uiByteX, &ptImage->pucImageData[uiY * uiLineSize + uiX / 8], 1);
#else
        uiByteX = ptImage->pucImageData[uiY * uiLineSize + uiX / 8];
#endif
      }
      uiColor = (uiByteX & 1 << (7 - (uiX % 8))) ? 1 : 0;
      poDisplay->DrawPixel(iX + uiX, iY + uiY, uiColor);
    }
  }
}

void setup() {
  oMyDisplay.Initialize(1);

  oMyDisplay.Clear();
  DisplayImage(&oMyDisplay, 0, 0, &tLala);
#ifdef SSD1306_I2C_ENABLE_FRAMEBUFFER
  oMyDisplay.Refresh();
#endif
}

void loop() {
}

