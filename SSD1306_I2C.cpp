/* Library of OLED modules connect with SSD1306(I2C bus) by AZO */

#include <string.h>
#include <Arduino.h>
#include <Wire.h>
#include "SSD1306_I2C.h"

void SSD1306_I2C::Initialize(const unsigned char ucI2CAddress, const unsigned int uiMaxX, const unsigned int uiMaxY, const unsigned char ucBeginI2C) {
#ifdef SSD1306_I2C_ENABLE_FRAMEBUFFER
	this->pucFrameBuffer = new unsigned char[this->uiMaxY * this->uiMaxX / 8];
	memset(this->pucFrameBuffer, 0, this->uiMaxY * this->uiMaxX / 8);
#endif	/* SSD1306_I2C_ENABLE_FRAMEBUFFER */
	this->uiMaxX = uiMaxX;
	this->uiMaxY = uiMaxY;
	this->ucSleep = 0;
	this->ucReverse = 0;
	this->ucI2CAddress = ucI2CAddress >> 1;

	if(ucBeginI2C) {
		Wire.begin();
		Wire.setClock(400000);
		delay(100);
	}
 
	Wire.beginTransmission(this->ucI2CAddress);
		Wire.write(0b00000000); //control byte, Co bit = 0 (continue), D/C# = 0 (command) Max=31byte
			Wire.write(0xAE); //display off
		Wire.write(0b00000000);
			Wire.write(0xA8); //Set Multiplex Ratio  0xA8, 0x3F
				Wire.write(0b00111111); //64MUX
		Wire.write(0b00000000);    
			Wire.write(0xD3); //Set Display Offset 0xD3, 0x00
				Wire.write(0x00);
		Wire.write(0b00000000);
			Wire.write(0x40); //Set Display Start Line 0x40
		Wire.write(0b00000000);
			Wire.write(0xA1); //Set Segment re-map 0xA0/0xA1
		Wire.write(0b00000000);
			Wire.write(0xC8); //Set COM Output Scan Direction 0xC0,/0xC8
		Wire.write(0b00000000);
			Wire.write(0xDA); //Set COM Pins hardware configuration 0xDA, 0x02
				Wire.write(0b00010010);
		Wire.write(0b00000000);
			Wire.write(0x81); //Set Contrast Control 0x81, default=0x7F
				Wire.write(255); //0-255
		Wire.write(0b00000000);
			Wire.write(0xA4); //Disable Entire Display On
		Wire.write(0b00000000);
			Wire.write(0xA6); //Set Normal Display 0xA6, Inverse display 0xA7
		Wire.write(0b00000000);
 			Wire.write(0xD5); //Set Display Clock Divide Ratio/Oscillator Frequency 0xD5, 0x80
				Wire.write(0b10000000);
		Wire.write(0b00000000);
			Wire.write(0x20); //Set Memory Addressing Mode
				Wire.write(0x10); //Page addressing mode
	Wire.endTransmission();
	Wire.beginTransmission(this->ucI2CAddress);
		Wire.write(0b00000000);
			Wire.write(0x22); //Set Page Address
				Wire.write(0); //Start page set
				Wire.write(this->uiMaxY / 8 - 1); //End page set
		Wire.write(0b00000000);
			Wire.write(0x21); //set Column Address
				Wire.write(0); //Column Start Address
				Wire.write(this->uiMaxX - 1); //Column Stop Address
		Wire.write(0b00000000);
			Wire.write(0x8D); //Set Enable charge pump regulator 0x8D, 0x14
				Wire.write(0x14);
		Wire.write(0b00000000);
			Wire.write(0xAF); //Display On 0xAF
	Wire.endTransmission();
}

