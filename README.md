# FanLight
/*
 * FanLight.ino
 *
 *  Created on: 4 maj 2021
 *      Author: jant
  Urządzenie te umożliwia sterowanie wentylatorem podsufitowym z lampą.
  Posiada dwa oddzielne przekaźniki załączania wentylatora i lampy.
  Sterowanie możliwe jest za pomocą dwóch przycisków i głosowo za pomocą Alexy.
  Jedno urządzenie jest ustawione jako FAN, a drugie jako LAMP.
  Te nazwy można zmienić w lini 44 i 45 jako ID_DEV1 i ID_DEV2.
  Przy pierwszym uruchomieniu działa program do konfiguracji wifi (WiFiManager).
  wiecej informacji https://github.com/tzapu/WiFiManager.git
  Ten program umożliwia przy pierwszym uruchomieniu:
  wybranie i ustawienie parametrów sieci WiFi,
  wyswietlenie informacji o naszym układzie esp8266 lub esp32,
  wykonanie upgrad firmware naszego modułu przez wifi,
  Przyciski załaczające oba urządzenia mają dodatkowe funkcje;
  - każdy przyciska załacza i wyłacza dane urzadzeni
  - przytrzymanie dowolnego przycisku przez 4 sec. uruchamia reset ESP,
  - przytrzymanie dowolnego przycisku przez10 sec. uruchamia resets ustawień WiFi
    i przy kolejny uruchomieniu pracuje jako AP,wtedy należy wybrać ponownie dane wifi.
    Ta opcja umożliwia wgranie nowego firmware przez www.
 */
/*
A device that allows you to control a ceiling fan with a lamp.
It has two devices with a relay for switching the oil and the lamp.
Control is possible with two and by voice using Alexa. One device is as FAN, the other is LAMP.
These names can be changed on lines 44 and 45 as ID_DEV1 and ID_DEV2.
The configuration configuration program is running on the first WiFi.
more information https: //github.com/tzapu/WiFiManager.git
This program enables the first time you run:
- selecting and setting WiFi network parameters,
- display information about our esp8266 or esp32 chip,
- upgrade of the firmware of our module via wifi,
- Buttons that activate both devices have additional functions;
- each press turns the device on and off,
- lasts longer than 4 sec. device reset
- periodic (permanent) holding for more than 10 sec.
  Configure the WiFi settings reset and use the AP as the next one and select the wifi data again.
  This option allows you to upload new software via www.
*/

Opis płyty PCB: https://github.com/poljant/FanLightKiCAD.git
