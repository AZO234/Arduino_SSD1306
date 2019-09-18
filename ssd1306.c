/* SSD1306 Driver by AZO */

#ifndef SSD1306_FRAMEBUFFER_STATIC
#include <stdlib.h>
#endif  /* SSD1306_FRAMEBUFFER_STATIC */
#include <string.h>
#include "ssd1306.h"

static void Lock(SSD1306_t* ptSSD1306) {
  ptSSD1306->tMemoryBarrier();
  while(*ptSSD1306->ppLock != ptSSD1306) {
    if(!*ptSSD1306->ppLock) {
      *ptSSD1306->ppLock = ptSSD1306;
      ptSSD1306->tMemoryBarrier();
    }
  }
}

static void Unlock(SSD1306_t* ptSSD1306) {
  *ptSSD1306->ppLock = NULL;
  ptSSD1306->tMemoryBarrier();
}

bool SSD1306_Initialize(
  SSD1306_t* ptSSD1306,
  void* pInstance,
  SSD1306_BeginTransmission_t tBeginTransmission,
  SSD1306_Read_t tRead,
  SSD1306_Write_t tWrite,
  SSD1306_EndTransmission_t tEndTransmission,
  SSD1306_MemoryBarrier_t tMemoryBarrier,
  void** ppLock,
  const uint8_t u8MaxX,
  const uint8_t u8MaxY,
  const uint8_t u8Contrast
) {
  bool bValid = false;

  if(ptSSD1306 && pInstance && tWrite && tMemoryBarrier && ppLock && u8MaxX > 0 && u8MaxY > 0) {
    bValid = true;
#ifndef SSD1306_FRAMEBUFFER_STATIC
    ptSSD1306->pu8FrameBuffer = malloc(u8MaxY * u8MaxX / 8);
    if(!ptSSD1306->pu8FrameBuffer) {
      bValid = false;
    }
#endif  /* SSD1306_FRAMEBUFFER_STATIC */
    if(bValid) {
      ptSSD1306->pInstance          = pInstance;
      ptSSD1306->tBeginTransmission = tBeginTransmission;
      ptSSD1306->tRead              = tRead;
      ptSSD1306->tWrite             = tWrite;
      ptSSD1306->tEndTransmission   = tEndTransmission;
      ptSSD1306->tMemoryBarrier     = tMemoryBarrier;
      ptSSD1306->ppLock             = ppLock;
      ptSSD1306->u8MaxX             = u8MaxX;
      ptSSD1306->u8MaxY             = u8MaxY;
      ptSSD1306->u8Contrast         = u8Contrast;
      ptSSD1306->bSleep             = false;
      ptSSD1306->bScroll            = false;
      ptSSD1306->bInvert            = false;
    }
  }

  return bValid;
}

#ifndef SSD1306_FRAMEBUFFER_STATIC
bool SSD1306_Destroy(SSD1306_t* ptSSD1306) {
  bool bValid = false;

  if(ptSSD1306) {
    bValid = true;
    if(ptSSD1306->pu8FrameBuffer) {
      free(ptSSD1306->pu8FrameBuffer);
      ptSSD1306->pu8FrameBuffer = NULL;
    }
  }

  return bValid;
}
#endif  /* SSD1306_FRAMEBUFFER_STATIC */

