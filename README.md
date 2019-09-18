Library of OLED modules connect with SSD1306(I2C/SPI bus) by AZO
================================================================
version: v1.0.1(Sep 19, 2019)  
http://domisan.sakura.ne.jp/

Library of OLED modules connect with SSD1306(I2C/SPI bus).

Connect Arduino UNO as follow.

[I2C]
3.3V - VDD  
 GND - GND  
  A5 - SCK  
  A4 - SDA

[SPI]
3.3V - VDD  
 GND - GND  
  13 - SCK  
  11 - SDA  
   9 - RES  
   8 - DC  
  10 - CS

First contrast instance of class SSD1306_I2C or SSD1306_SPI.  
Next call initialize(); and initDevice();  
Then draw with drawPixel(); to framebuffer.  
Finally call refresh(); to display from framebuffer.

Reference
---------
- SSD1306 Advance Information  
https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf
- 有機EL ( OLED ) SSD1306 を再検証してみました ( I2C 通信用 )  
https://www.mgo-tec.com/blog-entry-ssd1306-revalidation-i2c-esp32-esp8266.html

