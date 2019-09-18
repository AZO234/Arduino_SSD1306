/* SSD1306 SPI Driver by AZO */

#include <stdio.h>
#include "ssd1306_spi.h"

SSD1306_SPI::SSD1306_SPI(void) {
}

SSD1306_SPI::~SSD1306_SPI(void) {
  SSD1306_Destroy(&this->tSSD1306);
}

bool SSD1306_SPI::initialize(
  const SSD1306_SPI_SetPinIO_t  tSetPinIO,
  const SSD1306_SPI_PinOutput_t tPinOutput,
  const SSD1306_SPI_Transfer_t  tTransfer,
  const SSD1306_SPI_DelayMS_t   tDelayMS,
  const SSD1306_MemoryBarrier_t tMemoryBarrier,
  void** ppLock,
  const uint8_t u8CS,
  const uint8_t u8RST,
#if SSD1306_SPI_WIRE == 4
  const uint8_t u8DC,
#endif
  const uint8_t u8MaxX,
  const uint8_t u8MaxY,
  const uint8_t u8Contrast,
  const bool bFrameBuffer
) {
  bool bValid = false;

  this->tSetPinIO = NULL;
  this->tPinOutput = NULL;
  this->tTransfer = NULL;
  this->tDelayMS = NULL;

  if(tSetPinIO && tPinOutput && tTransfer && tDelayMS) {
    this->tSetPinIO = tSetPinIO;
    this->tPinOutput = tPinOutput;
    this->tTransfer = tTransfer;
    this->tDelayMS = tDelayMS;

    this->u8CS = u8CS;
    this->u8RST = u8RST;
#if SSD1306_SPI_WIRE == 4
    this->u8DC = u8DC;
#endif

    this->tSetPinIO(this->u8CS, 0);
    this->tPinOutput(this->u8CS, true);
#if SSD1306_SPI_WIRE == 4
    while(*ppLock != this) {
      tMemoryBarrier();
      if(!*ppLock) {
        *ppLock = this;
      }
      tMemoryBarrier();
    }
    this->tSetPinIO(this->u8DC, 0);
    this->tPinOutput(this->u8DC, true);
    *ppLock = NULL;
    tMemoryBarrier();
#endif
    this->tSetPinIO(this->u8RST, 0);
    this->tPinOutput(this->u8RST, true);

    this->tPinOutput(this->u8RST, false);
    this->tDelayMS(1);
    this->tPinOutput(this->u8RST, true);

    bValid = SSD1306_Initialize(
      &this->tSSD1306,
      this,
      NULL,
      NULL,
      SSD1306_SPI::gen_write,
      NULL,
      tMemoryBarrier,
      ppLock,
      u8MaxX,
      u8MaxY,
      u8Contrast,
      bFrameBuffer
    );
  }

  return bValid;
}

/* Access (Don't use) */
void SSD1306_SPI::gen_write(void* pSSD1306_SPI, const SSD1306_Write_Data_t* ptWriteData) {
  uint8_t u8Count;
  SSD1306_SPI* poSSD1306_SPI = (SSD1306_SPI*)pSSD1306_SPI;

  if(pSSD1306_SPI && ptWriteData) {
    if(ptWriteData->u8Count > 0) {
      poSSD1306_SPI->tPinOutput(poSSD1306_SPI->u8CS, false);
#if SSD1306_SPI_WIRE == 4
      poSSD1306_SPI->tPinOutput(poSSD1306_SPI->u8DC, ptWriteData->bData);
#else
      poSSD1306_SPI->tTransfer(ptWriteData->bData ? 0b01000000 : 0b00000000);
#endif
      for(u8Count = 0; u8Count < ptWriteData->u8Count; u8Count++) {
        poSSD1306_SPI->tTransfer(ptWriteData->au8Data[u8Count]);
      }
      poSSD1306_SPI->tPinOutput(poSSD1306_SPI->u8CS, true);
    }
  }
}

bool SSD1306_SPI::initDevice(void) {
  return SSD1306_InitDevice(&this->tSSD1306);
}