bool SSD1306_InitDevice(SSD1306_t* ptSSD1306) {
  bool bValid = false;

  if(ptSSD1306) {
    if(ptSSD1306->tWrite) {
      bValid = true;
      Lock(ptSSD1306);
      if(ptSSD1306->tBeginTransmission) {
        ptSSD1306->tBeginTransmission(ptSSD1306->pInstance);
      }
      ptSSD1306->tWriteData.u8Count = 1;
      ptSSD1306->tWriteData.bData = false;
      ptSSD1306->tWriteData.au8Data[0] = 0xAE;  /* Display off */
      ptSSD1306->tWrite(ptSSD1306->pInstance, &ptSSD1306->tWriteData);
      ptSSD1306->tWriteData.u8Count = 2;
      ptSSD1306->tWriteData.bData = false;
      ptSSD1306->tWriteData.au8Data[0] = 0xA8;  /* Set Multiplex Ratio  0xA8, 0x3F */
      ptSSD1306->tWriteData.au8Data[1] = 0b00111111;
      ptSSD1306->tWrite(ptSSD1306->pInstance, &ptSSD1306->tWriteData);
      ptSSD1306->tWriteData.u8Count = 2;
      ptSSD1306->tWriteData.bData = false;
      ptSSD1306->tWriteData.au8Data[0] = 0xD3;  /* Set Display Offset 0xD3, 0x00 */
      ptSSD1306->tWriteData.au8Data[1] = 0x00;
      ptSSD1306->tWrite(ptSSD1306->pInstance, &ptSSD1306->tWriteData);
      ptSSD1306->tWriteData.u8Count = 1;
      ptSSD1306->tWriteData.bData = false;
      ptSSD1306->tWriteData.au8Data[0] = 0x40;  /* Set Display Start Line 0x40 */
      ptSSD1306->tWrite(ptSSD1306->pInstance, &ptSSD1306->tWriteData);
      ptSSD1306->tWriteData.u8Count = 1;
      ptSSD1306->tWriteData.bData = false;
      ptSSD1306->tWriteData.au8Data[0] = 0xA1;  /* Set Segment re-map 0xA0/0xA1 */
      ptSSD1306->tWrite(ptSSD1306->pInstance, &ptSSD1306->tWriteData);
      ptSSD1306->tWriteData.u8Count = 1;
      ptSSD1306->tWriteData.bData = false;
      ptSSD1306->tWriteData.au8Data[0] = 0xC8;  /* Set COM Output Scan Direction 0xC0,/0xC8 */
      ptSSD1306->tWrite(ptSSD1306->pInstance, &ptSSD1306->tWriteData);
      ptSSD1306->tWriteData.u8Count = 2;
      ptSSD1306->tWriteData.bData = false;
      ptSSD1306->tWriteData.au8Data[0] = 0xDA;  /* Set COM Pins hardware configuration 0xDA, 0x02 */
      ptSSD1306->tWriteData.au8Data[1] = 0b00010010;
      ptSSD1306->tWrite(ptSSD1306->pInstance, &ptSSD1306->tWriteData);
      ptSSD1306->tWriteData.u8Count = 2;
      ptSSD1306->tWriteData.bData = false;
      ptSSD1306->tWriteData.au8Data[0] = 0x81;  /* Set Contrast Control 0x81, default=0x7F */
      ptSSD1306->tWriteData.au8Data[1] = ptSSD1306->u8Contrast;
      ptSSD1306->tWrite(ptSSD1306->pInstance, &ptSSD1306->tWriteData);
      ptSSD1306->tWriteData.u8Count = 1;
      ptSSD1306->tWriteData.bData = false;
      ptSSD1306->tWriteData.au8Data[0] = 0xA4;  /* Disable Entire Display On */
      ptSSD1306->tWrite(ptSSD1306->pInstance, &ptSSD1306->tWriteData);
      ptSSD1306->tWriteData.u8Count = 1;
      ptSSD1306->tWriteData.bData = false;
      ptSSD1306->tWriteData.au8Data[0] = 0xA6;  /* Set Normal Display 0xA6, Inverse display 0xA7 */
      ptSSD1306->tWrite(ptSSD1306->pInstance, &ptSSD1306->tWriteData);
      ptSSD1306->tWriteData.u8Count = 2;
      ptSSD1306->tWriteData.bData = false;
      ptSSD1306->tWriteData.au8Data[0] = 0xD5;  /* Set Display Clock Divide Ratio/Oscillator Frequency 0xD5, 0x80 */
      ptSSD1306->tWriteData.au8Data[1] = 0b10000000;
      ptSSD1306->tWrite(ptSSD1306->pInstance, &ptSSD1306->tWriteData);
      ptSSD1306->tWriteData.u8Count = 1;
      ptSSD1306->tWriteData.bData = false;
      ptSSD1306->tWriteData.au8Data[0] = 0x2E;  /* Deactivate scroll */
      ptSSD1306->tWrite(ptSSD1306->pInstance, &ptSSD1306->tWriteData);
      if(ptSSD1306->tEndTransmission) {
        ptSSD1306->tEndTransmission(ptSSD1306->pInstance);
      }
      Unlock(ptSSD1306);
      Lock(ptSSD1306);
      if(ptSSD1306->tBeginTransmission) {
        ptSSD1306->tBeginTransmission(ptSSD1306->pInstance);
      }
      ptSSD1306->tWriteData.u8Count = 2;
      ptSSD1306->tWriteData.bData = false;
      ptSSD1306->tWriteData.au8Data[0] = 0x20;  /* Set Memory Addressing Mode  0x10:Page addressing mode */
      ptSSD1306->tWriteData.au8Data[1] = 0x10;
      ptSSD1306->tWrite(ptSSD1306->pInstance, &ptSSD1306->tWriteData);
      ptSSD1306->tWriteData.u8Count = 3;
      ptSSD1306->tWriteData.bData = false;
      ptSSD1306->tWriteData.au8Data[0] = 0x21;  /* Set Column Address Start column, End column */
      ptSSD1306->tWriteData.au8Data[1] = 0;
      ptSSD1306->tWriteData.au8Data[2] = ptSSD1306->u8MaxX - 1;
      ptSSD1306->tWrite(ptSSD1306->pInstance, &ptSSD1306->tWriteData);
      ptSSD1306->tWriteData.u8Count = 3;
      ptSSD1306->tWriteData.bData = false;
      ptSSD1306->tWriteData.au8Data[0] = 0x22;  /* Set Page Address Start page, End page */
      ptSSD1306->tWriteData.au8Data[1] = 0;
      ptSSD1306->tWriteData.au8Data[2] = ptSSD1306->u8MaxY / 8 - 1;
      ptSSD1306->tWrite(ptSSD1306->pInstance, &ptSSD1306->tWriteData);
      ptSSD1306->tWriteData.u8Count = 2;
      ptSSD1306->tWriteData.bData = false;
      ptSSD1306->tWriteData.au8Data[0] = 0x8D;  /* Set Enable charge pump regulator 0x8D, 0x14 */
      ptSSD1306->tWriteData.au8Data[1] = 0x14;
      ptSSD1306->tWrite(ptSSD1306->pInstance, &ptSSD1306->tWriteData);
      ptSSD1306->tWriteData.u8Count = 1;
      ptSSD1306->tWriteData.bData = false;
      ptSSD1306->tWriteData.au8Data[0] = 0xAF;  /* Display On */
      ptSSD1306->tWrite(ptSSD1306->pInstance, &ptSSD1306->tWriteData);
      if(ptSSD1306->tEndTransmission) {
        ptSSD1306->tEndTransmission(ptSSD1306->pInstance);
      }
      Unlock(ptSSD1306);
    }
  }

  return bValid;
}

bool SSD1306_SetContrast(SSD1306_t* ptSSD1306, const uint8_t u8Contrast) {
  bool bValid = false;

  if(ptSSD1306) {
    if(ptSSD1306->tWrite) {
      bValid = true;
      ptSSD1306->u8Contrast = u8Contrast;
      Lock(ptSSD1306);
      if(ptSSD1306->tBeginTransmission) {
        ptSSD1306->tBeginTransmission(ptSSD1306->pInstance);
      }
      ptSSD1306->tWriteData.u8Count = 2;
      ptSSD1306->tWriteData.bData = false;
      ptSSD1306->tWriteData.au8Data[0] = 0x81;  /* Set Contrast */
      ptSSD1306->tWriteData.au8Data[1] = u8Contrast;
      ptSSD1306->tWrite(ptSSD1306->pInstance, &ptSSD1306->tWriteData);
      if(ptSSD1306->tEndTransmission) {
        ptSSD1306->tEndTransmission(ptSSD1306->pInstance);
      }
      Unlock(ptSSD1306);
    }
  }

  return bValid;
}

