/*Based off Arduino ESP8266 OLED 0.9 SSD1306 WiFi Scanner 
 * v 1.1 by Robert Ulbricht
 * LeRoy Miller, 2017
 * 
 * Changes - replaced u8g2 library with ESP8266 SSD1306 library by squix78
 * https://github.com/squix78/esp8266-oled-ssd1306
 * 
 * Board change to D-Duino with on board I2C OLED (128x64 display)
 * modified line setting display.
 * 
 * Special thanks to Robert Ulbricht.
 * 
 
   Arduino ESP8266 OLED 0.9 SSD1306 WiFi Scanner
   v. 1.1
   Copyright (C) 2017 Robert Ulbricht
   http://www.arduinoslovakia.eu
   WiFi Scanner.
   IDE: 1.8.2 or higher
   Board: NodeMCU 0.9 (ESP-12)
   Libraries:
   display: https://github.com/olikraus/display
   Version: 2.15.2 or higher
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "ESP8266WiFi.h"
#include <Wire.h>
#include "src/esp8266_ssd1306/SSD1306.h"

SSD1306  display(0x3c, 5 /*D1*/, 4 /*D2*/);

char draw[30];
char ssid[30];
char mac[30];

#define LINE0 3
#define LINE1 13
#define LINE2 23
#define LINE4 33
#define LINE5 43

void setup() {
  display.init();
  display.setFont(ArialMT_Plain_10);

  display.clear();
  display.drawString(0, 3, "WiFi Scanner");
  display.drawString(0, 13, "Based on sketch by");
  display.drawString(0, 23, "Robert Ulbricht, 2017");
  display.drawString(0, 33, "by LeRoy Miller 2017");
  display.display();

  //Serial.begin(115200);

  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(1000);

  Serial.println("Setup done");
}

void messageBox(const char *message)
{
  Serial.println(message);
  display.clear();
  display.drawString(0, 5, message);
  display.display();
}

void loop() {
  messageBox("scan start");

  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks(false,true); // false - async, true - hidden networks
  messageBox("scan done");
  if (n == 0)
    messageBox("no networks found");
  else
  {
    sprintf(draw, "%d netw. found", n);
    messageBox(draw);
    for (int i = 0; i < n; ++i)
    {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(") ");
      Serial.println(getEncryptionType(WiFi.encryptionType(i)));

      // oled
      strcpy(ssid,WiFi.SSID(i).c_str());
      int dl=strlen(ssid);
      if(dl>2) { // hide some details
        ssid[dl-2]='?';
        ssid[dl-1]='?';
      }
      strcpy(mac,WiFi.BSSIDstr(i).c_str());
      dl=strlen(mac);
      if(dl>2) { // hide some details
        mac[dl-2]='?';
        mac[dl-1]='?';
      }
      display.clear();
      sprintf(draw,"Net: %d/%d",i+1,n);
      display.drawString(1, LINE0, draw);
      display.drawString(1, LINE1, ssid);
      display.drawString(1, LINE2, mac);
      sprintf(draw,"Sig: %d dBm",WiFi.RSSI(i));
      display.drawString(1, LINE4, draw);
      sprintf(draw,"Ch: %d",WiFi.channel(i));
      display.drawString(80, LINE4, draw);
      sprintf(draw,"Enc: %s",getEncryptionType(WiFi.encryptionType(i)));
      display.drawString(1, LINE5, draw);
      display.drawRect(0,0,128,64);
      display.display();
      
      delay(2000);
    }
  }
  Serial.println("");
}

const char S_WEP[] = "WEP";
const char S_TKIP[] = "WPA";
const char S_CCMP[] = "WPA2";
const char S_NONE[] = "None";
const char S_AUTO[] = "Auto";
const char S_UNKNOWN[] = "Unkn.";

const char* getEncryptionType(int thisType) {
  // read the encryption type and print out the name:
  switch (thisType) {
    case ENC_TYPE_WEP:
      return S_WEP;
    case ENC_TYPE_TKIP:
      return S_TKIP;
    case ENC_TYPE_CCMP:
      return S_CCMP;
    case ENC_TYPE_NONE:
      return S_NONE;
    case ENC_TYPE_AUTO:
      return S_AUTO;
    default:
      return S_UNKNOWN;
  }
}
