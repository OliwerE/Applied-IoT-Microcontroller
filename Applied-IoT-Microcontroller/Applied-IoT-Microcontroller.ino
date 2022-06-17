#include "DHT.h"
#include "SparkFun_SGP40_Arduino_Library.h"
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BMP280.h>

Adafruit_BMP280 bmp;
#define DHTTYPE DHT11
#define DHTPIN 14

DHT dht(DHTPIN, DHTTYPE);
SGP40 sgp40; //create an object of the SGP40 class

void setup() {
  Serial.begin(115200);

  dht.begin();
  
  Wire.begin();

  unsigned status;
  status = bmp.begin(0x76);
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */

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

void loop() {
  // DHT11
  float temperatureC = dht.readTemperature();
  float humidity = dht.readHumidity();
  float heatIndexC = dht.computeHeatIndex(temperatureC, humidity, false);

  // BMP280
  float airPressure = bmp.readPressure();

  // SGP40
  int airQuality = sgp40.getVOCindex(humidity, temperatureC);

  // Test
  Serial.print("Temp C: ");
  Serial.print(temperatureC);
  Serial.print("  ,Heat index C: ");
  Serial.print(heatIndexC);
  Serial.print("  ,Humidity: ");
  Serial.print(humidity);
  Serial.print("  ,Air Pressure: ");
  Serial.print(airPressure);
  Serial.print("  ,Air quality: ");
  Serial.print(airQuality);
  Serial.println();

  delay(1000);
}