bool SSD1306_Clear(SSD1306_t* ptSSD1306) {
  bool bValid = false;
  uint8_t u8Page, u8Seg, u8Count;

  if(ptSSD1306) {
    bValid = true;
#ifndef SSD1306_FRAMEBUFFER_STATIC
    if(ptSSD1306->pu8FrameBuffer) {
      memset(ptSSD1306->pu8FrameBuffer, 0, ptSSD1306->u8MaxY * ptSSD1306->u8MaxX / 8);
    } else {
      for(u8Page = 0; u8Page < ptSSD1306->u8MaxY / 8; u8Page++) {
        Lock(ptSSD1306);
        if(ptSSD1306->tBeginTransmission) {
          ptSSD1306->tBeginTransmission(ptSSD1306->pInstance);
        }
        ptSSD1306->tWriteData.u8Count = 1;
        ptSSD1306->tWriteData.bData = false;
        ptSSD1306->tWriteData.au8Data[0] = 0xB0 | u8Page;  /* set page start address(B0～B7) */
        ptSSD1306->tWrite(ptSSD1306->pInstance, &ptSSD1306->tWriteData);
        ptSSD1306->tWriteData.u8Count = 3;
        ptSSD1306->tWriteData.bData = false;
        ptSSD1306->tWriteData.au8Data[0] = 0x21;  /* set Column Address */
        ptSSD1306->tWriteData.au8Data[1] = 0;
        ptSSD1306->tWriteData.au8Data[2] = ptSSD1306->u8MaxX - 1;
        ptSSD1306->tWrite(ptSSD1306->pInstance, &ptSSD1306->tWriteData);
        if(ptSSD1306->tEndTransmission) {
          ptSSD1306->tEndTransmission(ptSSD1306->pInstance);
        }
        Unlock(ptSSD1306);

        ptSSD1306->tWriteData.u8Count = 16;
        ptSSD1306->tWriteData.bData = true;
        for(u8Count = 0; u8Count < 16; u8Count++) {
          ptSSD1306->tWriteData.au8Data[u8Count] = ptSSD1306->bInvert ? 0xFF : 0;
        }
        for(u8Seg = 0; u8Seg < ptSSD1306->u8MaxX; u8Seg += 16) {  /* column = 8Byte x 16 */
          Lock(ptSSD1306);
          if(ptSSD1306->tBeginTransmission) {
            ptSSD1306->tBeginTransmission(ptSSD1306->pInstance);
          }
          ptSSD1306->tWrite(ptSSD1306->pInstance, &ptSSD1306->tWriteData);
          if(ptSSD1306->tEndTransmission) {
            ptSSD1306->tEndTransmission(ptSSD1306->pInstance);
          }
          Unlock(ptSSD1306);
        }
      }
    }
#else
    memset(ptSSD1306->au8FrameBuffer, 0, ptSSD1306->u8MaxY * ptSSD1306->u8MaxX / 8);
#endif  /* SSD1306_FRAMEBUFFER_STATIC */
  }

  return bValid;
}

bool SSD1306_GetPixel(uint8_t* pu8Pixel, const SSD1306_t* ptSSD1306, const uint8_t u8X, const uint8_t u8Y) {
  bool bValid = false;
  uint8_t u8Pattern;

  if(pu8Pixel && ptSSD1306) {
    bValid = true;
#ifndef SSD1306_FRAMEBUFFER_STATIC
    if(ptSSD1306->pu8FrameBuffer) {
#endif  /* SSD1306_FRAMEBUFFER_STATIC */
      if(u8X < ptSSD1306->u8MaxX && u8Y < ptSSD1306->u8MaxY) {
        bValid = SSD1306_GetSeg(&u8Pattern, ptSSD1306, u8X, u8Y / 8);
        if(bValid) {
          *pu8Pixel = ((u8Pattern >> (u8Y % 8)) & 0x1);
        }
      }
#ifndef SSD1306_FRAMEBUFFER_STATIC
    } else {
      *pu8Pixel = 0;
    }
#endif  /* SSD1306_FRAMEBUFFER_STATIC */
  }

  return bValid;
}

bool SSD1306_GetSeg(uint8_t* pu8Pattern, const SSD1306_t* ptSSD1306, const uint8_t u8Seg, const uint8_t u8Page) {
  bool bValid = false;

  if(pu8Pattern && ptSSD1306) {
    bValid = true;
#ifndef SSD1306_FRAMEBUFFER_STATIC
    if(ptSSD1306->pu8FrameBuffer) {
      if(u8Seg < ptSSD1306->u8MaxX && u8Page < ptSSD1306->u8MaxY / 8) {
        *pu8Pattern = ptSSD1306->pu8FrameBuffer[u8Page * ptSSD1306->u8MaxX + u8Seg];
      }
    } else {
      *pu8Pattern = 0;
    }
#else
    if(u8Seg < ptSSD1306->u8MaxX && u8Page < ptSSD1306->u8MaxY / 8) {
      *pu8Pattern = ptSSD1306->au8FrameBuffer[u8Page * ptSSD1306->u8MaxX + u8Seg];
    }
#endif  /* SSD1306_FRAMEBUFFER_STATIC */
  }

  return bValid;
}

