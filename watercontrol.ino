#include <OneWire.h>
#include <DallasTemperature.h>

//Wifi
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

ESP8266WiFiMulti WiFiMulti;

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS D5
#define TEMPERATURE_PRECISION 9 // Lower resolution

//#define L_RATE 0.01

const unsigned long eventInterval = 1000;
unsigned long previousTime = 0;

#define trig D6 
#define echo D7

long durasi, jarak;     // membuat variabel durasi dan jarak

#define relay 5

String _str = "";
char rx_byte = 0;
int heaterState = 0;

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

int numberOfDevices; // Number of temperature devices found

DeviceAddress tempDeviceAddress; // We'll use this variable to store a found device address

float w[4] = {0.01404401, 0.54064899, 0.08022875, -0.51533485};

#define ArrayLenth  40

int pHArray[ArrayLenth];
int pHArrayIndex=0;

float  m = -0.088, _b=36.74;
static float pHValue;

void setup() {
  // put your setup code here, to run once:
  // start serial port
  Serial.begin(115200);

  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("A-Malik", "malikinnas");

  pinMode(trig, OUTPUT);    // set pin trig menjadi OUTPUT
  pinMode(echo, INPUT);     // set pin echo menjadi INPUT
  
  // Start up the library
  sensors.begin();

  // Pin for relay module set as output
  pinMode(relay, OUTPUT);
  digitalWrite(relay, HIGH);

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
  for(int i=0;i<numberOfDevices; i++)
  {
    // Search the wire for address
    if(sensors.getAddress(tempDeviceAddress, i)){
      Serial.print("Found device ");
      Serial.print(i, DEC);
      Serial.print(" with address: ");
      printAddress(tempDeviceAddress);
      Serial.println();
      
      Serial.print("Setting resolution to ");
      Serial.println(TEMPERATURE_PRECISION, DEC);
      
      // set the resolution to TEMPERATURE_PRECISION bit (Each Dallas/Maxim device is capable of several different resolutions)
      sensors.setResolution(tempDeviceAddress, TEMPERATURE_PRECISION);
      
      Serial.print("Resolution actually set to: ");
      Serial.print(sensors.getResolution(tempDeviceAddress), DEC); 
      Serial.println();
    }else{
      Serial.print("Found ghost device at ");
      Serial.print(i, DEC);
      Serial.print(" but could not detect address. Check power and cabling");
    }
  }
}

int temp=1, pH=1, wH=1;
float _pH=7;

void loop() {
  // put your main code here, to run repeatedly:

  temp = tempNormalization(getTemperature());
  wH = heightNormalization(getWh());

//  Serial.println(getTemperature());
//  Serial.println(getPh());
//  Serial.println(getWh());
//  Serial.println("==========================");

  while(Serial.available() > 0){
    rx_byte = Serial.read();
    
    if((rx_byte >= '0' && rx_byte <= '9') || rx_byte == '.')
      _str.concat((char) rx_byte);
  }
  
  if(_str != ""){
    _pH = _str.toFloat();
    pH = pHNormalization(_pH);
    _str = "";
  }

  delay(10); 

  if(millis() - previousTime >= eventInterval){

    float q = getWaterQuality(temp, pH, wH);
    heaterHandler(q);
    sendData(q);

    previousTime = millis();
  }
}

float getTemperature(){
  sensors.requestTemperatures(); // Send the command to get temperatures
 
  return sensors.getTempC(tempDeviceAddress);;
}

float getPh(){

  pHArray[pHArrayIndex++]=analogRead(A0);
  if(pHArrayIndex==ArrayLenth)pHArrayIndex=0;
  pHValue = m * avergearray(pHArray, ArrayLenth) + _b;
  return _pH;
}

float getWh(){
  float distance;
  
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds

  // Calculating the distance
  float duration = pulseIn(echo, HIGH);
  distance = duration * 0.034 / 2;
  delay(1000);

  return (19.00 - distance);
}

void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}

float getWaterQuality(int temp, int pH, int wH){
  return (temp * w[0]) + (pH * w[1]) + (wH * w[2]) + w[3];
}

/*void trainingAdaline(){
  int epoch = 0, error;
  float output;
  
  while(epoch < 100){
    for(int i=0; i<19; i++){
      output = 0;
      for(int j=0; j<4; j++){
         output += (dTraining[i][j] * _w[j]);
      }
      error = dTarget[i] - activation(output);
      _w[0] += (L_RATE * error * dTraining[i][0]);
      _w[1] += (L_RATE * error * dTraining[i][1]);
      _w[2] += (L_RATE * error * dTraining[i][2]);
      _w[3] += (L_RATE * error * dTraining[i][3]);
    }
      
    epoch++;
  }
}*/

int activation(float val){
  if(val > 0)
    return 1;
  else
    return -1;
}

int tempNormalization(float val){
  if(val > 30)
    return 0;
  else if(val < 25)
    return -1;
  else
    return 1;
}

int pHNormalization(float val){
  if(val > 8)
    return 0;
  else if(val < 6)
    return -1;
  else
    return 1;
}

int heightNormalization(float val){
  if(val >= 11)
    return 0;
  else if(val <= 9)
    return -1;
  else
    return 1;
}

void heaterHandler(float quality){
  int q = round(quality * 100);
  if(q == round(0.01*100) || q == round(0.09*100)){
    digitalWrite(relay, LOW);
    heaterState = 1;
  }
  else {
    digitalWrite(relay, HIGH); 
    heaterState = 0;
  }
}

void sendData(float q){
  String req = "http://192.168.137.1/write?temp=" + (String) getTemperature() + "&ph=" + (String) getPh() + "&high=" + (String) getWh() + "&q=" + (String) q + "&heater=" + (String) heaterState;
  if ((WiFiMulti.run() == WL_CONNECTED)) {

    WiFiClient client;

    HTTPClient http;

    Serial.print("[HTTP] begin...\n");
    if (http.begin(client, req)) {  // HTTP


      Serial.print("[HTTP] GET...\n");
      // start connection and send HTTP header
      int httpCode = http.GET();

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = http.getString();
          Serial.println(payload);
        }
      } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }

      http.end();
    } else {
      Serial.printf("[HTTP} Unable to connect\n");
    }
  }
}

double avergearray(int* arr, int number){
  int i;
  int max,min;
  double avg;
  long amount=0;
  if(number<=0){
    Serial.println(F("Error number for the array to avraging!/n"));
    return 0;
  }
  if(number<5){   //less than 5, calculated directly statistics
    for(i=0;i<number;i++){
      amount+=arr[i];
    }
    avg = amount/number;
    return avg;
  }else{
    if(arr[0]<arr[1]){
      min = arr[0];max=arr[1];
    }
    else{
      min=arr[1];max=arr[0];
    }
    for(i=2;i<number;i++){
      if(arr[i]<min){
        amount+=min;        //arr<min
        min=arr[i];
      }else {
        if(arr[i]>max){
          amount+=max;    //arr>max
          max=arr[i];
        }else{
          amount+=arr[i]; //min<=arr<=max
        }
      }//if
    }//for
    avg = (double)amount/(number-2);
  }//if
  return avg;
}
