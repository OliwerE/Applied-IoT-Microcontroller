#include <WiFi.h>
#include <HTTPClient.h>
#include "ArduinoJson.h"
#include "DHT.h"
#include "SparkFun_SGP40_Arduino_Library.h"
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BMP280.h>

#include "./env.h"

Adafruit_BMP280 bmp;
#define DHTTYPE DHT11
#define DHTPIN 14

DHT dht(DHTPIN, DHTTYPE);
SGP40 sgp40;

int timer = postInterval; // timer to post data (seconds)

void setup() {
  Serial.begin(115200);

  startWiFi();

  dht.begin();
  
  Wire.begin();

  unsigned status;
  status = bmp.begin(0x76);
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,
                  Adafruit_BMP280::SAMPLING_X2,
                  Adafruit_BMP280::SAMPLING_X16,
                  Adafruit_BMP280::FILTER_X16,
                  Adafruit_BMP280::STANDBY_MS_500);

  if (sgp40.begin() == false)
  {
    while (1)
    {
      Serial.println("SGP40 not found");
      delay(2500);
    }
  }

  if (!status)
  {
    while (1)
    {
      Serial.println("BMP280 not found");
      delay(2500);
    }
  }

}

void startWiFi()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}

void loop() {
  // DHT11
  float temperatureC = dht.readTemperature();
  float humidity = dht.readHumidity();
  float heatIndexC = dht.computeHeatIndex(temperatureC, humidity, false);

  // BMP280
  float atmosphericPressure = bmp.readPressure();

  // SGP40
  int airQuality = sgp40.getVOCindex(humidity, temperatureC);

  // Test
  Serial.print("Temp C: ");
  Serial.print(temperatureC);
  Serial.print("  ,Heat index C: ");
  Serial.print(heatIndexC);
  Serial.print("  ,Humidity: ");
  Serial.print(humidity);
  Serial.print("  ,Atmospheric pressure: ");
  Serial.print(atmosphericPressure);
  Serial.print("  ,Air quality: ");
  Serial.print(airQuality);
  Serial.println();

  // send data only when timer is equal to zero.
  if (timer <= 0)
  {
    String sensorData = getSensorDataString(temperatureC, humidity, heatIndexC, atmosphericPressure, airQuality);
    sendData(sensorData);

    // reset timer
    timer = postInterval;
  }

  timer--;
  delay(1000);
}

String getSensorDataString(float temperatureC, float humidity, float heatIndexC, float atmosphericPressure, int airQuality)
{
  DynamicJsonDocument doc(2048);
  JsonArray array = doc.to<JsonArray>();

  // Add temperature
  if (!isnan(temperatureC))
  {
    JsonObject t = array.createNestedObject();
    t["sensorName"] = "Temperature";
    t["value"] = temperatureC; 
  }

  // Add humidity
  if (!isnan(humidity))
  {
    JsonObject h = array.createNestedObject();
    h["sensorName"] = "Humidity";
    h["value"] = humidity; 
  }

  // Add heat index
  if (!isnan(heatIndexC))
  {
    JsonObject hI = array.createNestedObject();
    hI["sensorName"] = "Heat index";
    hI["value"] = heatIndexC; 
  }

  // Add atmospheric pressure
  JsonObject aP = array.createNestedObject();
  aP["sensorName"] = "Atmospheric pressure";
  aP["value"] = atmosphericPressure; 

  // Add airquality
  if (airQuality != -100 && airQuality != 0) // -100 = error, 0 = sensor warming up
  {
    JsonObject aQ = array.createNestedObject();
    aQ["sensorName"] = "Air quality";
    aQ["value"] = airQuality; 
  }

  doc["sensors"] = array;

  String json;
  serializeJson(doc, json);
  return json;
}

bool sendData(String sensorData)
{
  if(WiFi.status() != WL_CONNECTED){ // If WiFi has been disconnected
    reconnectWiFi();
  }

  HTTPClient http;

  http.begin(apiURL);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("authorization", apiKey);
  int res = http.POST(sensorData);
  http.end();

  if (res != 200) {
    Serial.print("Error code (sendData): ");
    Serial.println(res);
    return false;
  }
  return true;
}

void reconnectWiFi()
{
  WiFi.reconnect();
  Serial.print("Reconnecting to WiFi ");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
}
