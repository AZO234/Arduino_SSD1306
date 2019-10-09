#ifndef _SSD1306_H_
#define _SSD1306_H_

/* SSD1306 Driver by AZO */

#define SSD1306_FRAMEBUFFER_STATIC
#define SSD1306_FRAMEBUFFER_SEGDIRTY

#ifdef SSD1306_FRAMEBUFFER_STATIC
/* width * (height / 8) */
#define SSD1306_FRAMEBUFFER_SIZE (128*8)
//#define SSD1306_FRAMEBUFFER_SIZE (72*5)
#endif /* SSD1306_FRAMEBUFFER_STATIC */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct SSD1306_Write_Data_t_ {
  bool bData;
  uint8_t u8Count;
  uint8_t au8Data[30];
} SSD1306_Write_Data_t;

/* Access */
typedef uint8_t (*SSD1306_Read_t)(void* pInstance, const uint8_t u8Reg);
typedef void (*SSD1306_Write_t)(void* pInstance, const SSD1306_Write_Data_t* ptWriteData);
typedef void (*SSD1306_MemoryBarrier_t)(void);

typedef struct SSD1306_t_ {
  void* pInstance;
  SSD1306_Read_t tRead;
  SSD1306_Write_t tWrite;
  SSD1306_MemoryBarrier_t tMemoryBarrier;
  void** ppLock;
  uint8_t u8MaxX;
  uint8_t u8MaxY;
  uint8_t u8Contrast;
#ifdef SSD1306_FRAMEBUFFER_STATIC
  uint8_t au8FrameBuffer[SSD1306_FRAMEBUFFER_SIZE];
#ifdef SSD1306_FRAMEBUFFER_SEGDIRTY
  uint8_t au8FBSegDirty[SSD1306_FRAMEBUFFER_SIZE / 8];
#endif  /* SSD1306_FRAMEBUFFER_SEGDIRTY */
#else
  uint8_t* pu8FrameBuffer;
#ifdef SSD1306_FRAMEBUFFER_SEGDIRTY
  uint8_t* pu8FBSegDirty;
#endif  /* SSD1306_FRAMEBUFFER_SEGDIRTY */
#endif  /* SSD1306_FRAMEBUFFER_STATIC */
  uint8_t u8FBPageDirty;
  bool bSleep;
  bool bScroll;
  bool bInvert;
  SSD1306_Write_Data_t tWriteCommand;
  SSD1306_Write_Data_t tWriteSeg;
} SSD1306_t;

bool SSD1306_Initialize(
  SSD1306_t* ptSSD1306,
  void* pInstance,
  const SSD1306_Read_t tRead,
  const SSD1306_Write_t tWrite,
  const SSD1306_MemoryBarrier_t tMemoryBarrier,
  void** ppLock,
  const uint8_t u8MaxX,
  const uint8_t u8MaxY,
  const uint8_t u8Contrast
);
#ifndef SSD1306_FRAMEBUFFER_STATIC
bool SSD1306_Destroy(SSD1306_t* ptSSD1306);
#endif  /* SSD1306_FRAMEBUFFER_STATIC */
bool SSD1306_InitDevice(SSD1306_t* ptSSD1306);
bool SSD1306_SetContrast(SSD1306_t* ptSSD1306, const uint8_t u8Contrast);

bool SSD1306_Clear(SSD1306_t* ptSSD1306);
bool SSD1306_GetPixel(uint8_t* pu8Pixel, const SSD1306_t* ptSSD1306, const uint8_t u8X, const uint8_t u8Y);
bool SSD1306_GetSeg(uint8_t* pu8Pattern, const SSD1306_t* ptSSD1306, const uint8_t u8Seg, const uint8_t u8Page);
bool SSD1306_SetSeg(SSD1306_t* ptSSD1306, const uint8_t u8Seg, const uint8_t u8Page, const uint8_t u8Pattern);
bool SSD1306_DrawPixel(SSD1306_t* ptSSD1306, const int16_t i16X, const int16_t i16Y, const uint8_t u8Color);
bool SSD1306_DrawLine(
  SSD1306_t* ptSSD1306,
  const int16_t i16X1,
  const int16_t i16Y1,
  const int16_t i16X2,
  const int16_t i16Y2,
  const uint8_t u8Color
);
bool SSD1306_DrawRectangle(
  SSD1306_t* ptSSD1306,
  const int16_t i16X1,
  const int16_t i16Y1,
  const int16_t i16X2,
  const int16_t i16Y2,
  const uint8_t u8Color,
  const bool bFill,
  const uint8_t u8FillColor
);
bool SSD1306_DrawCircle(
  SSD1306_t* ptSSD1306,
  const int16_t i16X,
  const int16_t i16Y,
  const int16_t i16R,
  const uint8_t u8Color,
  const bool bFill,
  const uint8_t u8FillColor
);

bool SSD1306_Refresh(SSD1306_t* ptSSD1306);

bool SSD1306_HScroll(SSD1306_t* ptSSD1306, const bool bLeft, const uint8_t u8StartPage, const uint8_t u8Intreval, const uint8_t u8EndPage);
bool SSD1306_VHScroll(SSD1306_t* ptSSD1306, const bool bLeft, const uint8_t u8StartPage, const uint8_t u8Intreval, const uint8_t u8EndPage, const uint8_t u8Offset);
bool SSD1306_DisableScroll(SSD1306_t* ptSSD1306);

bool SSD1306_Sleep(SSD1306_t* ptSSD1306);
bool SSD1306_Wakeup(SSD1306_t* ptSSD1306);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _SSD1306_H_ */

