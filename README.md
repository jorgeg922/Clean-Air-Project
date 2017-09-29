# Clean-Air-Project
An arduino project to improve the air quality around you.

This project makes use of sound sensor, air quality sensor and temperature sensor along with an Arduino Nano and NodeMCU to provide you with data about the environment around you.

The Air Quality Sensor has been set up to detect CO2 levels above 1500ppm. However, this value can be changed to detect higher levels.
The Sound Sensor has been assigned arbitrary levels of sound which are output to the JSON server as Silent, Moderate or Loud.
The temperature sensor simply read the temperature and humidity from a DHT11 sensor and outputs the readings to the JSON server.

All three of these components are attached to the nodeMCU which take care of setting up the JSON server and intermittently updating the data.

Please refer to the How-to-Guide in this repository for set-up instructions
