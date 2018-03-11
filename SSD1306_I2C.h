#ifndef SSD1306_I2C_H
#define SSD1306_I2C_H

/* SSD1306 I2C Driver for Arduino IDE by AZO */

class SSD1306_I2C {
	public:
		/* MaxX and MaxY must be multiples of 8 */
		SSD1306_I2C(const unsigned char ucI2CAddress, const unsigned int uiMaxX, const unsigned int uiMaxY);

		void Initialize(const unsigned char ucBeginI2C);
		unsigned int GetMaxX(void);
		unsigned int GetMaxY(void);
		void Clear(void);

		unsigned int GetPixel(const unsigned int uiX, const unsigned int uiY);
		unsigned char GetSeg(const unsigned int uiX, const unsigned int uiSegNo);
		void SetSeg(const unsigned int uiX, const unsigned int uiSegNo, const unsigned char ucPattern);
		void DrawPixel(const unsigned int uiX, const unsigned int uiY, const unsigned int uiColor);

		unsigned char IsReverse(void);
		void SetReverse(const unsigned char ucEnable);
		void Refresh(void);

		void HScroll(const ucReverse, const ucStartPage, const ucIntreval, const ucEndPage);
		void VHScroll(const ucReverse, const ucStartPage, const ucIntreval, const ucEndPage, const ucOffset);
		void ScrollOff(void);

		unsigned char IsSleep(void);
		void Sleep(void);
		void Wakeup(void);

	private:
		unsigned int uiMaxX;
		unsigned int uiMaxY;
		unsigned char ucReverse;
		unsigned char ucSleep;
		unsigned char ucI2CAddress;
		unsigned char* pucFrameBuffer;
};

#endif	/* SSD1306_I2C_H */

