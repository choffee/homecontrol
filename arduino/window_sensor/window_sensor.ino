//////////////////////////////////////////////////////////////////////////
// LLAP temperature and humidity sensor using a DHT11
//
// Reading temperature or humidity takes about 250 milliseconds!
// Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
//
// will work with any Arduino compatible however the target boards are the 
// Ciseco XinoRF and RFu-328, for LLAP over radio
// 
//
// Uses the Ciseco LLAPSerial library
// Uses the Adafruit DHT library https://github.com/adafruit/DHT-sensor-library
//////////////////////////////////////////////////////////////////////////

#include <LLAPSerial.h>
#include <DHT.h>

#define DEVICEID "WD"	// this is the LLAP device ID

#define RELAY_0_PIN 2
#define DHT_PIN 5
#define SOIL_POWER_PIN 7
#define SOIL_0_PIN A0
#define SOIL_1_PIN A1
#define DHTTYPE DHT11

DHT dht(DHT_PIN, DHTTYPE);

void setup_srf() {
  Serial.print("+++");
  delay(2000);
  //delay(1000);
  // Setup some things
  //http://openmicros.org/index.php/articles/88-%C2%AD%E2%80%90ciseco-%C2%AD%E2%80%90product-%C2%AD%E2%80%90documentation/260-%C2%AD%E2%80%90srf-%C2%AD%E2%80%90configuration
  // Set encryption key
  //Serial.println("ATEA HOMEAUTO");
  // Turn on encryption
  //Serial.println("ATEE1");
  // Set the PAN ID
  Serial.println("ATID3489");
  // Put the led into RSSI mode
  Serial.println("ATLI R");
  // Apply the config
  //Serial.println("ATAC");
  // Exit config mode
  Serial.println("ATDN");
}

void setup() {
	Serial.begin(115200);
	pinMode(8,OUTPUT);		// switch on the radio
	digitalWrite(8,HIGH);
	pinMode(4,OUTPUT);		// switch on the radio
	digitalWrite(4,LOW);	// ensure the radio is not sleeping
	delay(1000);				// allow the radio to startup

        setup_srf();
	LLAP.init(DEVICEID);

	dht.begin();

	LLAP.sendMessage(F("STARTED"));

}

void loop() {
	// print the string when a newline arrives:
	if (LLAP.bMsgReceived) {
		Serial.print(F("msg:"));
		Serial.println(LLAP.sMessage); 
		LLAP.bMsgReceived = false;	// if we do not clear the message flag then message processing will be blocked
	}

	// every 3 seconds
	static unsigned long lastTime = millis();
	if (millis() - lastTime >= 3000)
	{
  		lastTime = millis();
		int h = dht.readHumidity() * 10;
		int t = dht.readTemperature() * 10;
                // power up the soil sensors
                digitalWrite(SOIL_POWER_PIN, HIGH);
                delay(100);
                int soila = analogRead(SOIL_0_PIN);
                int soilb = analogRead(SOIL_1_PIN);
                digitalWrite(SOIL_POWER_PIN, LOW);

 		// check if returns are valid, if they are NaN (not a number) then something went wrong!
		if (isnan(t) || isnan(h)) {
			LLAP.sendMessage(F("ERROR"));
		} else {
			LLAP.sendIntWithDP("HUM",h,1);
			//delay(100);
			LLAP.sendIntWithDP("TMP",t,1);
                        LLAP.sendInt("SOA",soila);
                        LLAP.sendInt("SOB",soilb);
		}
	}
}
