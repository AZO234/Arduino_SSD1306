// SSD1306 Driver by AZO

#include <stdio.h>
#include "ssd1306_cpp.h"

SSD1306::SSD1306(void) {
}

#ifndef SSD1306_FRAMEBUFFER_STATIC
SSD1306::~SSD1306(void) {
  SSD1306_Destroy(&tSSD1306);
}
#endif  // SSD1306_FRAMEBUFFER_STATIC

SSD1306_I2C::SSD1306_I2C(void) {
}

SSD1306_SPI::SSD1306_SPI(void) {
}

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

  if(tBeginTransmission && tWrite && tEndTransmission) {
    bValid = SSD1306_Initialize(
      &tSSD1306,
      this,
      NULL,
      SSD1306_I2C::gen_write,
      tMemoryBarrier,
      ppLock,
      u8MaxX,
      u8MaxY,
      u8Contrast
    );

    this->tBeginTransmission = tBeginTransmission;
    this->tWrite = tWrite;
    this->tEndTransmission = tEndTransmission;
    this->u8I2CAddress = u8I2CAddress;
  }

  return bValid;
}

bool SSD1306_SPI::initialize(
  const SSD1306_SPI_SetPinIO_t  tSetPinIO,
  const SSD1306_SPI_PinOutput_t tPinOutput,
  const SSD1306_SPI_Transfer_t  tTransfer,
  const SSD1306_DelayMS_t       tDelayMS,
  const SSD1306_MemoryBarrier_t tMemoryBarrier,
  void** ppLock,
  const uint8_t u8CS,
  const uint8_t u8RST,
#if SSD1306_SPI_WIRE == 4
  const uint8_t u8DC,
#endif
  const uint8_t u8MaxX,
  const uint8_t u8MaxY,
  const uint8_t u8Contrast
) {
  bool bValid = false;

  if(tSetPinIO && tPinOutput && tTransfer && tDelayMS) {
    bValid = SSD1306_Initialize(
      &tSSD1306,
      this,
      NULL,
      SSD1306_SPI::gen_write,
      tMemoryBarrier,
      ppLock,
      u8MaxX,
      u8MaxY,
      u8Contrast
    );

    this->tSetPinIO = tSetPinIO;
    this->tPinOutput = tPinOutput;
    this->tTransfer = tTransfer;
    this->tDelayMS = tDelayMS;
    this->u8CS = u8CS;
    this->u8RST = u8RST;
#if SSD1306_SPI_WIRE == 4
    this->u8DC = u8DC;
#endif
  }

  return bValid;
}

// Access

void SSD1306_I2C::gen_write(void* pSSD1306_I2C, const SSD1306_Write_Data_t* ptWriteData) {
  uint8_t u8Count;
  SSD1306_I2C* poSSD1306_I2C = (SSD1306_I2C*)pSSD1306_I2C;

  if(pSSD1306_I2C && ptWriteData) {
    if(
      poSSD1306_I2C->tWrite &&
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

bool SSD1306::initDevice(void) {
  return SSD1306_InitDevice(&tSSD1306);
}

uint8_t SSD1306_I2C::getI2CAddress(void) {
  return u8I2CAddress;
}

bool SSD1306::clear(void) {
  return SSD1306_Clear(&tSSD1306);
}

bool SSD1306::getPixel(uint8_t* pu8Pixel, const uint8_t u8X, const uint8_t u8Y) {
  return SSD1306_GetPixel(pu8Pixel, &tSSD1306, u8X, u8Y);
}

bool SSD1306::getSeg(uint8_t* pu8Pattern, const uint8_t u8X, const uint8_t u8Seg) {
  return SSD1306_GetSeg(pu8Pattern, &tSSD1306, u8X, u8Seg);
}

bool SSD1306::setSeg(const uint8_t u8X, const uint8_t u8Seg, const uint8_t u8Pattern) {
  return SSD1306_SetSeg(&tSSD1306, u8X, u8Seg, u8Pattern);
}

bool SSD1306::drawPixel(const uint8_t u8X, const uint8_t u8Y, const uint8_t u8Color) {
  return SSD1306_DrawPixel(&tSSD1306, u8X, u8Y, u8Color);
}

bool SSD1306::drawLine(
  const int16_t i16X1,
  const int16_t i16Y1,
  const int16_t i16X2,
  const int16_t i16Y2,
  const uint8_t u8Color
) {
  return SSD1306_DrawLine(&tSSD1306, i16X1, i16Y1, i16X2, i16Y2, u8Color);
}

bool SSD1306::drawRectangle(
  const int16_t i16X1,
  const int16_t i16Y1,
  const int16_t i16X2,
  const int16_t i16Y2,
  const uint8_t u8Color,
  const bool bFill,
  const uint8_t u8FillColor
) {
  return SSD1306_DrawRectangle(&tSSD1306, i16X1, i16Y1, i16X2, i16Y2, u8Color, bFill, u8FillColor);
}

bool SSD1306::drawCircle(
  const int16_t i16X,
  const int16_t i16Y,
  const int16_t i16R,
  const uint8_t u8Color,
  const bool bFill,
  const uint8_t u8FillColor
) {
  return SSD1306_DrawCircle(&tSSD1306, i16X, i16Y, i16R, u8Color, bFill, u8FillColor);
}

bool SSD1306::isInvert(void) {
  return tSSD1306.bInvert;
}

void SSD1306::setInvert(const bool bInvert) {
  tSSD1306.bInvert = bInvert;
}

bool SSD1306::refresh(void) {
  return SSD1306_Refresh(&tSSD1306);
}

bool SSD1306::isScroll(void) {
  return tSSD1306.bScroll;
}

bool SSD1306::HScroll(const bool bLeft, const uint8_t u8StartPage, const uint8_t u8Intreval, const uint8_t u8EndPage) {
  return SSD1306_HScroll(&tSSD1306, bLeft, u8StartPage, u8Intreval, u8EndPage);
}

bool SSD1306::VHScroll(const bool bLeft, const uint8_t u8StartPage, const uint8_t u8Intreval, const uint8_t u8EndPage, const uint8_t u8Offset) {
  return SSD1306_VHScroll(&tSSD1306, bLeft, u8StartPage, u8Intreval, u8EndPage, u8Offset);
}

bool SSD1306::disableScroll(void) {
  return SSD1306_DisableScroll(&tSSD1306);
}

bool SSD1306::isSleep(void) {
  return tSSD1306.bSleep;
}

bool SSD1306::sleep(void) {
  return SSD1306_Sleep(&tSSD1306);
}

bool SSD1306::wakeup(void) {
  return SSD1306_Wakeup(&tSSD1306);
}

