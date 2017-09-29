/*
  Jorge Gonzales

  This code reads the data from a sound sensor, air quality sensor,
  and a temperature sensor usgin a NodeMCU and outputs it to a 
  webpage in JSON format.

  August 4, 2017
    
  This code is released under the [MIT License](http://opensource.org/licenses/MIT).
  Distributed as-is; no warranty is given.
*/

#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DHT.h>
#define DHTTYPE   DHT11
#define DHTPIN    2
#define AQPIN     12

#define WLAN_SSID     "WIFI_NAME"
#define WLAN_PASSWORD "WIFI_PASSWORD"

DHT dht(DHTPIN, DHTTYPE, 11);
ESP8266WebServer server(80);

float temperature, humidity, temperatureF;
unsigned long previousMillis = 0;
const long interval = 2000;


const int AnalogIn  = A0;
int soundLevel = 0;
int readingCO2 =0;
String CO2Level, soundWarn;

String Silent = "Silent";
String Moderate = "Moderate";
String Loud = "Loud"; 

void setup() {
    Serial.begin(115200);
    delay(10);

    dht.begin();
    pinMode(AQPIN, INPUT);
    
    // Unecessary. Uncomment if you like
    //humidity = dht.readHumidity();
    //temperature = dht.readTemperature();
    //soundLevel = analogRead(AnalogIn);

    Serial.println(); Serial.println();
    Serial.print("Connecting to ");
    Serial.println(WLAN_SSID);

    WiFi.mode(WIFI_STA);
    WiFi.begin(WLAN_SSID, WLAN_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println();

    Serial.println("WiFi connected");
    Serial.println("IP address: "); Serial.println(WiFi.localIP());

    server.on("/sensorData.json", [](){
        unsigned long currentMillis = millis();

        if (currentMillis - previousMillis >= interval) {
            previousMillis = currentMillis;

            humidity = dht.readHumidity(); //humidity
            delay(1000);
            temperature = dht.readTemperature(); //temperature
            delay(1000);
            temperatureF = temperature * (9/5) + 32;
            
            soundLevel = analogRead(AnalogIn); //Sound sensor data
            if(soundLevel < 30){ //arbitrary values
              soundWarn = Silent;
            } else if(soundLevel > 30 && soundLevel < 60){
              soundWarn = Moderate;
            }else if (soundLevel > 60){
              soundWarn = Loud;
            }
            readingCO2 = digitalRead(AQPIN);
            if(readingCO2 == 1){
              CO2Level = "High CO2 detected";
            }
            else{
              CO2Level = "CO2 level is normal";
            }
            
            
            Serial.println("Reporting CO2: " + String((int)readingCO2));
            Serial.println("Reporting Sound: " + String((int)soundLevel));
            Serial.println("Reporting DHT11: " + String((int)temperature) + "F and " + String((int)humidity) + " % humidity");
        }
        
        //Setting up JSON data
        DynamicJsonBuffer jsonBuffer; //Original buffer 500
        JsonObject& root = jsonBuffer.createObject();

        JsonArray& smokeArr = root.createNestedArray("Smoke Sensor");
        JsonObject& smokejson = smokeArr.createNestedObject();
        smokejson["CO2"]= readingCO2;
        smokejson["CO2 Level"] = CO2Level;
        
        JsonArray& tempArr = root.createNestedArray("Temperature Sensor");
        JsonObject& tempjson = tempArr.createNestedObject();
        tempjson["temperature (C)"] = temperature;
        tempjson["temperature (F)"] = temperatureF;
        tempjson["humidity"] = humidity;
        
        JsonArray& soundArr = root.createNestedArray("Sound Sensor");
        JsonObject& soundjson = soundArr.createNestedObject();
        soundjson["sound"] = soundLevel;
        soundjson["sound level"] = soundWarn;
        
        String jsonString;
        root.printTo(jsonString);

        Serial.println(jsonString);
        server.send(200, "application/json", jsonString);
    });
    
    server.begin();
    Serial.println("HTTP server started! Waiting for clients!");
}

void loop() {
    server.handleClient();
}
