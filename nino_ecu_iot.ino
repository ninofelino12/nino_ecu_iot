/*
 Based in the RTDB Basic Example by Firebase-ESP-Client library by mobizt
  https://github.com/mobizt/Firebase-ESP-Client/blob/main/examples/RTDB/Basic/Basic.ino
*/

#include <Arduino.h>
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <SoftwareSerial.h>

#include "SIM800L.h"

#define SIM800_RX_PIN D4
#define SIM800_TX_PIN D3
#define SIM800_RST_PIN D5

SIM800L* sim800l;

#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
#define WIFI_SSID "MEGAVISION"
#define WIFI_PASSWORD "nino2345"
#define API_KEY "AIzaSyCmZ7ySVfRFKVOfLXBs8rKin3VkKLgcgOc"
#define DATABASE_URL "https://myiot-5a297-default-rtdb.asia-southeast1.firebasedatabase.app" //<databaseName>.firebaseio.com or <databaseName>.<region>.firebasedatabase.app


//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;
LiquidCrystal_I2C lcd(0x27, 16,2);
const int BUTTON_PIN = D5; // The ESP8266 pin D7 connected to button

int prev_button_state = HIGH; // the previous state from the input pin
int button_state;    // the current reading from the input pin
String macStr;
  
void setup(){
SoftwareSerial* serial = new SoftwareSerial(SIM800_RX_PIN, SIM800_TX_PIN);

  serial->begin(9600);
   serial->begin(9600);
  delay(1000);

  // Initialize SIM800L driver with an internal buffer of 200 bytes and a reception buffer of 512 bytes, debug disabled
  sim800l = new SIM800L((Stream *)serial, SIM800_RST_PIN, 200, 512);

 pinMode(BUTTON_PIN, INPUT_PULLUP);
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    Serial.println(WiFi.macAddress());
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  scani2c();
   lcd.init();
   lcd.backlight();
    lcd.clear();
 macStr=WiFi.macAddress();   
 

}

void loop(){
 button_state = digitalRead(BUTTON_PIN);
 status();
  Serial.println(WiFi.macAddress());
lcd.clear();
  if(button_state == LOW ){
    Serial.println("LOW------------------------------------------------- ");
    lcd.setCursor(0, 0);
    lcd.print("LOW");

  } else {
 Serial.println("HIGHT------------------------------------------------- ");
 lcd.setCursor(0, 0);
 lcd.print("HEIGHT");
  };
    

  // save the last state
  prev_button_state = button_state;
  lcd.setCursor(0, 0);
  // print message
 // lcd.print("Hello, World!");
   delay(1000);
 
   scani2c();
 // String timestamp = String(millis());
  String timestamp = "1000";
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0)){
   // sendDataPrevMillis = millis();
      sendDataPrevMillis = 10000;
    // Write an Int number on the database path test/int
    String path ="/"+macStr+"/temperature" + timestamp+"/temp:";
    //Firebase.RTDB.setFloat(firebaseData, path, 40);
    
    if (Firebase.RTDB.setInt(&fbdo,path, count)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
      lcd.print("PATH: " + fbdo.dataPath());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }

    count++;
    lcd.setCursor(0, 10);
  // print message
  lcd.print(count);
    // Write an Float number on the database path test/float
    if (Firebase.RTDB.setFloat(&fbdo, path+"/float", 0.01 + random(0,100))){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
  }
}


void scani2c() {
  byte error, address;
  int nDevices;
  Serial.println("Scanning...");
  nDevices = 0;
  for(address = 1; address < 127; address++ ) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address<16) {
        Serial.print("0");
      }
      Serial.println(address,HEX);
      nDevices++;
    }
    else if (error==4) {
      Serial.print("Unknow error at address 0x");
      if (address<16) {
        Serial.print("0");
      }
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0) {
    Serial.println("No I2C devices found\n");
  }
  else {
    Serial.println("done\n");
  }
  delay(5000);          
}

void status(){
     Serial.print("status"+macStr);
      Serial.print("->PIN:");
       Serial.print("status"+macStr);
}