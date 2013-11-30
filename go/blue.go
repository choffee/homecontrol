package main

import (
  "os/exec"
  "log"
  "regexp"
)

func main() {
  var connected = regexp.MustCompile("^[0-9A-Z:]+ connected with level ([0-9]+)")
  out, err := exec.Command("bluemon-query").Output()
  if err != nil {
    // It errors out each time
    if err.Error() == "exit status 1" {
      log.Print("err")
    } else {
      log.Fatal(err)
    }
  }
  log.Printf("%s\n", out)
  level := connected.FindSubmatch(out)
  if len(level) == 2 {
    log.Printf("%s\n", level[1])
  } else {
    log.Print("Not Connected")
  }
}
