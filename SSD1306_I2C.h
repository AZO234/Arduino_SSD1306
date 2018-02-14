#ifndef SSD1306_I2C_H
#define SSD1306_I2C_H

/* SSD1306 I2C Driver for Arduino by AZO */

/* Don't disable until finding read GDDRAM via I2C method ... */
#define SSD1306_I2C_ENABLE_FRAMEBUFFER

class SSD1306_I2C {
	public:
		/* MaxX and MaxY must be multiples of 8 */
		void Initialize(const unsigned char ucI2CAddress, const unsigned int uiMaxX, const unsigned int uiMaxY, const unsigned char ucBeginI2C);
		unsigned int GetMaxX(void);
		unsigned int GetMaxY(void);
		void Clear(void);

		unsigned int GetPixel(const unsigned int uiX, const unsigned int uiY);
		unsigned char GetSeg(const unsigned int uiX, const unsigned int uiSegNo);
		void SetSeg(const unsigned int uiX, const unsigned int uiSegNo, const unsigned char ucPattern);
		void SetPixel(const unsigned int uiX, const unsigned int uiY, const unsigned int uiColor);

		unsigned char IsReverse(void);
		void SetReverse(const unsigned char ucEnable);
#ifdef SSD1306_I2C_ENABLE_FRAMEBUFFER
		void Refresh(void);
#endif	/* SSD1306_I2C_ENABLE_FRAMEBUFFER */

		unsigned char IsSleep(void);
		void Sleep(void);
		void Wakeup(void);

	private:
		unsigned int uiMaxX;
		unsigned int uiMaxY;
		unsigned char ucReverse;
		unsigned char ucSleep;
		unsigned char ucI2CAddress;
#ifdef SSD1306_I2C_ENABLE_FRAMEBUFFER
		unsigned char* pucFrameBuffer;
#endif	/* SSD1306_I2C_ENABLE_FRAMEBUFFER */
};

#endif	/* SSD1306_I2C_H */