bool SSD1306_SetSeg(SSD1306_t* ptSSD1306, const uint8_t u8Seg, const uint8_t u8Page, const uint8_t u8Pattern) {
  bool bValid = false;

  if(ptSSD1306 && u8Seg < ptSSD1306->u8MaxX && u8Page < ptSSD1306->u8MaxY / 8) {
    bValid = true;
#ifndef SSD1306_FRAMEBUFFER_STATIC
    if(ptSSD1306->pu8FrameBuffer) {
      ptSSD1306->pu8FrameBuffer[u8Page * ptSSD1306->u8MaxX + u8Seg] = u8Pattern;
    } else {
      Lock(ptSSD1306);
      if(ptSSD1306->tBeginTransmission) {
        ptSSD1306->tBeginTransmission(ptSSD1306->pInstance);
      }
      ptSSD1306->tWriteData.u8Count = 1;
      ptSSD1306->tWriteData.bData = false;
      ptSSD1306->tWriteData.au8Data[0] = 0xB0 | u8Page;  /* set page start address(B0～B7) */
      ptSSD1306->tWrite(ptSSD1306->pInstance, &ptSSD1306->tWriteData);
      ptSSD1306->tWriteData.u8Count = 3;
      ptSSD1306->tWriteData.bData = false;
      ptSSD1306->tWriteData.au8Data[0] = 0x21;  /* set Column Address */
      ptSSD1306->tWriteData.au8Data[1] = u8Seg;
      ptSSD1306->tWriteData.au8Data[2] = u8Seg;
      ptSSD1306->tWrite(ptSSD1306->pInstance, &ptSSD1306->tWriteData);
      if(ptSSD1306->tEndTransmission) {
        ptSSD1306->tEndTransmission(ptSSD1306->pInstance);
      }
      Unlock(ptSSD1306);

      Lock(ptSSD1306);
      if(ptSSD1306->tBeginTransmission) {
        ptSSD1306->tBeginTransmission(ptSSD1306->pInstance);
      }
      ptSSD1306->tWriteData.u8Count = 1;
      ptSSD1306->tWriteData.bData = true;
      ptSSD1306->tWriteData.au8Data[0] = ptSSD1306->bInvert ? ~u8Pattern : u8Pattern;
      ptSSD1306->tWrite(ptSSD1306->pInstance, &ptSSD1306->tWriteData);
      if(ptSSD1306->tEndTransmission) {
        ptSSD1306->tEndTransmission(ptSSD1306->pInstance);
      }
      Unlock(ptSSD1306);
    }
#else
    ptSSD1306->au8FrameBuffer[u8Page * ptSSD1306->u8MaxX + u8Seg] = u8Pattern;
#endif  /* SSD1306_FRAMEBUFFER_STATIC */
  }

  return bValid;
}

bool SSD1306_DrawPixel(SSD1306_t* ptSSD1306, const int16_t i16X, const int16_t i16Y, const uint8_t u8Color) {
  bool bValid = false;
  uint8_t u8Pattern;

  if(ptSSD1306) {
    bValid = true;
    if(i16X >= 0 && i16X < ptSSD1306->u8MaxX && i16Y >= 0 && i16Y < ptSSD1306->u8MaxY) {
#ifndef SSD1306_FRAMEBUFFER_STATIC
      if(ptSSD1306->pu8FrameBuffer) {
#endif  /* SSD1306_FRAMEBUFFER_STATIC */
        SSD1306_GetSeg(&u8Pattern, ptSSD1306, i16X, i16Y / 8);
        if(u8Color) {
          u8Pattern |=   1 << (i16Y % 8) ;
        } else {
          u8Pattern &= ~(1 << (i16Y % 8));
        }
        SSD1306_SetSeg(ptSSD1306, i16X, i16Y / 8, u8Pattern);
#ifndef SSD1306_FRAMEBUFFER_STATIC
      } else {
        SSD1306_SetSeg(ptSSD1306, i16X, i16Y / 8, u8Color ? 0xFF : 0x00);
      }
#endif  /* SSD1306_FRAMEBUFFER_STATIC */
    }
  }

  return bValid;
}

