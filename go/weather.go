package main

// Some of the openweathermap api

import (
  "net/http"
  "log"
  "encoding/json"
  "io/ioutil"
)

  // Example json
  // {"coord":{"lon":-1.08271,"lat":53.95763},
  //  "sys":{"country":"GB","sunrise":1381645783,"sunset":1381684265},
  //  "weather":[{"id":500,"main":"Rain","description":"light rain","icon":"10n"}],
  //  "base":"global stations",
  //  "main":{"temp":9.98,"pressure":1010,"temp_min":8.89,"temp_max":12,"humidity":91},
  //  "wind":{"speed":5.65,"gust":10.79,"deg":84},
  //  "rain":{"3h":0},
  //  "clouds":{"all":44},
  //  "dt":1381694298,
  //  "id":2633352,
  //  "name":"York",
  //  "cod":200}

type Coord struct {
  Lon float64 `json: "lon"`
  Lat float64 `json: "lat"`
}

type Sys struct {
  Country string `json: "country"`
  Sunrise int64  `json: "sunrise"`
  Sunset int64   `json: "sunset"`
}

type Weather struct {
  Id int `json: "id"`
  Main string `json: "main"`
  Description string `json: "description"`
  Icon string `json: "10n"`
}

type Main struct {
  //  "main":{"temp":9.98,"pressure":1010,"temp_min":8.89,"temp_max":12,"humidity":91},
  Temp float32 `json: "temp"`
  Pressure int `json: "pressure"`
  TempMin float32 `json: "temp_min"`
  TempMax float32 `json: "temp_max"`
  Humidity int `json: "humidity"`
}

type Wind struct {
  //  "wind":{"speed":5.65,"gust":10.79,"deg":84},
  Speed float32 `json: "speed"`
  Gust float32 `json: "gust"`
  Degree int `json: "deg"`
}

type Rain struct {
  Hour3 int `json: "3h"`
}
type Clouds struct {
  All int `json: "all"`
}

type WeatherData struct {
  Coord Coord `json: "coord"`
  Sys Sys `json: "sys"`
  Weather []Weather `json: "weather"`
  Base string `json: "base"`
  Main Main `json: "main"`
  Wind Wind `json: "wind"`
  Rain Rain `json: "rain"`
  Clouds Clouds `json: "clouds"`
  Dt int `json: "dt"`
  Id int `json: "id"`
  Name string `json: "name"`
  Cod int `json: "cod"`
}

func main() {
  client := &http.Client{}

  req, err := http.NewRequest("GET", "http://api.openweathermap.org/data/2.5/weather?q=York,uk&units=metric", nil)
  if err != nil {
    log.Fatal(err)
  }
  req.Header.Add("x-api-key","9b6c67c4d9a7153ed0fdfb1e4e9aeb9e")
  resp, err := client.Do(req)
  defer resp.Body.Close()
  if resp.StatusCode == 200 {
    var weather WeatherData
    body, err := ioutil.ReadAll(resp.Body)
    if err != nil {
      log.Fatal(err)
    }
    err = json.Unmarshal(body, &weather)
    if err != nil {
      log.Fatal(err)
    }
    log.Printf("%s: %.2f", weather.Name, weather.Main.Temp)
  }
}
