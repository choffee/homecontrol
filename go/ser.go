package main

import (
  "flag"
  "fmt"
	"github.com/tarm/goserial"
	"io"
	"log"
	"time"
)

func serialReader(serialPort *io.ReadWriteCloser, line *chan<- *string) {
	// Reads continuisly from a serial port and sends whole line back
	buf := make([]byte, 128)
	for {
		n, err := (*serialPort).Read(buf)
		if err != nil {
			log.Fatal(err)
			break
		}
    log.Printf("byte:%q", buf[n])
	}

}

func main() {

  var usbdev = flag.String("usb", "/dev/ttyUSB0", "The USB Device")
  var command = flag.String("command", "on", "The command")
  var device = flag.String("device", "A2", "The device to switch")
  flag.Parse()

	c := &serial.Config{Name: *usbdev, Baud: 9600}
	s, err := serial.OpenPort(c)
	if err != nil {
		log.Fatal(err)
	}

	serialLines := new(chan<- *string)

	go serialReader(&s, serialLines)

	time.Sleep(2000 * time.Millisecond)

	_, err = s.Write([]byte(fmt.Sprintf("RF  %s%s\n", *device, *command)))
	if err != nil {
		log.Fatal(err)
	}

	time.Sleep(2000 * time.Millisecond)

}