void SSD1306_I2C::Clear(void) {
#ifdef SSD1306_I2C_ENABLE_FRAMEBUFFER
	memset(this->pucFrameBuffer, 0, this->uiMaxY * this->uiMaxX / 8);
#else	/* SSD1306_I2C_ENABLE_FRAMEBUFFER */
	unsigned int uiSeg, uiX, uiColumn, uiCount;

	for(uiSeg = 0; uiSeg < this->uiMaxY / 8; uiSeg++) {
		Wire.beginTransmission(this->ucI2CAddress);
			Wire.write(0b00000000); //control byte, Co bit = 1 (One command only), D/C# = 0 (command)
				Wire.write(0xB0 | uiSeg); //set page start address(B0～B7)
			Wire.write(0b00000000);
				Wire.write(0x21); //set Column Address
					Wire.write(0); //Column Start Address(0-127)
					Wire.write(this->uiMaxX - 1); //Column Stop Address(0-127)
		Wire.endTransmission();

		for(uiColumn = 0; uiColumn < this->uiMaxX / 16; uiColumn++) { //column = 8byte x 16
			Wire.beginTransmission(this->ucI2CAddress);
				Wire.write(0b01000000); //control byte, Co bit = 0 (continue), D/C# = 1 (data)
				for(uiCount = 0; uiCount < 16; uiCount++) { //continue to 31byte
					if(ucReverse) {
						Wire.write(0xFF);
					} else {
						Wire.write(0);
					}
				}
			Wire.endTransmission();
		}
	}
#endif	/* SSD1306_I2C_ENABLE_FRAMEBUFFER */
}

unsigned int SSD1306_I2C::GetPixel(unsigned int uiX, unsigned int uiY) {
	unsigned char ucPattern;

	if(uiX >= this->uiMaxX || uiY >= this->uiMaxY) {
		return 0;
	}

	ucPattern = this->GetSeg(uiX, uiY / 8);
	return (ucPattern >> (uiY % 8)) & 0x1;
}

unsigned char SSD1306_I2C::GetSeg(unsigned int uiX, unsigned int uiSeg) {
	if(uiX >= this->uiMaxX || uiSeg >= this->uiMaxY / 8) {
		return 0;
	}

#ifdef SSD1306_I2C_ENABLE_FRAMEBUFFER
	return this->pucFrameBuffer[uiSeg * this->uiMaxX + uiX];
#else	/* SSD1306_I2C_ENABLE_FRAMEBUFFER */
	unsigned char ucPattern;

	Wire.beginTransmission(this->ucI2CAddress);
		Wire.write(0b00000000); //control byte, Co bit = 1 (One command only), D/C# = 0 (command)
			Wire.write(0xB0 | uiSeg); //set page start address(B0～B7)
		Wire.write(0b00000000);
			Wire.write(0x21); //set Column Address
				Wire.write(uiX); //Column Start Address
				Wire.write(uiX); //Column Stop Address
	Wire.endTransmission();
	Wire.beginTransmission(this->ucI2CAddress);
		Wire.write(0b01000000); //control byte, Co bit = 0 (continue), D/C# = 1 (data)
	Wire.endTransmission();
	Wire.requestFrom((int)this->ucI2CAddress, (int)1);
	while(Wire.available()) {
		ucPattern = Wire.read();
	}

	return ucPattern;
#endif	/* SSD1306_I2C_ENABLE_FRAMEBUFFER */
}

void SSD1306_I2C::SetSeg(unsigned int uiX, unsigned int uiSeg, unsigned char ucPattern) {
	if(uiX >= this->uiMaxX || uiSeg >= this->uiMaxY / 8) {
		return;
	}

#ifdef SSD1306_I2C_ENABLE_FRAMEBUFFER
	this->pucFrameBuffer[uiSeg * this->uiMaxX + uiX] = ucPattern;
#else	/* SSD1306_I2C_ENABLE_FRAMEBUFFER */
	Wire.beginTransmission(this->ucI2CAddress);
		Wire.write(0b00000000); //control byte, Co bit = 1 (One command only), D/C# = 0 (command)
			Wire.write(0xB0 | uiSeg); //set page start address(B0～B7)
		Wire.write(0b00000000);
			Wire.write(0x21); //set Column Address
				Wire.write(uiX); //Column Start Address
				Wire.write(uiX); //Column Stop Address
	Wire.endTransmission();
	Wire.beginTransmission(this->ucI2CAddress);
		Wire.write(0b01000000); //control byte, Co bit = 0 (continue), D/C# = 1 (data)
			if(this->ucReverse) {
				Wire.write(~ucPattern);
			} else {
				Wire.write( ucPattern);
			}
	Wire.endTransmission();
#endif	/* SSD1306_I2C_ENABLE_FRAMEBUFFER */
}

