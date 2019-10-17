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
  SSD1306_Read_t tRead,
  SSD1306_Write_t tWrite,
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
#ifdef SSD1306_FRAMEBUFFER_SEGDIRTY
    ptSSD1306->pu8FBSegDirty = malloc(u8MaxY * u8MaxX / 64);
    if(!ptSSD1306->pu8FBSegDirty) {
      bValid = false;
    }
#endif  /* SSD1306_FRAMEBUFFER_SEGDIRTY */
#endif  /* SSD1306_FRAMEBUFFER_STATIC */
    if(bValid) {
      ptSSD1306->pInstance      = pInstance;
      ptSSD1306->tRead          = tRead;
      ptSSD1306->tWrite         = tWrite;
      ptSSD1306->tMemoryBarrier = tMemoryBarrier;
      ptSSD1306->ppLock         = ppLock;
      ptSSD1306->u8MaxX         = u8MaxX;
      ptSSD1306->u8MaxY         = u8MaxY;
      ptSSD1306->u8Contrast     = u8Contrast;
      ptSSD1306->bSleep         = false;
      ptSSD1306->bScroll        = false;
      ptSSD1306->bInvert        = false;

#ifdef SSD1306_FRAMEBUFFER_STATIC
      ptSSD1306->pu8FrameBuffer = ptSSD1306->au8FrameBuffer;
#ifdef SSD1306_FRAMEBUFFER_SEGDIRTY
      ptSSD1306->pu8FBSegDirty = ptSSD1306->au8FBSegDirty;
#endif  /* SSD1306_FRAMEBUFFER_SEGDIRTY */
#endif  /* SSD1306_FRAMEBUFFER_STATIC */

      ptSSD1306->tWriteCommand.u8Count = 0;
      ptSSD1306->tWriteCommand.bData = false;
      ptSSD1306->tWriteSeg.u8Count = 0;
      ptSSD1306->tWriteSeg.bData = true;
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
#ifdef SSD1306_FRAMEBUFFER_SEGDIRTY
    if(ptSSD1306->pu8FBSegDirty) {
      free(ptSSD1306->pu8FBSegDirty);
      ptSSD1306->pu8FBSegDirty = NULL;
    }
#endif  /* SSD1306_FRAMEBUFFER_SEGDIRTY */
  }

  return bValid;
}
#endif  /* SSD1306_FRAMEBUFFER_STATIC */

