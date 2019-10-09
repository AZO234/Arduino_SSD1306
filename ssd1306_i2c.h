#ifndef _SSD1306_I2C_H_
#define _SSD1306_I2C_H_

/* SSD1306 I2C Driver by AZO */

#include "ssd1306.h"

typedef void (*SSD1306_I2C_BeginTransmission_t)(const uint8_t u8Address);
typedef void (*SSD1306_I2C_Write_t)(const uint8_t u8Value);
typedef void (*SSD1306_I2C_EndTransmission_t)(void);

class SSD1306_I2C {
public:
  SSD1306_I2C(void);
#ifndef SSD1306_FRAMEBUFFER_STATIC
  ~SSD1306_I2C(void);
#endif /* SSD1306_FRAMEBUFFER_STATIC */

  /* MaxX and MaxY must be multiples of 8 */
  bool initialize(
    const SSD1306_I2C_BeginTransmission_t tBeginTransmission,
    const SSD1306_I2C_Write_t             tWrite,
    const SSD1306_I2C_EndTransmission_t   tEndTransmission,
    const SSD1306_MemoryBarrier_t         tMemoryBarrier,
    void** ppLock,
    const uint8_t u8I2CAddress,
    const uint8_t u8MaxX,
    const uint8_t u8MaxY,
    const uint8_t u8Contrast
  );
  bool initDevice(void);

  uint8_t getI2CAddress(void);

  bool clear(void);
  bool getPixel(uint8_t* pu8Pixel, const uint8_t u8X, const uint8_t u8Y);
  bool getSeg(uint8_t* pu8Pattern, const uint8_t u8X, const uint8_t u8Seg);
  bool setSeg(const uint8_t u8X, const uint8_t u8Seg, const uint8_t u8Pattern);

  bool drawPixel(const uint8_t u8X, const uint8_t u8Y, const uint8_t u8Color);
  bool drawLine(
    const int16_t i16X1,
    const int16_t i16Y1,
    const int16_t i16X2,
    const int16_t i16Y2,
    const uint8_t u8Color
  );
  bool drawRectangle(
    const int16_t i16X1,
    const int16_t i16Y1,
    const int16_t i16X2,
    const int16_t i16Y2,
    const uint8_t u8Color,
    const bool bFill,
    const uint8_t u8FillColor
  );
  bool drawCircle(
    const int16_t i16X,
    const int16_t i16Y,
    const int16_t i16R,
    const uint8_t u8Color,
    const bool bFill,
    const uint8_t u8FillColor
  );
  bool isInvert(void);
  void setInvert(const bool bInvert);

  bool refresh(void);

  bool isScroll(void);
  bool HScroll(const bool bLeft, const uint8_t u8StartPage, const uint8_t u8Intreval, const uint8_t u8EndPage);
  bool VHScroll(const bool bLeft, const uint8_t u8StartPage, const uint8_t u8Intreval, const uint8_t u8EndPage, const uint8_t u8Offset);
  bool disableScroll(void);

  bool isSleep(void);
  bool sleep(void);
  bool wakeup(void);

  /* Access (Don't use) */
  static void gen_write(void* pSSD1306_I2C, const SSD1306_Write_Data_t* ptWriteData);

private:
  SSD1306_t tSSD1306;
  uint8_t u8I2CAddress;

  SSD1306_I2C_BeginTransmission_t tBeginTransmission;
  SSD1306_I2C_Write_t             tWrite;
  SSD1306_I2C_EndTransmission_t   tEndTransmission;
};

#endif  /* _SSD1306_I2C_H_ */

