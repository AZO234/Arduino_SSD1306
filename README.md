Library of OLED modules connect with SSD1306(I2C bus) by AZO
============================================================
version: v1.0.0(Feb 12, 2018)  
http://domisan.sakura.ne.jp/

Library of OLED modules connect with SSD1306(I2C bus).

First constrast instance of class SSD1306_I2C.  
Next call Initialize(I2CAddressOfOLED, MaxX, MaxY, 1);  
Then draw with SetPixel(); to framebuffer.  
Finally call Refresh(); to display from framebuffer.

Reference
---------
- 有機EL ( OLED ) SSD1306 を再検証してみました ( I2C 通信用 )  
https://www.mgo-tec.com/blog-entry-ssd1306-revalidation-i2c-esp32-esp8266.html

