
#include "DHT.h"


#define RELAY_0_PIN D0
#define DHT_PIN 5
#define SOIL_POWER_PIN 7
#define SOIL_0_PIN A0
#define SOIL_1_PIN A1
#define DHTTYPE DHT11

DHT dht(DHT_PIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  Serial.println("Window Sensor");
  dht.begin();
  
  pinMode(RELAY_0_PIN, OUTPUT);
  pinMode(SOIL_POWER_PIN, OUTPUT);
  
  
  digitalWrite(RELAY_0_PIN, LOW);
  digitalWrite(SOIL_POWER_PIN, LOW);
  
}

int soilSensor = 0;

void loop() {
  
  delay(2000);
  digitalWrite(SOIL_POWER_PIN HIGH);
  delay(100);
  soilSensor = analogRead(SOIL_0_PIN);
  Serial.print("Soil Sensor 0: ");
  Serial.println(soilSensor);
  soilSensor = analogRead(SOIL_1_PIN);
  Serial.print("Soil Sensor 1: ");
  Serial.println(soilSensor);
  
  
  
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read DHT sensor!");
    return;
  }
  
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.println(" *C ");
  
  
}
