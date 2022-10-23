#include "PubSubClient.h" // Connect and publish to the MQTT broker
#include "Arduino.h"
#include <Wire.h>            // include Arduino Wire library (required for I2C devices)
#include "src\VEML7700\VEML7700.h"     // VEML7700 library - expect bugs
#include <Adafruit_Sensor.h>  //DHT sensor prerequisite
#include <DHT.h>           
#include <DHT_U.h>
VEML7700 myVEML;

// Code for the ESP32
// #include "WiFi.h" // Enables the ESP32 to connect to the local network (via WiFi)

// Code for the ESP8266
#include "ESP8266WiFi.h"  // Enables the ESP8266 to connect to the local network (via WiFi)

#define DS1621_ADDRESS  0x48
#define EUID 12345

//Define values related to the moisture sensor
#define MOISTURE_PIN A0
#define ADC_MAX 1024.0  //Max value of ADC
#define ADC_WETTEST 375
#define ADC_DRIEST 661

//DHT sensor variables
#define DHT_PIN 12
#define DHTTYPE DHT11     
DHT_Unified dht(DHT_PIN, DHTTYPE);
sensor_t sensor;

//Define whether each sensor is connected
#define VEML7700_CONNECTED true           //Define whether or not the ambient light sensor is being used
#define MOISTURE_SENSOR_CONNECTED true    //Define whether or not the moisture sensor is being used
#define DHT11_CONNECTED true              //Define whether or not the humidity sensor is being used


// WiFi
const char* ssid = "ssid";                 // Your personal network SSID
const char* wifi_password = "password"; // Your personal network password

// MQTT
const char* mqtt_server = "192.168.1.65";  // IP of the MQTT broker
const char* message_topic = "home/message/test"; // Message location
const char* mqtt_username = "username"; // MQTT username
const char* mqtt_password = "test123"; // MQTT password
const char* clientID = "testNode"; // MQTT client ID
char c_buffer[8], f_buffer[8];
float temp = 0.0;
float moisture = 0.0;
float uv = 0.0;
float humid = 0.0;
float lux;

// Initialise the WiFi and MQTT Client objects
WiFiClient wifiClient;
// 1883 is the listener port for the Broker
PubSubClient client(mqtt_server, 1883, wifiClient); 


// Custom function to connet to the MQTT broker via WiFi
void connect_MQTT(){
  Serial.print("Connecting to ");
  Serial.println(ssid);

  // Connect to the WiFi
  WiFi.begin(ssid, wifi_password);

  // Wait until the connection has been confirmed before continuing
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Debugging - Output the IP Address of the ESP8266
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Connect to MQTT Broker
  // client.connect returns a boolean value to let us know if the connection was successful.
  // If the connection is failing, make sure you are using the correct MQTT Username and Password (Setup Earlier in the Instructable)
  if (client.connect(clientID, mqtt_username, mqtt_password)) {
    Serial.println("Connected to MQTT Broker!");
  }
  else {
    Serial.println("Connection to MQTT Broker failed...");
  }
}


int16_t get_temperature() {
  Wire.beginTransmission(DS1621_ADDRESS); // connect to DS1621 (send DS1621 address)
  Wire.write(0xAA);                       // read temperature command
  Wire.endTransmission(false);            // send repeated start condition
  Wire.requestFrom(DS1621_ADDRESS, 2);    // request 2 bytes from DS1621 and release I2C bus at end of reading
  uint8_t t_msb = Wire.read();            // read temperature MSB register
  uint8_t t_lsb = Wire.read();            // read temperature LSB register
 
  // calculate full temperature (raw value)
  int16_t raw_t = (int8_t)t_msb << 1 | t_lsb >> 7;
  // convert raw temperature value to tenths °
  raw_t = raw_t * 10/2;
  return raw_t/10;
}

float get_humidity(){
  sensors_event_t event;
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    //Return error
    return -1;
  }
  else {
    
    return event.relative_humidity;
  }
}

//Map function with decimal arithmetic
double fmap(double x, double in_min, double in_max, double out_min, double out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

float get_moisture(){
  int moistureADC = analogRead(MOISTURE_PIN);                                           //Read from ADC
  int constrainedADC = constrain(moistureADC, ADC_WETTEST, ADC_DRIEST);                 //Constrain ADC to expected max/min
  float dryPercent = (float)fmap(constrainedADC, ADC_WETTEST, ADC_DRIEST, 0, 100);      //Map ADC range to percentage
  float moisturePercent = 100 - dryPercent;                                             //Convert from dryness % to moisture %

  //Ensure percentage is within expected range (update with error detection e.g., negative numbers etc.)
  if(moisturePercent > 100){
    moisturePercent = 100;    
  }
  else if(moisturePercent < 0){
    moisturePercent = 0;
  }
  return moisturePercent;
}


void setup() {
  // put your setup code here, to run once:
  Wire.begin();           // join i2c bus
  dht.begin();            //Initialise DHT sensor
  // initialize DS1621 sensor
  Wire.beginTransmission(DS1621_ADDRESS); // connect to DS1621 (send DS1621 address)
  Wire.write(0xAC);                       // send configuration register address (Access Config)
  Wire.write(0);                          // perform continuous conversion
  Wire.beginTransmission(DS1621_ADDRESS); // send repeated start condition
  Wire.write(0xEE);                       // send start temperature conversion command
  Wire.endTransmission(); 
  Serial.begin(9600);  
}

void loop() {
  connect_MQTT();
  Serial.setTimeout(2000);
  // temp = get_temperature();
  temp = rand() % 100;
  //uv = rand() % 100;
  
  //Get humidity
  if(DHT11_CONNECTED){
    humid = get_humidity();
  }
  else{
    humid = rand() % 100;
  }

  //Get ALS reading in lux
  if(VEML7700_CONNECTED){
    lux = myVEML.getAmbient();
  }
  else{
    lux = rand() % 100;
  }

  //Get soil moisture value
  if(MOISTURE_SENSOR_CONNECTED){
    moisture = get_moisture();
  }
  else{
    moisture = rand() % 100;
  }
  
  
  Serial.print("Message/" + String(EUID) + "/" + String((float)temp) + "/" + String((float)moisture) + "/" + String((float)lux) + "/" + String((float)humid));

  // MQTT can only transmit strings
  String hs="Message/" + String(EUID) + "/"+ String((float)temp) + "/" + String((float)moisture) + "/" + String((float)lux)+ "/" + String((float)humid);

  // PUBLISH to the MQTT Broker (topic = Temperature, defined at the beginning)
  if (client.publish(message_topic, String(hs).c_str())) {
    Serial.println("Message sent!");
  }
  // Again, client.publish will return a boolean value depending on whether it succeded or not.
  // If the message failed to send, we will try again, as the connection may have broken.
  else {
    Serial.println("Message failed to send. Reconnecting to MQTT Broker and trying again");
    client.connect(clientID, mqtt_username, mqtt_password);
    delay(10); // This delay ensures that client.publish doesn't clash with the client.connect call
    client.publish(message_topic, String(hs).c_str());
  }
  client.disconnect();  // disconnect from the MQTT broker
  Serial.println("client disconnected successfully");
  delay(1000*60);       // print new values every 1 Minute
}