package main

import (
	"io"
	"log"
	"regexp"

	"bytes"

	"bitbucket.org/shanehanna/mosquitto"
	"github.com/tarm/goserial"
)

func sendPinPress(buf []byte, conn *mosquitto.Conn) {
	bufSplit := bytes.Split(buf, []byte(":"))
	if len(bufSplit) < 2 {
		return
		log.Print(buf)
	}
	pin := bufSplit[1]
	m, _ := mosquitto.NewMessage("/remote/desk/events", pin)
	log.Printf("(%s) -> /remote/desk/events\n", m.Payload)
	err := conn.Publish(m)
	if err != nil {
		log.Print("Failed to publish pin press message")
	}
}

func serialReader(serialPort *io.ReadWriteCloser, conn *mosquitto.Conn) {
	// Reads continuisly from a serial port and sends whole line back
	buf := make([]byte, 128)
	var message bytes.Buffer
	for {
		n, err := (*serialPort).Read(buf)
		if err != nil {
			log.Fatalf("Error reading from serial port: %v", err)
			//break
		}
		message.Write(buf[:n])
		if bytes.Contains(buf[:n], []byte("\n")) {
			msg := message.Bytes()
			log.Printf("%q", msg)
			if bytes.Contains(msg, []byte("Pin:")) {
				sendPinPress(msg, conn)
			}
			message.Reset()
		}
	}
}

func main() {
	c := &serial.Config{Name: "/dev/ttyUSB0", Baud: 9600}
	s, err := serial.OpenPort(c)
	if err != nil {
		log.Fatal(err)
	}

	// Now setup the mqtt connection
	conn, _ := mosquitto.Dial("powercontroller", "localhost:1883", true)
	go conn.Listen()

	go serialReader(&s, &conn)

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
