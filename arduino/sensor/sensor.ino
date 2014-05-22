/*
 Copyright (C) 2011 J. Coliz <maniacbug@ymail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

/**
 * Read from a sensor and send that down the line to reader
 *
 * This is an example of how to use the RF24 class.  Write this sketch to two
 * different nodes.  Put one of the nodes into 'transmit' mode by connecting
 * with the serial monitor and sending a 'T'.  The ping node sends the current
 * time to the pong node, which responds by sending the value back.  The ping
 * node can then see how long the whole cycle took.
 */

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

//
// Hardware configuration
//

// Set up nRF24L01 radio on SPI bus plus pins 9 & 10

RF24 radio(9,10);

//
// Topology
//

// Radio pipe address
//const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };
const uint64_t pipe            = 0xF0F0F0F0E1LL;
const uint64_t nrfSensorPipe            = 0xF0F0F0F0E1LL;

const uint8_t sensor_power_pin = 1;
const uint8_t sensor_pin       = A0;

int sensorValue            = 0;

void setup(void)
{
  //
  // Print preamble
  //

  Serial.begin(57600);
  printf_begin();
  printf("\n\rRF24 Remote Sensor\n\r");

  //Setup sensor pins, and switch the power off
  pinMode(sensor_power_pin, OUTPUT);
  digitalWrite(sensor_power_pin, LOW);

  //
  // Setup and configure rf radio
  //

  radio.begin();

  // optionally, increase the delay between retries & # of retries
  radio.setRetries(15,15);

  // optionally, reduce the payload size.  seems to
  // improve reliability
  //radio.setPayloadSize(8);

  //
  // Open pipes to other nodes for communication
  //
  {
    radio.openWritingPipe(pipe);
  }
  //
  // Dump the configuration of the rf unit for debugging
  //

  radio.printDetails();
}

void loop(void)
{

    // Wake the reader
    digitalWrite(sensor_power_pin, HIGH);
    // Sleep for a bit to make sure it's awake
    delay(200);
    // Get a reading
    sensorValue = analogRead(sensor_pin);
    // Turn off the power
    digitalWrite(sensor_power_pin, LOW);
    // Send that down the pipe
    printf("Sensor reads: %hu\n", sensorValue);
    char outBuffer[32]="RemoteID=0 ";
    char temp[5];
    strcat(outBuffer, "Moisture_0=");
    sprintf(temp, "%d", sensorValue);
    strcat(outBuffer, temp);
    bool ok;
    ok = radio.write( &outBuffer, strlen(outBuffer));
    // Sleep for a big bit
    delay(5000);
}
// vim:cin:ai:sts=2 sw=2 ft=cpp