bool SSD1306_DrawLine(
  SSD1306_t* ptSSD1306,
  const int16_t i16X1,
  const int16_t i16Y1,
  const int16_t i16X2,
  const int16_t i16Y2,
  const uint8_t u8Color
) {
  bool bValid = false;
  int16_t i16UX1, i16UX2, i16UY1, i16UY2;
  double dA;
  int16_t i16B;
  int16_t i16Temp;
  int16_t i16Locate;
  uint8_t u8Touch = 0;

  if(ptSSD1306) {
    if(ptSSD1306->tWrite) {
      bValid = true;
      if(i16X1 <= i16X2) {
        i16UX1 = i16X1;
        i16UX2 = i16X2;
        i16UY1 = i16Y1;
        i16UY2 = i16Y2;
      } else {
        i16UX1 = i16X2;
        i16UX2 = i16X1;
        i16UY1 = i16Y2;
        i16UY2 = i16Y1;
      }
      if(i16UX1 == i16UX2) {
        if(i16UX1 >= 0 && i16UX1 < ptSSD1306->u8MaxX) {
          if(i16UY1 < 0) {
            i16UY1 = 0;
          }
          if(i16UY1 >= ptSSD1306->u8MaxY) {
            i16UY1 = ptSSD1306->u8MaxY - 1;
          }
          if(i16UY2 < 0) {
            i16UY2 = 0;
          }
          if(i16UY2 >= ptSSD1306->u8MaxY) {
            i16UY2 = ptSSD1306->u8MaxY - 1;
          }
          u8Touch = 1;
        }
      } else if(i16UY1 == i16UY2) {
        if(i16UY1 >= 0 && i16UY1 < ptSSD1306->u8MaxY) {
          if(i16UX1 < 0) {
            i16UX1 = 0;
          }
          if(i16UX1 >= ptSSD1306->u8MaxX) {
            i16UX1 = ptSSD1306->u8MaxX - 1;
          }
          if(i16UX2 < 0) {
            i16UX2 = 0;
          }
          if(i16UX2 >= ptSSD1306->u8MaxX) {
            i16UX2 = ptSSD1306->u8MaxX - 1;
          }
          u8Touch = 1;
        }
      } else {
        dA = ((double)i16UY2 - i16UY1) / (i16UX2 - i16UX1);
        i16B = ((i16UY1 * 2) + 1) - (int16_t)(dA * ((i16UX1 * 2) + 1));

        i16Temp = dA + i16B;
        if(i16Temp >= 0 && i16Temp < (ptSSD1306->u8MaxY * 2)) {
          if(!u8Touch) {
            i16UX1 = 0;
            i16UY1 = i16Temp / 2;
          } else {
            i16UX2 = 0;
            i16UY2 = i16Temp / 2;
          }
          u8Touch |= 1 << 0;
        }
        i16Temp = (int16_t)(dA * ((ptSSD1306->u8MaxX * 2) - 1)) + i16B;
        if(i16Temp >= 0 && i16Temp < (ptSSD1306->u8MaxY * 2)) {
          if(!u8Touch) {
            i16UX1 = ptSSD1306->u8MaxX - 1;
            i16UY1 = i16Temp / 2;
          } else {
            i16UX2 = ptSSD1306->u8MaxX - 1;
            i16UY2 = i16Temp / 2;
          }
          u8Touch |= 1 << 1;
        }
        i16Temp = (int16_t)((1 - i16B) / dA);
        if(i16Temp >= 0 && i16Temp < (ptSSD1306->u8MaxX * 2)) {
          if(!u8Touch) {
            i16UY1 = i16Temp / 2;
            i16UY1 = 0;
          } else {
            i16UX2 = i16Temp / 2;
            i16UY2 = 0;
          }
          u8Touch |= 1 << 2;
        }
        i16Temp = (int16_t)((((ptSSD1306->u8MaxY * 2) - 1) - i16B) / dA);
        if(i16Temp >= 0 && i16Temp < (ptSSD1306->u8MaxX * 2)) {
          if(!u8Touch) {
            i16UX1 = i16Temp / 2;
            i16UY1 = ptSSD1306->u8MaxY - 1;
          } else {
            i16UX2 = i16Temp / 2;
            i16UY2 = ptSSD1306->u8MaxY - 1;
          }
          u8Touch |= 1 << 3;
        }
        if(u8Touch) {
          if(
            (i16X1 >= 0 && i16X1 < ptSSD1306->u8MaxX) &&
            (i16Y1 >= 0 && i16Y1 < ptSSD1306->u8MaxY)
          ) {
            i16UX1 = i16X1;
            i16UY1 = i16Y1;
          } else if(
            (i16X2 >= 0 && i16X2 < ptSSD1306->u8MaxX) &&
            (i16Y2 >= 0 && i16Y2 < ptSSD1306->u8MaxY)
          ) {
            i16UX2 = i16X2;
            i16UY2 = i16Y2;
          }
          if(i16UX1 > i16UX2) {
            i16Temp = i16UX2;
            i16UX2 = i16UX1;
            i16UX1 = i16Temp;
            i16Temp = i16UY2;
            i16UY2 = i16UY1;
            i16UY1 = i16Temp;
          }
        }
      }

      if(u8Touch) {
        if(i16UX1 == i16UX2) {
          if(i16UY1 <= i16UY2) {
            for(i16Locate = i16UY1; i16Locate <= i16UY2; i16Locate++) {
              SSD1306_DrawPixel(ptSSD1306, i16UX1, i16Locate, u8Color);
            }
          } else {
            for(i16Locate = i16UY2; i16Locate <= i16UY1; i16Locate++) {
              SSD1306_DrawPixel(ptSSD1306, i16UX1, i16Locate, u8Color);
            }
          }
        } else if(i16UY1 == i16UY2) {
          for(i16Locate = i16UX1; i16Locate <= i16UX2; i16Locate++) {
            SSD1306_DrawPixel(ptSSD1306, i16Locate, i16UY1, u8Color);
          }
        } else {
          if(i16UY1 <= i16UY2) {
            if(i16UX2 - i16UX1 > i16UY2 - i16UY1) {
              for(i16Locate = i16UX1; i16Locate <= i16UX2; i16Locate++) {
                SSD1306_DrawPixel(ptSSD1306, i16Locate, (int16_t)(dA * i16Locate) + i16B, u8Color);
              }
            } else {
              for(i16Locate = i16UY1; i16Locate <= i16UY2; i16Locate++) {
                SSD1306_DrawPixel(ptSSD1306, (int16_t)((i16Locate - i16B) / dA), i16Locate, u8Color);
              }
            }
          } else {
            if(i16UX2 - i16UX1 > i16UY2 - i16UY1) {
              for(i16Locate = i16UX1; i16Locate <= i16UX2; i16Locate++) {
                SSD1306_DrawPixel(ptSSD1306, i16Locate, (int16_t)(dA * i16Locate) + i16B, u8Color);
              }
            } else {
              for(i16Locate = i16UY2; i16Locate <= i16UY1; i16Locate++) {
                SSD1306_DrawPixel(ptSSD1306, (int16_t)((i16Locate - i16B) / dA), i16Locate, u8Color);
              }
            }
          }
        }
      }
    }
  }

  return bValid;
}

