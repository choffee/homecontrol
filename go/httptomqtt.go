package main

// A simple daemon to bridge http to mqtt
// Send a request to url eg  /foo/bar
// Send some message as the body (probably json)
// Send the x-mqtt-auth header with username:pass
// This daemon with then send that write along to mqtt
// Give you a 200 if good else a corrosponding error

import (
    "net/http"
    "bitbucket.org/shanehanna/mosquitto"
    "bytes"
    "fmt"
    "log"
)


var conn, _ = mosquitto.Dial("homecontrol", "192.168.1.107:1883", true)

func rootHandler(w http.ResponseWriter, r *http.Request) {
    fmt.Fprint(w, "Sending Message")
    buf := new(bytes.Buffer)
    buf.ReadFrom(r.Body)
    b := buf.Bytes() // Should not have array of bytes
    message, _ := mosquitto.NewMessage(r.URL.Path, b)
    err := conn.Publish(message)
    if err != nil {
        log.Print(err)
    }
}

func main() {
    // Setup the mqtt server connection
    go conn.Listen()

    http.HandleFunc("/", rootHandler)
    log.Fatal(http.ListenAndServe(":8080", nil))
}