uint8_t SSD1306_SPI::getCS(void) {
  return u8CS;
}

uint8_t SSD1306_SPI::getRST(void) {
  return u8RST;
}

#if SSD1306_SPI_WIRE == 4
uint8_t SSD1306_SPI::getDC(void) {
  return u8DC;
}
#endif

bool SSD1306_SPI::clear(void) {
  return SSD1306_Clear(&this->tSSD1306);
}

bool SSD1306_SPI::getPixel(uint8_t* pu8Pixel, const uint8_t u8X, const uint8_t u8Y) {
  return SSD1306_GetPixel(pu8Pixel, &this->tSSD1306, u8X, u8Y);
}

bool SSD1306_SPI::getSeg(uint8_t* pu8Pattern, const uint8_t u8X, const uint8_t u8Seg) {
  return SSD1306_GetSeg(pu8Pattern, &this->tSSD1306, u8X, u8Seg);
}

bool SSD1306_SPI::setSeg(const uint8_t u8X, const uint8_t u8Seg, const uint8_t u8Pattern) {
  return SSD1306_SetSeg(&this->tSSD1306, u8X, u8Seg, u8Pattern);
}

bool SSD1306_SPI::drawPixel(const uint8_t u8X, const uint8_t u8Y, const uint8_t u8Color) {
  return SSD1306_DrawPixel(&this->tSSD1306, u8X, u8Y, u8Color);
}

bool SSD1306_SPI::drawLine(
  const int16_t i16X1,
  const int16_t i16Y1,
  const int16_t i16X2,
  const int16_t i16Y2,
  const uint8_t u8Color
) {
  return SSD1306_DrawLine(&this->tSSD1306, i16X1, i16Y1, i16X2, i16Y2, u8Color);
}

bool SSD1306_SPI::drawRectangle(
  const int16_t i16X1,
  const int16_t i16Y1,
  const int16_t i16X2,
  const int16_t i16Y2,
  const uint8_t u8Color,
  const bool bFill,
  const uint8_t u8FillColor
) {
  return SSD1306_DrawRectangle(&this->tSSD1306, i16X1, i16Y1, i16X2, i16Y2, u8Color, bFill, u8FillColor);
}

bool SSD1306_SPI::drawCircle(
  const int16_t i16X,
  const int16_t i16Y,
  const int16_t i16R,
  const uint8_t u8Color,
  const bool bFill,
  const uint8_t u8FillColor
) {
  return SSD1306_DrawCircle(&this->tSSD1306, i16X, i16Y, i16R, u8Color, bFill, u8FillColor);
}

bool SSD1306_SPI::isInvert(void) {
  return this->tSSD1306.bInvert;
}

void SSD1306_SPI::setInvert(const bool bInvert) {
  this->tSSD1306.bInvert = bInvert;
}

bool SSD1306_SPI::refresh(void) {
  return SSD1306_Refresh(&this->tSSD1306);
}

bool SSD1306_SPI::isScroll(void) {
  return this->tSSD1306.bScroll;
}

bool SSD1306_SPI::HScroll(const bool bLeft, const uint8_t u8StartPage, const uint8_t u8Intreval, const uint8_t u8EndPage) {
  return SSD1306_HScroll(&this->tSSD1306, bLeft, u8StartPage, u8Intreval, u8EndPage);
}

bool SSD1306_SPI::VHScroll(const bool bLeft, const uint8_t u8StartPage, const uint8_t u8Intreval, const uint8_t u8EndPage, const uint8_t u8Offset) {
  return SSD1306_VHScroll(&this->tSSD1306, bLeft, u8StartPage, u8Intreval, u8EndPage, u8Offset);
}

bool SSD1306_SPI::disableScroll(void) {
  return SSD1306_DisableScroll(&this->tSSD1306);
}

bool SSD1306_SPI::isSleep(void) {
  return this->tSSD1306.bSleep;
}

bool SSD1306_SPI::sleep(void) {
  return SSD1306_Sleep(&this->tSSD1306);
}

bool SSD1306_SPI::wakeup(void) {
  return SSD1306_Wakeup(&this->tSSD1306);
}