bool SSD1306_DrawRectangle(
  SSD1306_t* ptSSD1306,
  const int16_t i16X1,
  const int16_t i16Y1,
  const int16_t i16X2,
  const int16_t i16Y2,
  const uint8_t u8Color,
  const bool bFill,
  const uint8_t u8FillColor
) {
  bool bValid = false;
  int16_t i16UX1, i16UX2, i16UY1, i16UY2;
  int16_t i16Locate;
  bool bDraw = false;

  if(ptSSD1306) {
    if(ptSSD1306->tWrite) {
      bValid = true;
      if(
        (i16X1 >= 0 && i16X1 < ptSSD1306->u8MaxX) &&
        (i16Y1 >= 0 && i16Y1 < ptSSD1306->u8MaxY) &&
        (i16X2 >= 0 && i16X2 < ptSSD1306->u8MaxX) &&
        (i16Y2 >= 0 && i16Y2 < ptSSD1306->u8MaxY)
      ) {
        i16UX1 = i16X1;
        i16UX2 = i16X2;
        i16UY1 = i16Y1;
        i16UY2 = i16Y2;
        bDraw = true;
      } else {
        if(i16X1 <= i16X2) {
          i16UX1 = i16X1;
          i16UX2 = i16X2;
          i16UY1 = i16Y1;
          i16UY2 = i16Y2;
        } else {
          i16UX1 = i16X2;
          i16UX2 = i16X1;
          i16UY1 = i16Y2;
          i16UY2 = i16Y1;
        }
        if(i16UX1 < 0) {
          if(i16UY1 < 0) {
            if(i16UX2 >= 0) {
              if(i16UY2 >= 0) {
                bDraw = true;
              }
            }
          } else {
            if(i16UY1 < ptSSD1306->u8MaxY) {
              if(i16UX2 >= 0) {
                bDraw = true;
              }
            } else {
              if(i16UX2 >= 0) {
                if(i16UY2 >= ptSSD1306->u8MaxY) {
                  bDraw = true;
                }
              }
            }
          }
        } else {
          if(i16UX1 < ptSSD1306->u8MaxX) {
            if(i16UY1 < 0) {
              if(i16UY2 >= 0) {
                bDraw = true;
              }
            } else {
              if(i16Y1 < ptSSD1306->u8MaxY) {
                bDraw = true;
              } else {
                if(i16UY2 < ptSSD1306->u8MaxY) {
                  bDraw = true;
                }
              }
            }
          }
        }
      }

      if(bDraw) {
        if(bFill) {
          for(i16Locate = i16UX1; i16Locate < i16UX2; i16Locate++) {
            SSD1306_DrawLine(ptSSD1306, i16Locate, i16UY1, i16Locate, i16UY2, u8FillColor);
          }
        }
        SSD1306_DrawLine(ptSSD1306, i16UX1, i16UY1, i16UX2, i16UY1, u8Color);
        SSD1306_DrawLine(ptSSD1306, i16UX1, i16UY2, i16UX2, i16UY2, u8Color);
        SSD1306_DrawLine(ptSSD1306, i16UX1, i16UY1, i16UX1, i16UY2, u8Color);
        SSD1306_DrawLine(ptSSD1306, i16UX2, i16UY1, i16UX2, i16UY2, u8Color);
      }
    }
  }

  return bValid;
}

bool SSD1306_DrawCircle(
  SSD1306_t* ptSSD1306,
  const int16_t i16X,
  const int16_t i16Y,
  const int16_t i16R,
  const uint8_t u8Color,
  const bool bFill,
  const uint8_t u8FillColor
) {
  bool bValid = false;
  int16_t i16UX, i16UY, i16F;

  if(ptSSD1306 && i16R > 0) {
    if(ptSSD1306->tWrite) {
      bValid = true;
      if(
        (i16X >= -1 * i16R && i16X < ptSSD1306->u8MaxX + i16R) &&
        (i16Y >= -1 * i16R && i16Y < ptSSD1306->u8MaxY + i16R)
      ) {
        if(bFill) {
          i16UX = i16R;
          i16UY = 0;
          i16F = -2 * i16R + 3;
          while(i16UX >= i16UY) {
            SSD1306_DrawLine(ptSSD1306, i16X + i16UX, i16Y + i16UY, i16X - i16UX, i16Y + i16UY, u8FillColor);
            SSD1306_DrawLine(ptSSD1306, i16X + i16UX, i16Y - i16UY, i16X - i16UX, i16Y - i16UY, u8FillColor);
            SSD1306_DrawLine(ptSSD1306, i16X + i16UY, i16Y + i16UX, i16X - i16UY, i16Y + i16UX, u8FillColor);
            SSD1306_DrawLine(ptSSD1306, i16X + i16UY, i16Y - i16UX, i16X - i16UY, i16Y - i16UX, u8FillColor);
            if(i16F >= 0) {
              i16UX--;
              i16F -= 4 * i16UX;
            }
            i16UY++;
            i16F += 4 * i16UY + 2;
          }
        }

        i16UX = i16R;
        i16UY = 0;
        i16F = -2 * i16R + 3;
        while(i16UX >= i16UY) {
          SSD1306_DrawPixel(ptSSD1306, i16X + i16UX, i16Y + i16UY, u8Color);
          SSD1306_DrawPixel(ptSSD1306, i16X - i16UX, i16Y + i16UY, u8Color);
          SSD1306_DrawPixel(ptSSD1306, i16X + i16UX, i16Y - i16UY, u8Color);
          SSD1306_DrawPixel(ptSSD1306, i16X - i16UX, i16Y - i16UY, u8Color);
          SSD1306_DrawPixel(ptSSD1306, i16X + i16UY, i16Y + i16UX, u8Color);
          SSD1306_DrawPixel(ptSSD1306, i16X - i16UY, i16Y + i16UX, u8Color);
          SSD1306_DrawPixel(ptSSD1306, i16X + i16UY, i16Y - i16UX, u8Color);
          SSD1306_DrawPixel(ptSSD1306, i16X - i16UY, i16Y - i16UX, u8Color);
          if(i16F >= 0) {
            i16UX--;
            i16F -= 4 * i16UX;
          }
          i16UY++;
          i16F += 4 * i16UY + 2;
        }
      }
    }
  }

  return bValid;
}

