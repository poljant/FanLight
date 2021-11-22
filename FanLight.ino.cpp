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


#include <Arduino.h>
#include <ESP8266WiFi.h>
//#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
//#include <functional>
#include <Relay.h>	//https:/github.com/poljant/relay

#include "secrets.h"
#include "FanLight.h"
#include <fauxmoESP.h>
//#include <ESPAsyncWebServer.h>
#ifdef BUTTON2
#include <Button2.h>
#endif
#include "debug.h"
#include <WiFiManager.h>

String hostname();

Relay device1;
Relay device2;
fauxmoESP fauxmo;
//AsyncWebServer server(80);
//extern unsigned long fminutes(unsigned int);
unsigned long aptime;
unsigned int apminutes = 10;
#define BUTTON_PIN_DEV1  12
#define BUTTON_PIN_DEV2  14
#define RELAY_PIN_DEV1 5
#define RELAY_PIN_DEV2 4
#define LED_PIN LED_BUILTIN
#define ID_DEV1 "FAN"
#define ID_DEV2 "LAMP"

String version = VERSION;
bool eraseSetWiFi = false;

#ifdef IP_STATIC
IPAddress IPadr(10,110,1,105); //stały IP
IPAddress netmask(255,255,0,0);
IPAddress gateway(10,110,0,1);
//////////////////////////////
#endif

//unsigned int ilM=10;
unsigned long fminutes(unsigned int ile) {
	return (millis() + (1000 * 60 * ile));
}
//format bytes
String formatBytes(size_t bytes) {
	if (bytes < 1024) {
		return String(bytes) + "B";
	} else if (bytes < (1024 * 1024)) {
		return String(bytes / 1024.0) + "KB";
	} else if (bytes < (1024 * 1024 * 1024)) {
		return String(bytes / 1024.0 / 1024.0) + "MB";
	} else {
		return String(bytes / 1024.0 / 1024.0 / 1024.0) + "GB";
	};
};

//dane dla AP
const char* ap_ssid = hostname().c_str();   // SSID AP
const char* ap_pass = "12345678";  // password do AP
int ap_channel = 7; //numer kanału dla AP


///////////////////////////////////////////
String hostname(void) {
	uint32_t chipId = ESP.getChipId();
	char uid[20];
	sprintf_P(uid, PSTR("%s_%02x%02x%02x"), HOSTNAME,
			(uint16_t) ((chipId >> 16) & 0xff),
			(uint16_t) ((chipId >> 8) & 0xff), ((uint16_t) chipId & 0xff));
	//	Serial.println(uid);
	return String(uid);
}
void setwifi(bool erase){
#include <WiFiManager.h>
	WiFiManager wifiManager;
	if(erase){
		wifiManager.resetSettings();

	}
#ifdef IP_STATIC
	wifiManager.setSTAStaticIPConfig(IPadr,gateway,netmask);
	WiFi.config(IPadr,gateway,netmask);  // stały IP
#endif
	wifiManager.autoConnect(hostname().c_str());
	delay(100);
}
/*
void callback(unsigned char device_id, const char * device_name, bool state, unsigned char value) {
			Serial.printf("[MAIN] Device #%d (%s) state: %s value: %d\n", device_id, device_name, state ? "ON" : "OFF", value);
			if ( (strcmp(device_name, "device1") == 0) )
			{
				if (state)  {
					device1.setOn();
				} else {
					device1.setOff();
				}
			}
			if ( (strcmp(device_name, "LAMP") == 0) )
			{
				if (state)  {
					device2.setOn();
				} else {
					device2.setOff();
				}
			}
		}
*/


#ifdef BUTTON2
Button2 buttondevice1;
Button2 buttondevice2;