void SSD1306_I2C::SetPixel(const unsigned int uiX, const unsigned int uiY, const unsigned int uiColor) {
	unsigned char ucPattern;

	if(uiX >= this->uiMaxX || uiY >= this->uiMaxY) {
		return;
	}

	ucPattern = this->GetSeg(uiX, uiY / 8);
	if(this->ucReverse) {
		if(uiColor) {
			ucPattern &= ~(1 << (uiY % 8));
		} else {
			ucPattern |=   1 << (uiY % 8) ;
		}
	} else {
		if(uiColor) {
			ucPattern |=   1 << (uiY % 8) ;
		} else {
			ucPattern &= ~(1 << (uiY % 8));
		}
	}
	this->SetSeg(uiX, uiY / 8, ucPattern);
}

unsigned char SSD1306_I2C::IsReverse(void) {
	return this->ucReverse;
}

void SSD1306_I2C::SetReverse(const unsigned char ucEnable) {
	if(ucEnable) {
		this->ucReverse = 1;
	} else {
		this->ucReverse = 0;
	}
}

#ifdef SSD1306_I2C_ENABLE_FRAMEBUFFER
void SSD1306_I2C::Refresh(void) {
	unsigned int uiSeg, uiX, uiColumn, uiCount;

	for(uiSeg = 0; uiSeg < this->uiMaxY / 8; uiSeg++) {
		Wire.beginTransmission(this->ucI2CAddress);
			Wire.write(0b00000000); //control byte, Co bit = 1 (One command only), D/C# = 0 (command)
				Wire.write(0xB0 | uiSeg); //set page start address(B0～B7)
			Wire.write(0b00000000);
				Wire.write(0x21); //set Column Address
					Wire.write(0); //Column Start Address(0-127)
					Wire.write(this->uiMaxX - 1); //Column Stop Address(0-127)
		Wire.endTransmission();

		for(uiColumn = 0; uiColumn < this->uiMaxX / 16; uiColumn++) { //column = 8byte x 16
			Wire.beginTransmission(this->ucI2CAddress);
				Wire.write(0b01000000); //control byte, Co bit = 0 (continue), D/C# = 1 (data)
				for(uiCount = 0; uiCount < 16; uiCount++) { //continue to 31byte
					if(ucReverse) {
						Wire.write(~this->pucFrameBuffer[uiSeg * this->uiMaxX + uiColumn * 16 + uiCount]);
					} else {
						Wire.write( this->pucFrameBuffer[uiSeg * this->uiMaxX + uiColumn * 16 + uiCount]);
					}
				}
			Wire.endTransmission();
		}
	}
}
#endif	/* SSD1306_I2C_ENABLE_FRAMEBUFFER */

unsigned char SSD1306_I2C::IsSleep(void) {
	return this->ucSleep;
}

void SSD1306_I2C::Sleep(void) {
	if(this->IsSleep()) {
		return;
	}

	Wire.beginTransmission(this->ucI2CAddress);
		Wire.write(0b00000000); //control byte, Co bit = 1 (One command only), D/C# = 0 (command)
			Wire.write(0xAE); //Display Off 0xAE
	Wire.endTransmission();

	this->ucSleep = 1;
}

void SSD1306_I2C::Wakeup(void) {
	if(!this->IsSleep()) {
		return;
	}

	Wire.beginTransmission(this->ucI2CAddress);
		Wire.write(0b00000000);
			Wire.write(0x81); //Set Contrast Control 0x81, default=0x7F
				Wire.write(255); //0-255
		Wire.write(0b00000000); //control byte, Co bit = 1 (One command only), D/C# = 0 (command)
			Wire.write(0xAF); //Display On 0xAF
	Wire.endTransmission();

	this->ucSleep = 0;
}

