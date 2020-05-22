// Example for reading out DS18B20 temperature sensors
// Used devices: DS18B20 sensor
// Michael Morscher, May 2020
// Tested on Arduino IDE 1.8.12 / VSCode
// Board: NodeMCU ESP8266 "Lolin v3"
// Based on: https://github.com/milesburton/Arduino-Temperature-Control-Library/blob/master/examples/Tester/Tester.pde

// Libraries:
// OneWire v2.3.5: https://github.com/PaulStoffregen/OneWire
// Arduino-Temperature-Control-Library v3.8.0: https://github.com/milesburton/Arduino-Temperature-Control-Library
#include <OneWire.h>
#include <DallasTemperature.h>

// Settings
#define VERSION 1.0
#define BAUDRATE 115200

// Define the pin you are using for onewire
#define ONE_WIRE_BUS D3

// Initiate one wire bus library with the previously specified PIN
OneWire oneWire(ONE_WIRE_BUS);

// Pass reference to one wire bus to dallas temperature library
DallasTemperature sensors(&oneWire);

int numberOfDevices; // Number of temperature devices found
DeviceAddress tempDeviceAddress; // We'll use this variable to store a found device address

void setup()
{
  // Serial configuration
  Serial.begin(BAUDRATE);
  Serial.print("Project version: ");
  Serial.println(VERSION);
  Serial.println("Setup initialised!");

  // Start sensor configuration
  sensors.begin();

  // Grab a count of devices on the wire
  numberOfDevices = sensors.getDeviceCount();

  // locate devices on the bus
  Serial.print("Locating devices...");

  Serial.print("Found ");
  Serial.print(numberOfDevices, DEC);
  Serial.println(" devices.");

  // report parasite power requirements
  Serial.print("Parasite power is: ");
  if (sensors.isParasitePowerMode()) Serial.println("ON");
  else Serial.println("OFF");

  // Loop through each device, print out address
  for (int i = 0; i < numberOfDevices; i++)
  {
    // Search the wire for address
    if (sensors.getAddress(tempDeviceAddress, i))
    {
      Serial.print("Found device ");
      Serial.print(i, DEC);
      Serial.print(" with address: ");
      printAddress(tempDeviceAddress);
      Serial.println();
    } else {
      Serial.print("Found ghost device at ");
      Serial.print(i, DEC);
      Serial.print(" but could not detect address. Check power and cabling");
    }
  }
}

// function to print the temperature for a device
void printTemperature(DeviceAddress deviceAddress)
{
  float tempC = sensors.getTempC(deviceAddress);
  if (tempC == DEVICE_DISCONNECTED_C)
  {
    Serial.println("Error: Could not read temperature data");
    return;
  }
  Serial.print("Temp C: ");
  Serial.print(tempC);
  Serial.print(" Temp F: ");
  Serial.println(DallasTemperature::toFahrenheit(tempC)); // Converts tempC to Fahrenheit
}

// function to print a device address
void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}

void loop(void)
{
  // call sensors.requestTemperatures() to issue a global temperature
  // request to all devices on the bus
  Serial.print("Requesting temperatures... ");
  sensors.requestTemperatures(); // Send the command to get temperatures
  Serial.println("DONE!");


  // Loop through each device, print out temperature data
  for (int i = 0; i < numberOfDevices; i++)
  {
    // Search the wire for address
    if (sensors.getAddress(tempDeviceAddress, i))
    {
      // Output the device ID
      Serial.print("Temperature for device: ");
      Serial.println(i, DEC);

      // It responds almost immediately. Let's print out the data
      printTemperature(tempDeviceAddress); // Use a simple function to print out the data
    }
    //else ghost device! Check your power requirements and cabling

  }
  
  delay(1000);
}
