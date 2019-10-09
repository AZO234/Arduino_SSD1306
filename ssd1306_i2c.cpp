/* SSD1306 I2C Driver by AZO */

#include <stdio.h>
#include "ssd1306_i2c.h"

SSD1306_I2C::SSD1306_I2C(void) {
}

#ifndef SSD1306_FRAMEBUFFER_STATIC
SSD1306_I2C::~SSD1306_I2C(void) {
  SSD1306_Destroy(&this->tSSD1306);
}
#endif /* SSD1306_FRAMEBUFFER_STATIC */

bool SSD1306_I2C::initialize(
  const SSD1306_I2C_BeginTransmission_t tBeginTransmission,
  const SSD1306_I2C_Write_t             tWrite,
  const SSD1306_I2C_EndTransmission_t   tEndTransmission,
  const SSD1306_MemoryBarrier_t         tMemoryBarrier,
  void** ppLock,
  const uint8_t u8I2CAddress,
  const uint8_t u8MaxX,
  const uint8_t u8MaxY,
  const uint8_t u8Contrast
) {
  bool bValid = false;

  this->tBeginTransmission = NULL;
  this->tWrite = NULL;
  this->tEndTransmission = NULL;

  if(tBeginTransmission && tWrite && tEndTransmission) {
    this->u8I2CAddress = u8I2CAddress;
    this->tBeginTransmission = tBeginTransmission;
    this->tWrite = tWrite;
    this->tEndTransmission = tEndTransmission;

    bValid = SSD1306_Initialize(
      &this->tSSD1306,
      this,
      NULL,
      SSD1306_I2C::gen_write,
      tMemoryBarrier,
      ppLock,
      u8MaxX,
      u8MaxY,
      u8Contrast
    );
  }

  return bValid;
}

/* Access */

void SSD1306_I2C::gen_write(void* pSSD1306_I2C, const SSD1306_Write_Data_t* ptWriteData) {
  uint8_t u8Count;
  SSD1306_I2C* poSSD1306_I2C = (SSD1306_I2C*)pSSD1306_I2C;

  if(pSSD1306_I2C && ptWriteData) {
    if(
      poSSD1306_I2C->tBeginTransmission &&
      poSSD1306_I2C->tWrite &&
      poSSD1306_I2C->tEndTransmission &&
      ptWriteData->u8Count > 0
    ) {
      poSSD1306_I2C->tBeginTransmission(poSSD1306_I2C->u8I2CAddress);
      poSSD1306_I2C->tWrite(ptWriteData->bData ? 0b01000000 : (ptWriteData->u8Count == 1 ? 0b10000000 : 0b00000000));
      for(u8Count = 0; u8Count < ptWriteData->u8Count; u8Count++) {
        poSSD1306_I2C->tWrite(ptWriteData->au8Data[u8Count]);
      }
      poSSD1306_I2C->tEndTransmission();
    }
  }
}

bool SSD1306_I2C::initDevice(void) {
  return SSD1306_InitDevice(&this->tSSD1306);
}

uint8_t SSD1306_I2C::getI2CAddress(void) {
  return u8I2CAddress;
}

bool SSD1306_I2C::clear(void) {
  return SSD1306_Clear(&this->tSSD1306);
}

bool SSD1306_I2C::getPixel(uint8_t* pu8Pixel, const uint8_t u8X, const uint8_t u8Y) {
  return SSD1306_GetPixel(pu8Pixel, &this->tSSD1306, u8X, u8Y);
}

bool SSD1306_I2C::getSeg(uint8_t* pu8Pattern, const uint8_t u8X, const uint8_t u8Seg) {
  return SSD1306_GetSeg(pu8Pattern, &this->tSSD1306, u8X, u8Seg);
}

bool SSD1306_I2C::setSeg(const uint8_t u8X, const uint8_t u8Seg, const uint8_t u8Pattern) {
  return SSD1306_SetSeg(&this->tSSD1306, u8X, u8Seg, u8Pattern);
}

bool SSD1306_I2C::drawPixel(const uint8_t u8X, const uint8_t u8Y, const uint8_t u8Color) {
  return SSD1306_DrawPixel(&this->tSSD1306, u8X, u8Y, u8Color);
}

bool SSD1306_I2C::drawLine(
  const int16_t i16X1,
  const int16_t i16Y1,
  const int16_t i16X2,
  const int16_t i16Y2,
  const uint8_t u8Color
) {
  return SSD1306_DrawLine(&this->tSSD1306, i16X1, i16Y1, i16X2, i16Y2, u8Color);
}

bool SSD1306_I2C::drawRectangle(
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

bool SSD1306_I2C::drawCircle(
  const int16_t i16X,
  const int16_t i16Y,
  const int16_t i16R,
  const uint8_t u8Color,
  const bool bFill,
  const uint8_t u8FillColor
) {
  return SSD1306_DrawCircle(&this->tSSD1306, i16X, i16Y, i16R, u8Color, bFill, u8FillColor);
}

bool SSD1306_I2C::isInvert(void) {
  return this->tSSD1306.bInvert;
}

void SSD1306_I2C::setInvert(const bool bInvert) {
  this->tSSD1306.bInvert = bInvert;
}

bool SSD1306_I2C::refresh(void) {
  return SSD1306_Refresh(&this->tSSD1306);
}

bool SSD1306_I2C::isScroll(void) {
  return this->tSSD1306.bScroll;
}

bool SSD1306_I2C::HScroll(const bool bLeft, const uint8_t u8StartPage, const uint8_t u8Intreval, const uint8_t u8EndPage) {
  return SSD1306_HScroll(&this->tSSD1306, bLeft, u8StartPage, u8Intreval, u8EndPage);
}

bool SSD1306_I2C::VHScroll(const bool bLeft, const uint8_t u8StartPage, const uint8_t u8Intreval, const uint8_t u8EndPage, const uint8_t u8Offset) {
  return SSD1306_VHScroll(&this->tSSD1306, bLeft, u8StartPage, u8Intreval, u8EndPage, u8Offset);
}

bool SSD1306_I2C::disableScroll(void) {
  return SSD1306_DisableScroll(&this->tSSD1306);
}

bool SSD1306_I2C::isSleep(void) {
  return this->tSSD1306.bSleep;
}

bool SSD1306_I2C::sleep(void) {
  return SSD1306_Sleep(&this->tSSD1306);
}

bool SSD1306_I2C::wakeup(void) {
  return SSD1306_Wakeup(&this->tSSD1306);
}

