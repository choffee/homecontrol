package main

import (
	"bitbucket.org/shanehanna/mosquitto"
	"github.com/tarm/goserial"
	"io"
	"log"
	"regexp"
)

func serialReader(serialPort *io.ReadWriteCloser, line *chan<- *string) {
	// Reads continuisly from a serial port and sends whole line back
	buf := make([]byte, 128)
	for {
		n, err := (*serialPort).Read(buf)
		if err != nil {
			log.Fatalf("Error reading from serial port: %v", err)
			//break
		}
		log.Printf("%q", buf[n])
	}

}

func main() {
	c := &serial.Config{Name: "/dev/ttyUSB0", Baud: 9600}
	s, err := serial.OpenPort(c)
	if err != nil {
		log.Fatal(err)
	}

	serialLines := new(chan<- *string)

	go serialReader(&s, serialLines)

	// Now setup the mqtt connection
	conn, _ := mosquitto.Dial("powercontroller", "localhost:1883", true)
	go conn.Listen()

	device_re, err := regexp.Compile(`/homeauto/power/([^/]+)`)
	if err != nil {
		log.Fatal(err)
	}
	err = conn.HandleFunc("/homeauto/power/#", 2, func(c *mosquitto.Conn, m mosquitto.Message) {
		log.Printf("foo <- (%s)\nfoo -> bar(%s)\n", m.Payload, m.Payload)
		rec := device_re.FindStringSubmatch(m.Topic)
		log.Printf("%v", rec[1])
		// Check the length of rec here XXX
		device := rec[1]
		if string(m.Payload) == "on" || string(m.Payload) == "off" {
			msg := "RF  " + device + string(m.Payload) + "\n"
			_, err = s.Write([]byte(msg))
			if err != nil {
				log.Fatal(err)
			}
		}
	})

	if err != nil {
		log.Fatal(err)
	}

	// Just wait at the end
	end := make(chan bool, 1)
	<-end

}
