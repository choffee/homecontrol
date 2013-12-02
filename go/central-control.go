// This is going to be the central controller bringing together some of the parts and 
// makeing the decisions about things.
//
// A lot of listeners
//
package main

import (
	"arduino"
	"log"
	"regexp"
	//"power"
	//"weather"
)

var arduino = make(chan string, 5)

var device_re, err = regexp.Compile(`/homeauto/power/([^/]+)`)

func handlePower(c *mosquitto.Conn, m mosquitto.Message) {
	log.Printf("foo <- (%s)\nfoo -> bar(%s)\n", m.Payload, m.Payload)
	rec := device_re.FindStringSubmatch(m.Topic)
	log.Printf("%v", rec[1])
	// Check the length of rec here XXX
	device := rec[1]
	if string(m.Payload) == "on" || string(m.Payload) == "off" {
		msg := "RF  " + device + string(m.Payload) + "\n"
		arduino <- msg
	}
}

func main() {
	go arduino.ArduinoControl(arduino)

	// Now setup the mqtt connection
	conn, _ := mosquitto.Dial("centralcontroller", "localhost:1883", true)
	go conn.Listen()

	err = conn.HandleFunc("/homeauto/power/#", 2, handlePower)
}
