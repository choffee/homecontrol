package arduino

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

func ArduinoControl(cmd *chan<- *string) {
	// Connect to the serial port
	c := &serial.Config{Name: "/dev/ttyUSB0", Baud: 9600}
	s, err := serial.OpenPort(c)
	if err != nil {
		log.Fatal(err)
	}

	// Just read in all the input for now
	serialLines := new(chan<- *string)
	go serialReader(&s, serialLines)

	for {
		line := <-cmd
		_, err = s.Write([]byte(line))
	}

}
