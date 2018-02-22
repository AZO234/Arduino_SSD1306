#include <SSD1306_I2C.h>
#include "avr/pgmspace.h"
#include "bit1bmp.h"

extern Bit1BMP_t tImageLala;

class SSD1306_I2C oMyDisplay(0x78, 128, 64);

void DisplayImage(class SSD1306_I2C* poDisplay, const int iX, const int iY, const Bit1BMP_t* ptImage) {
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
        uiByteX = pgm_read_byte_near(ptImage->pucImageData + uiY * uiLineSize + uiX / 8);
      }
      uiColor = (uiByteX & 1 << (7 - (uiX % 8))) ? 1 : 0;
      poDisplay->DrawPixel(iX + uiX, iY + uiY, uiColor);
    }
  }
}

void setup() {
  oMyDisplay.Initialize(1);

  oMyDisplay.Clear();
  DisplayImage(&oMyDisplay, 0, 0, &tImageLala);
#ifdef SSD1306_I2C_ENABLE_FRAMEBUFFER
  oMyDisplay.Refresh();
#endif
}

void loop() {
}

