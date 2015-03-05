// An LLAP to mqtt bridge
// This should be really simple so that it just works and the
// heavy lifting should be done elsewhere.
//

package main

import (
	"bytes"
	"github.com/tarm/goserial"
	"log"
	"math"
)

const (
	Alpha = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	Nums  = "0123456789"
)

type Msg struct {
	address [2]byte
	name    string
	value   float32
}

func breakoutMessage(message []byte) Msg {
	msg := Msg{}
	msg.address[0] = message[1]
	msg.address[1] = message[2]
	msg.name = ""
	msg.value = 0.0

	message = bytes.ToUpper(message)
	afterDecimal := false
	decimalCount := 0
	var chunk byte
	for l := 2; l < 11; l++ {
		switch chunk = message[l]; chunk {
		case '-':
			continue
		case (bytes.IndexAny([]byte(chunk), Alpha) > 0):
			msg.name.append(chunk)
		case chunk == '.':
			afterDecimal = True
		case bytes.Contains(Nums, chunk):
			msg.value = (msg.value * 10) + Int.setBytes(chunk)
			if afterDecimal {
				decimalCount++
			}
		default:
			log.Error("Bad Char")
		}
	}
	msg.value = msg.value / math.Pow(10, decimalCount)
	return msg
}

func splitMessage(rawMessage []byte, rawLength int) []Msg {
	messages := make([]Msg, n/12)
	for l := 0; l < rawlength; l++ {
		chunk := rawMessage[l]
		if chunk == 'a' {
			messages.append(breakoutMessage(rawMessage[l : l+11]))
		} else {
			log.Error("This looks bad")
		}
	}
	return messages
}

func main() {
	c := &serial.Config{Name: "/dev/ttyACM0", Baud: 9600}
	s, err := serial.OpenPort(c)
	if err != nil {
		log.Fatal(err)
	}
	buf := make([]byte, 128)
	n, err := s.Read(buf)
	if err != nil {
		log.Fatal(err)
	}
	msg := splitMessage(buf, n)
	log.Printf("%q", buf[:n])
	log.Printf("%q", msg)

}