bool SSD1306_InitDevice(SSD1306_t* ptSSD1306) {
  bool bValid = false;

  if(ptSSD1306) {
    if(ptSSD1306->tWrite) {
      bValid = true;
#ifndef SSD1306_LCD_72X40
      ptSSD1306->tWriteCommand.u8Count = 28;
      ptSSD1306->tWriteCommand.au8Data[ 0] = 0xAE;  /* Display off */
      ptSSD1306->tWriteCommand.au8Data[ 1] = 0xA8;  /* Set Multiplex Ratio  0xA8, 0x3F */
      ptSSD1306->tWriteCommand.au8Data[ 2] = 0b00111111;
      ptSSD1306->tWriteCommand.au8Data[ 3] = 0xD3;  /* Set Display Offset 0xD3, 0x00 */
      ptSSD1306->tWriteCommand.au8Data[ 4] = 0x00;
      ptSSD1306->tWriteCommand.au8Data[ 5] = 0x40;  /* Set Display Start Line 0x40 */
      ptSSD1306->tWriteCommand.au8Data[ 6] = 0xA1;  /* Set Segment re-map 0xA0/0xA1 */
      ptSSD1306->tWriteCommand.au8Data[ 7] = 0xC8;  /* Set COM Output Scan Direction 0xC0,/0xC8 */
      ptSSD1306->tWriteCommand.au8Data[ 8] = 0xDA;  /* Set COM Pins hardware configuration 0xDA, 0x12 */
      ptSSD1306->tWriteCommand.au8Data[ 9] = 0b00010010;
      ptSSD1306->tWriteCommand.au8Data[10] = 0x81;  /* Set Contrast Control 0x81, default=0x7F */
      ptSSD1306->tWriteCommand.au8Data[11] = ptSSD1306->u8Contrast;
      ptSSD1306->tWriteCommand.au8Data[12] = 0xA4;  /* Disable Entire Display On */
      ptSSD1306->tWriteCommand.au8Data[13] = 0xA6;  /* Set Normal Display 0xA6, Inverse display 0xA7 */
      ptSSD1306->tWriteCommand.au8Data[14] = 0xD5;  /* Set Display Clock Divide Ratio/Oscillator Frequency 0xD5, 0x80 */
      ptSSD1306->tWriteCommand.au8Data[15] = 0b10000000;
      ptSSD1306->tWriteCommand.au8Data[16] = 0x2E;  /* Deactivate scroll */
      ptSSD1306->tWriteCommand.au8Data[17] = 0x20;  /* Set Memory Addressing Mode  0x10:Page addressing mode */
      ptSSD1306->tWriteCommand.au8Data[18] = 0x10;
      ptSSD1306->tWriteCommand.au8Data[19] = 0x21;  /* Set Column Address Start column, End column */
      ptSSD1306->tWriteCommand.au8Data[20] = 0;
      ptSSD1306->tWriteCommand.au8Data[21] = ptSSD1306->u8MaxX - 1;
      ptSSD1306->tWriteCommand.au8Data[22] = 0x22;  /* Set Page Address Start page, End page */
      ptSSD1306->tWriteCommand.au8Data[23] = 0;
      ptSSD1306->tWriteCommand.au8Data[24] = ptSSD1306->u8MaxY / 8 - 1;
      ptSSD1306->tWriteCommand.au8Data[25] = 0x8D;  /* Set Enable charge pump regulator 0x8D, 0x14 */
      ptSSD1306->tWriteCommand.au8Data[26] = 0x14;
      ptSSD1306->tWriteCommand.au8Data[27] = 0xAF;  /* Display On */
#else  /* SSD1306_LCD_72X40 */
      ptSSD1306->tWriteCommand.u8Count = 25;
      ptSSD1306->tWriteCommand.au8Data[ 0] = 0xae;//--turn off oled panel
      ptSSD1306->tWriteCommand.au8Data[ 1] = 0xd5;//--set display clock divide ratio/oscillator frequency
      ptSSD1306->tWriteCommand.au8Data[ 2] = 0x80;//--set divide ratio
      ptSSD1306->tWriteCommand.au8Data[ 3] = 0xa8;//--set multiplex ratio
      ptSSD1306->tWriteCommand.au8Data[ 4] = 0x27;//--1/40 duty
      ptSSD1306->tWriteCommand.au8Data[ 5] = 0xd3;//-set display offset
      ptSSD1306->tWriteCommand.au8Data[ 6] = 0x00;//-not offset
      ptSSD1306->tWriteCommand.au8Data[ 7] = 0xad;//--Internal IREF Setting 
      ptSSD1306->tWriteCommand.au8Data[ 8] = 0x30;//--
      ptSSD1306->tWriteCommand.au8Data[ 9] = 0x8d;//--set Charge Pump enable/disable
      ptSSD1306->tWriteCommand.au8Data[10] = 0x14;//--set(0x10) disable
      ptSSD1306->tWriteCommand.au8Data[11] = 0x40;//--set start line address
      ptSSD1306->tWriteCommand.au8Data[12] = 0xa6;//--set normal display
      ptSSD1306->tWriteCommand.au8Data[13] = 0xa4;//Disable Entire Display On
      ptSSD1306->tWriteCommand.au8Data[14] = 0xa1;//--set segment re-map 128 to 0
      ptSSD1306->tWriteCommand.au8Data[15] = 0xC8;//--Set COM Output Scan Direction 64 to 0
      ptSSD1306->tWriteCommand.au8Data[16] = 0xda;//--set com pins hardware configuration
      ptSSD1306->tWriteCommand.au8Data[17] = 0x12;
      ptSSD1306->tWriteCommand.au8Data[18] = 0x81;//--set contrast control register
      ptSSD1306->tWriteCommand.au8Data[19] = 0xaf;
      ptSSD1306->tWriteCommand.au8Data[20] = 0xd9;//--set pre-charge period
      ptSSD1306->tWriteCommand.au8Data[21] = 0x22;
      ptSSD1306->tWriteCommand.au8Data[22] = 0xdb;//--set vcomh
      ptSSD1306->tWriteCommand.au8Data[23] = 0x20;
      ptSSD1306->tWriteCommand.au8Data[24] = 0xaf;//--turn on oled panel
#endif  /* SSD1306_LCD_72X40 */
      Lock(ptSSD1306);
      ptSSD1306->tWrite(ptSSD1306->pInstance, &ptSSD1306->tWriteCommand);
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
      ptSSD1306->tWriteCommand.u8Count = 2;
      ptSSD1306->tWriteCommand.au8Data[0] = 0x81;  /* Set Contrast */
      ptSSD1306->tWriteCommand.au8Data[1] = u8Contrast;
      Lock(ptSSD1306);
      ptSSD1306->tWrite(ptSSD1306->pInstance, &ptSSD1306->tWriteCommand);
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
#ifdef SSD1306_FRAMEBUFFER_SEGDIRTY
    if(ptSSD1306->pu8FrameBuffer && ptSSD1306->pu8FBSegDirty) {
#else  /* SSD1306_FRAMEBUFFER_SEGDIRTY */
    if(ptSSD1306->pu8FrameBuffer) {
#endif  /* SSD1306_FRAMEBUFFER_SEGDIRTY */
#endif  /* SSD1306_FRAMEBUFFER_STATIC */
      memset(ptSSD1306->pu8FrameBuffer, 0, ptSSD1306->u8MaxY * ptSSD1306->u8MaxX / 8);
      ptSSD1306->u8FBPageDirty = 0xFF;
#ifdef SSD1306_FRAMEBUFFER_SEGDIRTY
      memset(ptSSD1306->pu8FBSegDirty, 0xFF, ptSSD1306->u8MaxY * ptSSD1306->u8MaxX / 64);
#endif  /* SSD1306_FRAMEBUFFER_SEGDIRTY */
#ifndef SSD1306_FRAMEBUFFER_STATIC
    } else {
#endif  /* SSD1306_FRAMEBUFFER_STATIC */
      ptSSD1306->tWriteSeg.u8Count = 16;
      for(u8Count = 0; u8Count < 16; u8Count++) {
        ptSSD1306->tWriteSeg.au8Data[u8Count] = ptSSD1306->bInvert ? 0xFF : 0;
      }
      for(u8Page = 0; u8Page < ptSSD1306->u8MaxY >> 3; u8Page++) {
        ptSSD1306->tWriteCommand.u8Count = 4;
        ptSSD1306->tWriteCommand.au8Data[0] = 0xB0 | u8Page;  /* set page start address(B0～B7) */
        ptSSD1306->tWriteCommand.au8Data[1] = 0x21;  /* set Column Address */
        ptSSD1306->tWriteCommand.au8Data[2] = 0;
        ptSSD1306->tWriteCommand.au8Data[3] = ptSSD1306->u8MaxX - 1;
        Lock(ptSSD1306);
        ptSSD1306->tWrite(ptSSD1306->pInstance, &ptSSD1306->tWriteCommand);
        Unlock(ptSSD1306);

        for(u8Seg = 0; u8Seg < ptSSD1306->u8MaxX; u8Seg += 16) {  /* column = 8Byte x 16 */
          Lock(ptSSD1306);
          ptSSD1306->tWrite(ptSSD1306->pInstance, &ptSSD1306->tWriteSeg);
          Unlock(ptSSD1306);
        }
      }
#ifndef SSD1306_FRAMEBUFFER_STATIC
    }
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
#endif  /* SSD1306_FRAMEBUFFER_STATIC */
      if(u8Seg < ptSSD1306->u8MaxX && u8Page < ptSSD1306->u8MaxY / 8) {
        *pu8Pattern = ptSSD1306->pu8FrameBuffer[(uint16_t)u8Page * ptSSD1306->u8MaxX + u8Seg];
      }
#ifndef SSD1306_FRAMEBUFFER_STATIC
    } else {
      *pu8Pattern = 0;
    }
#endif  /* SSD1306_FRAMEBUFFER_STATIC */
  }

  return bValid;
}

bool SSD1306_SetSeg(SSD1306_t* ptSSD1306, const uint8_t u8Seg, const uint8_t u8Page, const uint8_t u8Pattern) {
  bool bValid = false;
  uint16_t u16Loc;

  if(ptSSD1306 && u8Seg < ptSSD1306->u8MaxX && u8Page < ptSSD1306->u8MaxY / 8) {
    bValid = true;
#ifndef SSD1306_FRAMEBUFFER_STATIC
#ifdef SSD1306_FRAMEBUFFER_SEGDIRTY
    if(ptSSD1306->pu8FrameBuffer && ptSSD1306->pu8FBSegDirty) {
#else  /* SSD1306_FRAMEBUFFER_SEGDIRTY */
    if(ptSSD1306->pu8FrameBuffer) {
#endif  /* SSD1306_FRAMEBUFFER_SEGDIRTY */
#endif  /* SSD1306_FRAMEBUFFER_STATIC */
      u16Loc = (uint16_t)u8Page * ptSSD1306->u8MaxX + u8Seg;
      if(ptSSD1306->pu8FrameBuffer[u16Loc] != u8Pattern) {
        ptSSD1306->pu8FrameBuffer[u16Loc] = u8Pattern;
        ptSSD1306->u8FBPageDirty |= (1 << u8Page);
#ifdef SSD1306_FRAMEBUFFER_SEGDIRTY
        ptSSD1306->pu8FBSegDirty[(uint16_t)u8Page * (ptSSD1306->u8MaxX >> 3) + (u8Seg >> 3)] |= 1 << (u8Seg & 0x7);
#endif  /* SSD1306_FRAMEBUFFER_SEGDIRTY */
      }
#ifndef SSD1306_FRAMEBUFFER_STATIC
    } else {
      ptSSD1306->tWriteCommand.u8Count = 4;
      ptSSD1306->tWriteCommand.au8Data[0] = 0xB0 | u8Page;  /* set page start address(B0～B7) */
      ptSSD1306->tWriteCommand.au8Data[1] = 0x21;  /* set Column Address */
      ptSSD1306->tWriteCommand.au8Data[2] = u8Seg;
      ptSSD1306->tWriteCommand.au8Data[3] = u8Seg;
      Lock(ptSSD1306);
      ptSSD1306->tWrite(ptSSD1306->pInstance, &ptSSD1306->tWriteCommand);
      Unlock(ptSSD1306);

      ptSSD1306->tWriteSeg.u8Count = 1;
      ptSSD1306->tWriteSeg.au8Data[0] = ptSSD1306->bInvert ? ~u8Pattern : u8Pattern;
      Lock(ptSSD1306);
      ptSSD1306->tWrite(ptSSD1306->pInstance, &ptSSD1306->tWriteSeg);
      Unlock(ptSSD1306);
    }
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

static void SSD1306_Refresh_sub(SSD1306_t* ptSSD1306, const uint8_t u8PreSeg) {
  if(ptSSD1306->tWriteSeg.u8Count > 0) {
    ptSSD1306->tWriteCommand.u8Count = 3;
    ptSSD1306->tWriteCommand.au8Data[0] = 0x21;  /* set Column Address */
#ifndef SSD1306_LCD_72X40
    ptSSD1306->tWriteCommand.au8Data[1] = u8PreSeg;
    ptSSD1306->tWriteCommand.au8Data[2] = u8PreSeg + (ptSSD1306->tWriteSeg.u8Count - 1);
#else  /* SSD1306_LCD_72X40 */
    ptSSD1306->tWriteCommand.au8Data[1] = u8PreSeg + 0x1C;
    ptSSD1306->tWriteCommand.au8Data[2] = u8PreSeg + 0x1C + (ptSSD1306->tWriteSeg.u8Count - 1);
#endif  /* SSD1306_LCD_72X40 */
    Lock(ptSSD1306);
    ptSSD1306->tWrite(ptSSD1306->pInstance, &ptSSD1306->tWriteCommand);
    ptSSD1306->tWrite(ptSSD1306->pInstance, &ptSSD1306->tWriteSeg);
    Unlock(ptSSD1306);
    ptSSD1306->tWriteSeg.u8Count = 0;
  }
}

bool SSD1306_Refresh(SSD1306_t* ptSSD1306) {
  bool bValid = false;
  uint8_t u8Page, u8Seg;
  uint8_t u8Count;
  uint8_t u8PreSeg;
#ifdef SSD1306_FRAMEBUFFER_SEGDIRTY
  uint16_t u16Loc;
#endif  /* SSD1306_FRAMEBUFFER_SEGDIRTY */

  if(ptSSD1306) {
    if(ptSSD1306->tWrite) {
      bValid = true;
#ifndef SSD1306_FRAMEBUFFER_STATIC
#ifdef SSD1306_FRAMEBUFFER_SEGDIRTY
      if(ptSSD1306->pu8FrameBuffer && ptSSD1306->pu8FBSegDirty) {
#else  /* SSD1306_FRAMEBUFFER_SEGDIRTY */
      if(ptSSD1306->pu8FrameBuffer) {
#endif  /* SSD1306_FRAMEBUFFER_SEGDIRTY */
#endif  /* SSD1306_FRAMEBUFFER_STATIC */
        ptSSD1306->tWriteSeg.u8Count = 0;
        ptSSD1306->tWriteSeg.bData = true;
        for(u8Page = 0; u8Page < ptSSD1306->u8MaxY / 8; u8Page++) {
          if(ptSSD1306->u8FBPageDirty & (1 << u8Page)) {
            ptSSD1306->u8FBPageDirty &= ~(1 << u8Page);
            ptSSD1306->tWriteCommand.u8Count = 3;
            ptSSD1306->tWriteCommand.au8Data[0] = 0x22;  /* set Row Address */
            ptSSD1306->tWriteCommand.au8Data[1] = u8Page;
            ptSSD1306->tWriteCommand.au8Data[2] = u8Page;
            Lock(ptSSD1306);
            ptSSD1306->tWrite(ptSSD1306->pInstance, &ptSSD1306->tWriteCommand);
            Unlock(ptSSD1306);
            u8PreSeg = 0xFF;
            for(u8Seg = 0; u8Seg < ptSSD1306->u8MaxX; u8Seg++) {
#ifdef SSD1306_FRAMEBUFFER_SEGDIRTY
              u16Loc = (uint16_t)u8Page * (ptSSD1306->u8MaxX >> 3) + (u8Seg >> 3);
              if(ptSSD1306->pu8FBSegDirty[u16Loc] == 0xFF) {
                ptSSD1306->pu8FBSegDirty[u16Loc] = 0;
                for(u8Count = 0; u8Count < 8; u8Count++) {
                  if(u8PreSeg == 0xFF) {
                    u8PreSeg = u8Seg + u8Count;
                  }
                  ptSSD1306->tWriteSeg.au8Data[ptSSD1306->tWriteSeg.u8Count] = ptSSD1306->pu8FrameBuffer[(uint16_t)u8Page * ptSSD1306->u8MaxX + u8Seg + u8Count];
                  ptSSD1306->tWriteSeg.u8Count++;
                  if(ptSSD1306->tWriteSeg.u8Count >= 30) {
                    SSD1306_Refresh_sub(ptSSD1306, u8PreSeg);
                    u8PreSeg = 0xFF;
                  }
                }
                u8Seg += 7;
              } else if(ptSSD1306->pu8FBSegDirty[u16Loc]) {
                if(ptSSD1306->pu8FBSegDirty[u16Loc] & (1 << (u8Seg & 0x7))) {
                  ptSSD1306->pu8FBSegDirty[u16Loc] &= ~(1 << (u8Seg & 0x7));
#endif  /* SSD1306_FRAMEBUFFER_SEGDIRTY */
                  if(u8PreSeg == 0xFF) {
                    u8PreSeg = u8Seg;
                  }
                  ptSSD1306->tWriteSeg.au8Data[ptSSD1306->tWriteSeg.u8Count] = ptSSD1306->pu8FrameBuffer[(uint16_t)u8Page * ptSSD1306->u8MaxX + u8Seg];
                  ptSSD1306->tWriteSeg.u8Count++;
                  if(ptSSD1306->tWriteSeg.u8Count >= 30) {
                    SSD1306_Refresh_sub(ptSSD1306, u8PreSeg);
                    u8PreSeg = 0xFF;
                  }
#ifdef SSD1306_FRAMEBUFFER_SEGDIRTY
                } else {
                  SSD1306_Refresh_sub(ptSSD1306, u8PreSeg);
                  u8PreSeg = 0xFF;
                }
              } else {
                SSD1306_Refresh_sub(ptSSD1306, u8PreSeg);
                u8PreSeg = 0xFF;
                u8Seg = (u8Seg & 0xF8) + 7;
              }
#endif  /* SSD1306_FRAMEBUFFER_SEGDIRTY */
            }
            SSD1306_Refresh_sub(ptSSD1306, u8PreSeg);
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
      ptSSD1306->tWriteCommand.u8Count = 9;
      ptSSD1306->tWriteCommand.au8Data[0] = 0x2E;         /* For configuration, once off the scroll. */
      if(bLeft) {
        ptSSD1306->tWriteCommand.au8Data[1] = 0x27;       /* Horizontal scroll set. 0x26=Right/0x27=Left direction. */
      } else {
        ptSSD1306->tWriteCommand.au8Data[1] = 0x26;       /* Horizontal scroll set. */
      }
      ptSSD1306->tWriteCommand.au8Data[2] = 0x00;         /* Dummy byte */
      ptSSD1306->tWriteCommand.au8Data[3] = u8StartPage;  /* Define start page address. */
      ptSSD1306->tWriteCommand.au8Data[4] = u8Intreval;   /* Set time interval pattern. */
      ptSSD1306->tWriteCommand.au8Data[5] = u8EndPage;    /* Define end page address. */
      ptSSD1306->tWriteCommand.au8Data[6] = 0x00;         /* Dummy byte */
      ptSSD1306->tWriteCommand.au8Data[7] = 0xFF;         /* Dummy byte */
      ptSSD1306->tWriteCommand.au8Data[8] = 0x2F;         /* Activate scroll */
      Lock(ptSSD1306);
      ptSSD1306->tWrite(ptSSD1306->pInstance, &ptSSD1306->tWriteCommand);
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
      ptSSD1306->tWriteCommand.u8Count = 8;
      ptSSD1306->tWriteCommand.au8Data[0] = 0x2E;         /* For configuration, once off the scroll. */
      if(bLeft) {
        ptSSD1306->tWriteCommand.au8Data[1] = 0x2A;       /* Horizontal scroll set. */
      } else {
        ptSSD1306->tWriteCommand.au8Data[1] = 0x29;       /* Horizontal scroll set. 0x29=V+Right/0x2A=V+Left direction. */
      }
      ptSSD1306->tWriteCommand.au8Data[2] = 0x00;         /* Dummy byte */
      ptSSD1306->tWriteCommand.au8Data[3] = u8StartPage;  /* Define start page address. */
      ptSSD1306->tWriteCommand.au8Data[4] = u8Intreval;   /* Set time interval pattern. */
      ptSSD1306->tWriteCommand.au8Data[5] = u8EndPage;    /* Define end page address. */
      ptSSD1306->tWriteCommand.au8Data[6] = u8Offset;     /* Vertical scrolling offset. */
      ptSSD1306->tWriteCommand.au8Data[7] = 0x2F;         /* Activate scroll */
      Lock(ptSSD1306);
      ptSSD1306->tWrite(ptSSD1306->pInstance, &ptSSD1306->tWriteCommand);
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
      ptSSD1306->tWriteCommand.u8Count = 1;
      ptSSD1306->tWriteCommand.au8Data[0] = 0x2E;  /* For configuration, once off the scroll. */
      Lock(ptSSD1306);
      ptSSD1306->tWrite(ptSSD1306->pInstance, &ptSSD1306->tWriteCommand);
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
        ptSSD1306->tWriteCommand.u8Count = 1;
        ptSSD1306->tWriteCommand.au8Data[0] = 0xAE;  /* Display Off */
        Lock(ptSSD1306);
        ptSSD1306->tWrite(ptSSD1306->pInstance, &ptSSD1306->tWriteCommand);
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
        ptSSD1306->tWriteCommand.u8Count = 3;
        ptSSD1306->tWriteCommand.au8Data[0] = 0x81;  /* Set Contrast Control 0x81, default=0x7F */
        ptSSD1306->tWriteCommand.au8Data[1] = ptSSD1306->u8Contrast;
        ptSSD1306->tWriteCommand.au8Data[2] = 0xAF;  /* Display On */
        Lock(ptSSD1306);
        ptSSD1306->tWrite(ptSSD1306->pInstance, &ptSSD1306->tWriteCommand);
        Unlock(ptSSD1306);
        ptSSD1306->bSleep = false;
      }
    }
  }

  return bValid;
}

