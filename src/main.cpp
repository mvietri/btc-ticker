using namespace std;
#include "Free_Fonts.h"
#include <WiFi.h>
#include <M5Stack.h>
#include <string>
#include <math.h>
#include <vector>
#include <string>
#include "ArduinoJson.h"
#include <HTTPClient.h>
 
HTTPClient client;
int status = WL_IDLE_STATUS;     

char ssid[]     = "<WiFi SSID>";
char password[] = "<WiFI Password>"; 

void setup() {
  M5.begin();
  M5.lcd.setBrightness(25);
  M5.Lcd.setTextDatum(MC_DATUM);

  Serial.begin(9600);
  delay(100);
  
  WiFi.disconnect();
  M5.Lcd.println("Connecting to ");
  M5.Lcd.println(ssid);
  delay(500);

  WiFi.begin(ssid, password);

  wl_status_t wifi_Status = WiFi.status();  
  int n_trials = 0;

  while (wifi_Status != WL_CONNECTED && n_trials < 5) {
    wifi_Status = WiFi.status();
    n_trials++;

    switch(wifi_Status){
      case WL_NO_SSID_AVAIL:
          M5.Lcd.println("[ERR] SSID not available");
          break;
      case WL_CONNECT_FAILED:
          M5.Lcd.println("[ERR] Connection failed");
          break;
      case WL_CONNECTION_LOST:
          M5.Lcd.println("[ERR] Connection lost");
          break;
      case WL_DISCONNECTED:
          M5.Lcd.println("[ERR] WiFi disconnected");
          break;
      case WL_IDLE_STATUS:
          M5.Lcd.println("[ERR] WiFi idle status");
          break;
      case WL_SCAN_COMPLETED:
          M5.Lcd.println("[OK] WiFi scan completed");
          break;
      case WL_CONNECTED:
          M5.Lcd.println("[OK] WiFi connected");
          break;
      default:
          M5.Lcd.println("[ERR] unknown Status");
          break;
    }
    delay(500);
  }

  if(wifi_Status == WL_CONNECTED){
    M5.Lcd.print("IP address: ");
    M5.Lcd.println(WiFi.localIP()); 
    
    M5.update();

    client.begin("https://api.coindesk.com/v1/bpi/currentprice.json");
    int httpCode = client.GET();

    if (httpCode > 0) {
        String payload = client.getString();

        char json[500];
        payload.replace(" ", "");
        payload.replace("\n", "");
        payload.trim();
        //payload.remove(0,1);
        payload.toCharArray(json, 500);

        StaticJsonDocument<200> doc;
        deserializeJson(doc, json, DeserializationOption::NestingLimit(15));

        const char* chartName = doc["chartName"];
        const char* price = doc["bpi"]["USD"]["rate"];

        M5.Lcd.clear();

        M5.Lcd.setTextColor(YELLOW);
        M5.Lcd.setTextSize(2);
        M5.Lcd.println(String(chartName));

        M5.Lcd.setTextColor(GREEN);
        M5.Lcd.setTextSize(4);
        M5.Lcd.println(String(price));
         
    } else {  
      M5.Lcd.println("");
      M5.Lcd.println("[ERR] unable to get data"); 
    }

  } else {
    M5.Lcd.println("");
    M5.Lcd.println("[ERR] unable to connect Wifi"); 
  }
} 

void loop() { }