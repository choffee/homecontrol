package main

import (
    "fmt"
    "io"
    "log"
    "regexp"

    "bytes"

    MQTT "git.eclipse.org/gitroot/paho/org.eclipse.paho.mqtt.golang.git"
    "github.com/tarm/goserial"
)

func sendPinPress(buf []byte, client *MQTT.MqttClient) {
    bufSplit := bytes.Split(buf, []byte(":"))
    if len(bufSplit) < 2 {
        log.Print(buf)
        return
    }
    pin := bufSplit[1]
    m := MQTT.NewMessage(pin)
    log.Printf("(%s) -> /remote/desk/events\n", m.Payload)
    _ = client.PublishMessage("/remote/desk/events", m)
}

func readRFData(buf []byte, client *MQTT.MqttClient) {
    readings := bytes.Split(buf, []byte(";"))
    sensor := bytes.Split(readings[0], []byte("="))[1]
    for l := 1; l < len(readings); l++ {
        readingpair := bytes.Split(readings[l], []byte("="))
        dev := readingpair[0]
        value := readingpair[1]
        location := fmt.Sprintf("/sensor/%s/%s", sensor, dev)
        m := MQTT.NewMessage(value)
        log.Printf("%s", m.Payload)
        _ = client.PublishMessage(location, m)
    }
}

func sendRFMessage(buf []byte, client *MQTT.MqttClient) {
    // RF Pipe:1 Data:RemoteID=0;Moisture_0=973
    bufSplit := bytes.Split(buf, []byte(" "))
    if len(bufSplit) < 2 {
        log.Print(buf)
        return
    }
    for l := 0; l < len(bufSplit); l++ {
        chunkSplit := bytes.Split(bufSplit[l], []byte(":"))
        switch string(chunkSplit[0]) {
        case "RF":
            continue
        case "Pipe":
            log.Printf("Pipe %s", chunkSplit[1])
        case "Data":
            log.Printf("Data - %s", chunkSplit[1])
            readRFData(chunkSplit[1], client)
        default:
            log.Print("Bad line")
        }
    }
}

func serialReader(serialPort *io.ReadWriteCloser, client *MQTT.MqttClient) {
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
            msg = bytes.TrimSpace(msg)
            log.Printf("%q", msg)
            if bytes.HasPrefix(msg, []byte("Pin:")) {
                sendPinPress(msg, client)
            }
            if bytes.HasPrefix(msg, []byte("RF ")) {
                sendRFMessage(msg, client)
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
    opts := MQTT.NewClientOptions()
    opts.SetBroker("tcp://localhost:1883")
    opts.SetClientId("powercontroller")
    opts.SetCleanSession(true)
    opts.SetTraceLevel(MQTT.Off)
    client := MQTT.NewClient(opts)
    _, err = client.Start()
    if err != nil {
        log.Fatal(err)
    } else {
        log.Printf("Connected as powercontroller to 127.0.0.1:1883")
    }

    go serialReader(&s, client)

    device_re, err := regexp.Compile(`/homeauto/power/([^/]+)`)
    if err != nil {
        log.Fatal(err)
    }

    powerFilter, err := MQTT.NewTopicFilter("/homeauto/power/+", 1)
    if err != nil {
        log.Fatal(err)
    }
    client.StartSubscription(func(client *MQTT.MqttClient, msg MQTT.Message) {
        payload := msg.Payload()
        topic := msg.Topic()
        log.Printf("Topic(%s),Payload(%s)\n", topic, payload)
        rec := device_re.FindStringSubmatch(topic)
        if len(rec) > 0 {
            // Check the length of rec here XXX
            device := rec[1]
            if string(payload) == "on" || string(payload) == "off" {
                msg := "RF  " + device + string(payload) + "\n"
                _, err = s.Write([]byte(msg))
                if err != nil {
                    log.Fatal(err)
                }
            }
        }
    }, powerFilter)

    if err != nil {
        log.Fatal(err)
    }

    // Just wait at the end
    end := make(chan bool, 1)
    <-end

}
