# Indoor climate app
#### By Oliwer Ellréus (oe222ez)

## Project description
The project consist of a webapplication built using the MERN stack and a microcontroller that sends indoor climate data to the application. The sensors are temperature, humidity, heat index, air quality and air pressure. This data is visualized in the webapplication using gauges for the current value and four charts for each sensor (24h, 1d, 14d and 30d). The project took about a week to complete, but this was with experience from working with microcontrollers and the MERN stack before.

## Objective
I chose this project because I'm spending a lot of time indoor studying and because it would be interesting to know if my indoor climate was good for studying by measuring air quality, temperature, etc. The purpose of the project is to visualize my indoor climate over time and show the current values. I could use this insight to determine when to open a window or to know when it is not suitable to study in the room where the sensors are placed.

## List of material

* [ESP32 huzzah, 279kr, Electrokit](https://www.electrokit.com/produkt/adafruit-feather-huzzah32-esp32/)
* [Breadboard (800p), 24kr, Aliexpress](https://www.aliexpress.com/item/1005003880378231.html?spm=a2g0o.order_list.0.0.4ccc1802ktdiAn)
* [Breadboard cables, 29kr, Electrokit](https://www.electrokit.com/produkt/labbsladd-20-pin-15cm-hane-hane/)
* [Temperature and humidity sensor (DHT11), 49kr, Electrokit](https://www.electrokit.com/produkt/digital-temperatur-och-fuktsensor-dht11/)
* [Air quality sensor (SGP40), 249kr, Electrokit](https://www.electrokit.com/produkt/sparkfun-air-quality-sensor-sgp40-qwiic/)
* [Air pressure sensor (BMP280), 5,5kr, Aliexpress](https://www.electrokit.com/produkt/adafruit-feather-huzzah32-esp32/)

The ESP32 huzzah is a ESP32 microcontroller on a development board. It is used to read data from the sensors and sending the values to the backend of the webapplication where they are stored in a database. The breadboard and breadboard cables are used to connect the sensors to the microcontroller. The DHT11 sensor is used to read temperature, heat index and humidity. The SGP40 sensor is an air quality sensor. It has a index range between 0 and 500. Zero means the air quality is good and 500 is very bad.

## Computer setup
### Microcontroller
I have used the Arduino IDE. Libraries are installed by opening the library manager (ctrl + shift + I). The libraries used in this project are WiFi, HTTPClient, ArduinoJson, DHT, SparkFun_SGP40_Arduino_Library, Wire, SPI and Adafruit_BMP280. Search for all of these libraries in the library manager and press install. The arduino sketch must be open in the editor in order to flash the microcontroller. The board manager url has to be added the first time the ESP32 is used with the Arduino IDE. To add the URL open File -> Preferences then add the following URL "https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json" in the "Additional Boards Manager URL:s" field, then press ok. Then the board type has to be selected in Tools -> Board -> ESP32 Arduino. Choose "Adafruit ESP32 feather". Lastly the com port has to be selected in Tools -> Port. You will most likely only have one alternative. If you have many alternatives, choose one of them and if you get a port error when flashing change to another port. Now the microcontroller is ready to be flashed. Press the right arrow in the top menu of the Arduino IDE. The sketch will now be compiled and uploaded to the microcontroller if the compilation is successful.

### Webapplication
The webapplication have been developed using [Visual studio code](https://code.visualstudio.com/download). Both the frond end and backend is developed using NodeJS. I recommend to use the latest LTS version of [NodeJS](https://nodejs.org/en/) in order to run the applications. Install all modules when the frontend or backend application is opened on the computer using the command "npm install" in the teminal. then start the frontend or backend using npm start. The backend application does also require the installation of MongoDB. [Docker desktop](https://www.docker.com/products/docker-desktop/) is needed in order to run a MongoDB container. When docker desktop is installed and opened, go to images and search for MongoDB. Install then start the container.

### Workflow
Both the microcontroller sketch and the webapplication projects are uploaded to GitHub using [git](https://git-scm.com/downloads). The frondend and backend of the webapplication has two branches. Dev is used for development of new features and Main is the current version in production. The server is automatically updated using CI when code is pushed or pulled into the main branch. The server is an EC2 t3.micro instance from [AWS](https://aws.amazon.com/).

## Putting everything together
The DHT11 (temperature) sensor are connected to a digital, ground and 3.3v pin on the microcontroller. I used pin 14 to connect the sensor. The air pressure and air quality sensor are connected using the I2C protocol. This means that they can share pins for data transfer. Connect the scl and sda pins from the SGP40 and BMP280 sensor to the scl and sda pin on the microcontroller. Then connect the ground and 3.3v pins to the ground and 3.3v pins on the microcontroller.

All sensors used in this project are 3.3 volt. Other sensors can sometimes be 5v which is not supported by the ESP32. There is no need to use any resistors in this project. There is also not any need for an external power supply because the sensors does not use a lot of current. The setup could be used in production but a breadboard setup is meant for development. I would therefore recommend 3d printing a case and creating a custom PCB for long term use.

![image](/img/circuit-diagram.png)

## Platform
I decided to develop my own webapplication. The main advantage of creating my own webapplication is to be able to customize it exactly the way i want. Another advantage is that I own my webapplication instead of being limited by free tiers or functionality of other platforms. I did not try different platforms because I knew that I wanted to create my own webapplication from the beginning. I use a virtual server on AWS in order to host the webapplication. The server has a 12 month trial. The server will cost $0.0104 per hour after the trial.

The application have gauges showing the latest reported values from each sensor. It also have charts for each sensor. Each chart can show the latest 24 hours (default), 7 days, 14 days or 30 days.

Going forward I could have sensors in more rooms in order to measure the entire apartment. Another idea is to continue working on the webapplication. For example I could turn it into a new IoT platform where users can create their own dashboards.

## The code
* [Microcontroller](https://github.com/OliwerE/Applied-IoT-Microcontroller)
* [Webapplication frontend](https://github.com/OliwerE/Applied-IoT-Frontend)
* [Webapplication backend](https://github.com/OliwerE/Applied-IoT-Backend)
## Transmitting the data
The microcontroller sends the data once every five minutes. WiFi is used as the wireless protocol and the data is sent using HTTP.
I chose WiFi because the sensors are placed indoors. Which means that i have access to a power outlet. This is useful because WiFi uses a lot of power. If the sensors were placed outdoors and was powered by a battery it would be more useful to use a wireless protocol with lower power consumtion and sending the data less frequently. Then I could use LoRa as the wireless protocol, which is both has low power and long range.

## Presenting the data
![image](/img/dashboard.png)
The latest value is visualized using the gauges at the top. Each chart show the values from a sensor over time.

The data is sent to the backend every five minutes. All data reports from the microcontroller are stored in the database for 31 days. They are used to calculate the average hourly and daily values. I chose MongoDB because it can handle large amounts of data.

## Finalizing the design
![image](/img/breadboard.jpg)
I think the project went well. My idea from the beginning was to create a webapplication were you could create your own dashboards. In the end I decided to only create one hardcoded dashboard because I did not have enough time and because it was not a part of the course requirements, but I do plan to continue working on the application in the future.

[Link to webapplication](https://iot.oliwere.tk/)