bool SSD1306_Refresh(SSD1306_t* ptSSD1306) {
  bool bValid = false;
  uint8_t u8Page, u8Seg, u8Count;

  if(ptSSD1306) {
    if(ptSSD1306->tWrite) {
      bValid = true;
#ifndef SSD1306_FRAMEBUFFER_STATIC
      if(ptSSD1306->pu8FrameBuffer) {
#endif  /* SSD1306_FRAMEBUFFER_STATIC */
        for(u8Page = 0; u8Page < ptSSD1306->u8MaxY / 8; u8Page++) {
          Lock(ptSSD1306);
          if(ptSSD1306->tBeginTransmission) {
            ptSSD1306->tBeginTransmission(ptSSD1306->pInstance);
          }
          ptSSD1306->tWriteData.u8Count = 1;
          ptSSD1306->tWriteData.bData = false;
          ptSSD1306->tWriteData.au8Data[0] = 0xB0 | u8Page;  /* set page start address(B0～B7) */
          ptSSD1306->tWrite(ptSSD1306->pInstance, &ptSSD1306->tWriteData);
          ptSSD1306->tWriteData.u8Count = 3;
          ptSSD1306->tWriteData.bData = false;
          ptSSD1306->tWriteData.au8Data[0] = 0x21;  /* set Column Address */
          ptSSD1306->tWriteData.au8Data[1] = 0;
          ptSSD1306->tWriteData.au8Data[2] = ptSSD1306->u8MaxX - 1;
          ptSSD1306->tWrite(ptSSD1306->pInstance, &ptSSD1306->tWriteData);
          if(ptSSD1306->tEndTransmission) {
            ptSSD1306->tEndTransmission(ptSSD1306->pInstance);
          }
          Unlock(ptSSD1306);

          ptSSD1306->tWriteData.u8Count = 16;
          ptSSD1306->tWriteData.bData = true;
          for(u8Seg = 0; u8Seg < ptSSD1306->u8MaxX; u8Seg += 16) {  /* column = 8Byte x 16 */
            for(u8Count = 0; u8Count < 16; u8Count++) {             /* continue to 31Byte (this use 16Byte) */
#ifndef SSD1306_FRAMEBUFFER_STATIC
              ptSSD1306->tWriteData.au8Data[u8Count] = ptSSD1306->pu8FrameBuffer[u8Page * ptSSD1306->u8MaxX + u8Seg + u8Count];
#else
              ptSSD1306->tWriteData.au8Data[u8Count] = ptSSD1306->au8FrameBuffer[u8Page * ptSSD1306->u8MaxX + u8Seg + u8Count];
#endif  /* SSD1306_FRAMEBUFFER_STATIC */
              if(ptSSD1306->bInvert) {
                ptSSD1306->tWriteData.au8Data[u8Count] = ~ptSSD1306->tWriteData.au8Data[u8Count];
              } else {
                ptSSD1306->tWriteData.au8Data[u8Count] =  ptSSD1306->tWriteData.au8Data[u8Count];
              }
            }
            Lock(ptSSD1306);
            if(ptSSD1306->tBeginTransmission) {
              ptSSD1306->tBeginTransmission(ptSSD1306->pInstance);
            }
            ptSSD1306->tWrite(ptSSD1306->pInstance, &ptSSD1306->tWriteData);
            if(ptSSD1306->tEndTransmission) {
              ptSSD1306->tEndTransmission(ptSSD1306->pInstance);
            }
            Unlock(ptSSD1306);
          }
        }
#ifndef SSD1306_FRAMEBUFFER_STATIC
      }
#endif  /* SSD1306_FRAMEBUFFER_STATIC */
    }
  }

  return bValid;
}

bool SSD1306_HScroll(SSD1306_t* ptSSD1306, const bool bLeft, const uint8_t u8StartPage, const uint8_t u8Intreval, const uint8_t u8EndPage) {
  bool bValid = false;

  if(ptSSD1306) {
    if(ptSSD1306->tWrite && u8StartPage < ptSSD1306->u8MaxY / 8 && u8EndPage < ptSSD1306->u8MaxY / 8) {
      bValid = true;
      Lock(ptSSD1306);
      if(ptSSD1306->tBeginTransmission) {
        ptSSD1306->tBeginTransmission(ptSSD1306->pInstance);
      }
      ptSSD1306->tWriteData.u8Count = 8;
      ptSSD1306->tWriteData.bData = false;
      ptSSD1306->tWriteData.au8Data[0] = 0x2E;         /* For configuration, once off the scroll. */
      if(bLeft) {
        ptSSD1306->tWriteData.au8Data[1] = 0x27;       /* Horizontal scroll set. 0x26=Right/0x27=Left direction. */
      } else {
        ptSSD1306->tWriteData.au8Data[1] = 0x26;       /* Horizontal scroll set. */
      }
      ptSSD1306->tWriteData.au8Data[2] = 0x00;         /* Dummy byte */
      ptSSD1306->tWriteData.au8Data[3] = u8StartPage;  /* Define start page address. */
      ptSSD1306->tWriteData.au8Data[4] = u8Intreval;   /* Set time interval pattern. */
      ptSSD1306->tWriteData.au8Data[5] = u8EndPage;    /* Define end page address. */
      ptSSD1306->tWriteData.au8Data[6] = 0x00;         /* Dummy byte */
      ptSSD1306->tWriteData.au8Data[7] = 0xFF;         /* Dummy byte */
      ptSSD1306->tWrite(ptSSD1306->pInstance, &ptSSD1306->tWriteData);
      ptSSD1306->tWriteData.u8Count = 1;
      ptSSD1306->tWriteData.bData = false;
      ptSSD1306->tWriteData.au8Data[0] = 0x2F;         /* Activate scroll */
      ptSSD1306->tWrite(ptSSD1306->pInstance, &ptSSD1306->tWriteData);
      if(ptSSD1306->tEndTransmission) {
        ptSSD1306->tEndTransmission(ptSSD1306->pInstance);
      }
      Unlock(ptSSD1306);
      ptSSD1306->bScroll = true;
    }
  }

  return bValid;
}