void clickdev1(Button2& btn){
	if (!device1.read()){
	DEBUG_MSG_PROG("[CLICK] %s On\n\r", ID_DEV1);
	device1.setOn();
	fauxmo.setState(ID_DEV1,true, 254);
	}else{
	DEBUG_MSG_PROG("[CLICK] %s Off\n\r", ID_DEV1);
	device1.setOff();
	fauxmo.setState(ID_DEV1,false, 0);
	}
}
void clickdev2(Button2& btn){
	if (!device2.read()){
	DEBUG_MSG_PROG("[CLICK] %s On\n\r", ID_DEV2);
//	Serial.printf("%s On\n\r", ID_DEV2);
	device2.setOn();
	fauxmo.setState(ID_DEV2,true, 254);
	}else{
	DEBUG_MSG_PROG("[CLICK] %s Off\n\r", ID_DEV2);
//	Serial.printf("%s Off\n\r", ID_DEV2);
	device2.setOff();
	fauxmo.setState(ID_DEV2,false, 0);
	}
}
void longclickdev1(Button2& btn){
	if(buttondevice1.wasPressedFor()<8000){
		DEBUG_MSG_PROG("[LONG CLICK] %s Reset ESP\n\r",ID_DEV1);
		delay(100);
		ESP.reset();
	}else{
		DEBUG_MSG_PROG("[LONG LONG CLICK] %s Reset WiFi\n\r",ID_DEV1);
		delay(100);
		setwifi(true);
	}

}
void longclickdev2(Button2& btn){
	if(buttondevice2.wasPressedFor()<8000){
		DEBUG_MSG_PROG("[LONG CLICK] %s Reset ESP\n\r",ID_DEV2);
		delay(100);
		ESP.reset();
	}else{
		DEBUG_MSG_PROG("[LONG LONG CLICK] %s Reset WiFi\n\r",ID_DEV2);
		setwifi(true);
	}
}
#endif
/*void callback(uint8_t device_id, const char * device_name, bool state)
{
  Serial.print("Device "); Serial.print(device_name);
  Serial.print(" state: ");
  if (state)
  {
    Serial.println("ON");
  }
  else
  {
    Serial.println("OFF");
  }
}*/
/*
void serverSetup() {
    // Custom entry point (not required by the library, here just as an example)
    server.on("/index.html", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/plain", "Hello, world");
    });

    // These two callbacks are required for gen1 and gen3 compatibility
    server.onRequestBody([](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
        if (fauxmo.process(request->client(), request->method() == HTTP_GET, request->url(), String((char *)data))) return;
        // Handle any other body request here...
    });
    server.onNotFound([](AsyncWebServerRequest *request) {
        String body = (request->hasParam("body", true)) ? request->getParam("body", true)->value() : String();
        if (fauxmo.process(request->client(), request->method() == HTTP_GET, request->url(), body)) return;
        // Handle not found request here...
    });

    // Start the server
    server.begin();

}*/


void setup() {
	WiFi.mode(WIFI_STA);
	Serial.begin(115200);
	//	Serial.setDebugOutput(true);
	device1.begin(RELAY_PIN_DEV1);
	device2.begin(RELAY_PIN_DEV2);

#ifdef BUTTON2
	buttondevice1.begin(BUTTON_PIN_DEV1);
	buttondevice1.setLongClickTime(4000);
	buttondevice1.setClickHandler(clickdev1);
	buttondevice1.setLongClickHandler(longclickdev1);

	buttondevice2.begin(BUTTON_PIN_DEV2);
	buttondevice2.setLongClickTime(4000);
	buttondevice2.setClickHandler(clickdev2);
	buttondevice2.setLongClickHandler(longclickdev2);
#endif
	setwifi(eraseSetWiFi);

	fauxmo.createServer(true);
	fauxmo.setPort(80);
	fauxmo.enable(true);
	fauxmo.addDevice(ID_DEV1);
	fauxmo.addDevice(ID_DEV2);
//	fauxmo.onSetState([]callback(device_id,  device_name,  state,  value));
	fauxmo.onSetState([](unsigned char device_id, const char * device_name, bool state, unsigned char value) {
		DEBUG_MSG_PROG("[MAIN] Device #%d (%s) state: %s value: %d\n\r", device_id, device_name, state ? "ON" : "OFF", value);
		if ( (strcmp(device_name, ID_DEV1) == 0) )
		{
			if (state)  {
				device1.setOn();
			} else {
				device1.setOff();
			}
		}
		if ( (strcmp(device_name, ID_DEV2) == 0) )
		{
			if (state)  {
				device2.setOn();
			} else {
				device2.setOff();
			}
		}
	});
	MDNS.begin(hostname(), WiFi.localIP());
	Serial.println();
	Serial.println(hostname());
	Serial.println(WiFi.localIP());
	Serial.println(WiFi.macAddress());
	aptime = fminutes(apminutes);
}

void loop() {

#ifdef BUTTON2
	buttondevice1.loop();
	buttondevice2.loop();
#endif
	if (WiFi.status() != WL_CONNECTED) {
		WiFi.mode(WIFI_AP_STA); //tryb AP+STATION
		aptime = fminutes(apminutes);  // licz czas trwania AP po połaczeniu
	} else {
		if (aptime <= millis()){  // gdy minął czas trwania AP przełącz na STATION
			WiFi.mode(WIFI_STA); //tryb STATION
		}
	}

	fauxmo.handle();
	MDNS.update();
}
