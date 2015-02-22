
#include "DHT.h"


#define RELAY_0_PIN 0
#define DHT_PIN 5
#define SOIL_POWER_PIN 7
#define SOIL_0_PIN A0
#define SOIL_1_PIN A1
#define DHTTYPE DHT11

DHT dht(DHT_PIN, DHTTYPE);

String inputString = "";
boolean stringComplete = false;

void setup() {
  // Turn on the SRF stuff
  pinMode(8, OUTPUT);
  digitalWrite(8, HIGH);
  Serial.begin(115200);
  Serial.print("+++");
  delay(1000);
  // Setup some things
  //http://openmicros.org/index.php/articles/88-%C2%AD%E2%80%90ciseco-%C2%AD%E2%80%90product-%C2%AD%E2%80%90documentation/260-%C2%AD%E2%80%90srf-%C2%AD%E2%80%90configuration
  // Set encryption key
  Serial.println("ATEA HOMEAUTO");
  // Turn on encryption
  Serial.println("ATEE");
  // Set the PAN ID
  Serial.println("ATID 3489");
  // Exit config mode
  Serial.println("ATDN");
  // Comment out above for default serial
  // Serial.begin(9600);
  Serial.println("Window Sensor");
  inputString.reserve(200);
  dht.begin();
  
  pinMode(RELAY_0_PIN, OUTPUT);
  pinMode(SOIL_POWER_PIN, OUTPUT);
  
  
  digitalWrite(RELAY_0_PIN, LOW);
  digitalWrite(SOIL_POWER_PIN, LOW);
  
}

int soilSensor = 0;

void loop() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    inputString += inChar;
    if (inChar == '\n' ) {
      stringComplete = true;
    }
  }
  if (stringComplete) {
    if (inputString == "RELAY:0:ON\n") {
      digitalWrite(RELAY_0_PIN, HIGH);
      Serial.println("RELAY:0:STATE:ON");
    }
    if (inputString == "RELAY:0:OFF\n") {
      digitalWrite(RELAY_0_PIN, LOW);
      Serial.println("RELAY:0:STATE:OFF");
    }
    Serial.print("echo:");
    Serial.println(inputString);
    inputString = "";
    stringComplete = false;
    
  }
  
  delay(2000);
  digitalWrite(SOIL_POWER_PIN, HIGH);
  delay(100);
  soilSensor = analogRead(SOIL_0_PIN);
  Serial.print("Soil Sensor 0: ");
  Serial.println(soilSensor);
  soilSensor = analogRead(SOIL_1_PIN);
  Serial.print("Soil Sensor 1: ");
  Serial.println(soilSensor);
  digitalWrite(SOIL_POWER_PIN, LOW);
  
  
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read DHT sensor!");
    return;
  }
  
  Serial.print("Humidity: ");
  Serial.println(h);
  Serial.print("Temperature: ");
  Serial.println(t);

  
  
}