bool SSD1306_VHScroll(SSD1306_t* ptSSD1306, const bool bLeft, const uint8_t u8StartPage, const uint8_t u8Intreval, const uint8_t u8EndPage, const uint8_t u8Offset) {
  bool bValid = false;

  if(ptSSD1306) {
    if(ptSSD1306->tWrite && u8StartPage < ptSSD1306->u8MaxY / 8 && u8EndPage < ptSSD1306->u8MaxY / 8 && u8Intreval < 8) {
      bValid = true;
      Lock(ptSSD1306);
      if(ptSSD1306->tBeginTransmission) {
        ptSSD1306->tBeginTransmission(ptSSD1306->pInstance);
      }
      ptSSD1306->tWriteData.u8Count = 7;
      ptSSD1306->tWriteData.bData = false;
      ptSSD1306->tWriteData.au8Data[0] = 0x2E;         /* For configuration, once off the scroll. */
      if(bLeft) {
        ptSSD1306->tWriteData.au8Data[1] = 0x2A;       /* Horizontal scroll set. */
      } else {
        ptSSD1306->tWriteData.au8Data[1] = 0x29;       /* Horizontal scroll set. 0x29=V+Right/0x2A=V+Left direction. */
      }
      ptSSD1306->tWriteData.au8Data[2] = 0x00;         /* Dummy byte */
      ptSSD1306->tWriteData.au8Data[3] = u8StartPage;  /* Define start page address. */
      ptSSD1306->tWriteData.au8Data[4] = u8Intreval;   /* Set time interval pattern. */
      ptSSD1306->tWriteData.au8Data[5] = u8EndPage;    /* Define end page address. */
      ptSSD1306->tWriteData.au8Data[6] = u8Offset;     /* Vertical scrolling offset. */
      ptSSD1306->tWrite(ptSSD1306->pInstance, &ptSSD1306->tWriteData);
      ptSSD1306->tWriteData.u8Count = 1;
      ptSSD1306->tWriteData.bData = false;
      ptSSD1306->tWriteData.au8Data[0] = 0x2F;         /* Activate scroll */
      ptSSD1306->tWrite(ptSSD1306->pInstance, &ptSSD1306->tWriteData);
      if(ptSSD1306->tEndTransmission) {
        ptSSD1306->tEndTransmission(ptSSD1306->pInstance);
      }
      Unlock(ptSSD1306);
      ptSSD1306->bScroll = true;
    }
  }

  return bValid;
}

bool SSD1306_DisableScroll(SSD1306_t* ptSSD1306) {
  bool bValid = false;

  if(ptSSD1306) {
    if(ptSSD1306->tWrite && ptSSD1306->bScroll) {
      bValid = true;
      Lock(ptSSD1306);
      if(ptSSD1306->tBeginTransmission) {
        ptSSD1306->tBeginTransmission(ptSSD1306->pInstance);
      }
      ptSSD1306->tWriteData.u8Count = 1;
      ptSSD1306->tWriteData.bData = false;
      ptSSD1306->tWriteData.au8Data[0] = 0x2E;  /* For configuration, once off the scroll. */
      ptSSD1306->tWrite(ptSSD1306->pInstance, &ptSSD1306->tWriteData);
      if(ptSSD1306->tEndTransmission) {
        ptSSD1306->tEndTransmission(ptSSD1306->pInstance);
      }
      Unlock(ptSSD1306);
      ptSSD1306->bScroll = false;
    }
  }

  return bValid;
}

bool SSD1306_Sleep(SSD1306_t* ptSSD1306) {
  bool bValid = false;

  if(ptSSD1306) {
    if(ptSSD1306->tWrite) {
      bValid = true;
      if(!ptSSD1306->bSleep) {
        Lock(ptSSD1306);
        if(ptSSD1306->tBeginTransmission) {
          ptSSD1306->tBeginTransmission(ptSSD1306->pInstance);
        }
        ptSSD1306->tWriteData.u8Count = 1;
        ptSSD1306->tWriteData.bData = false;
        ptSSD1306->tWriteData.au8Data[0] = 0xAE;  /* Display Off */
        ptSSD1306->tWrite(ptSSD1306->pInstance, &ptSSD1306->tWriteData);
        if(ptSSD1306->tEndTransmission) {
          ptSSD1306->tEndTransmission(ptSSD1306->pInstance);
        }
        Unlock(ptSSD1306);
        ptSSD1306->bSleep = true;
      }
    }
  }

  return bValid;
}

bool SSD1306_Wakeup(SSD1306_t* ptSSD1306) {
  bool bValid = false;

  if(ptSSD1306) {
    if(ptSSD1306->tWrite) {
      bValid = true;
      if(ptSSD1306->bSleep) {
        Lock(ptSSD1306);
        if(ptSSD1306->tBeginTransmission) {
          ptSSD1306->tBeginTransmission(ptSSD1306->pInstance);
        }
        ptSSD1306->tWriteData.u8Count = 2;
        ptSSD1306->tWriteData.bData = false;
        ptSSD1306->tWriteData.au8Data[0] = 0x81;  /* Set Contrast Control 0x81, default=0x7F */
        ptSSD1306->tWriteData.au8Data[1] = ptSSD1306->u8Contrast;
        ptSSD1306->tWrite(ptSSD1306->pInstance, &ptSSD1306->tWriteData);
        ptSSD1306->tWriteData.u8Count = 1;
        ptSSD1306->tWriteData.au8Data[0] = 0xAF;  /* Display On */
        ptSSD1306->tWrite(ptSSD1306->pInstance, &ptSSD1306->tWriteData);
        if(ptSSD1306->tEndTransmission) {
          ptSSD1306->tEndTransmission(ptSSD1306->pInstance);
        }
        Unlock(ptSSD1306);
        ptSSD1306->bSleep = false;
      }
    }
  }

  return bValid;
}

