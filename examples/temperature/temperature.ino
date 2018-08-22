#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

// assign the ESP8266 pins to arduino pins
#define D1 5
#define D2 4
#define D3 0
#define D4 2
#define D5 14

// assign the SPI bus to pins
#define BME_SCK D1
#define BME_MISO D5
#define BME_MOSI D2
#define BME1_CS D3
#define BME2_CS D4

#define SEALEVELPRESSURE_HPA (1017.3)

Adafruit_BME280 bme1(BME1_CS, BME_MOSI, BME_MISO, BME_SCK); // software SPI

unsigned long delayTime;

void setup() {
    Serial.begin(115200);
    Serial.println(F("BME280 test"));

    bool status;
    
    // default settings
    status = bme1.begin();
    if (!status) {
        Serial.println("Could not find a valid BME280 sensor 1 , check wiring!");
    }


    
    Serial.println("-- Default Test --");
    delayTime = 5000;

    Serial.println();
}


void loop() { 
    printValues();
    delay(delayTime);
}


void printValues() {
    Serial.print("Temperature = ");
    Serial.print(bme1.readTemperature());
    Serial.println(" *C");


    Serial.print("Pressure = ");
    Serial.print(bme1.readPressure() / 100.0F);
    Serial.println(" hPa");


    Serial.print("Approx. Altitude = ");
    Serial.print(bme1.readAltitude(SEALEVELPRESSURE_HPA));
    Serial.println(" m");


    Serial.print("Humidity = ");
    Serial.print(bme1.readHumidity());
    Serial.println(" %");


        

    Serial.println();
}
