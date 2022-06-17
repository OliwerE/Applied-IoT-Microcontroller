#include <WiFi.h>
#include <HTTPClient.h>
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

  delay(1000);
}
