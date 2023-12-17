# ESP32 Cross Lights Controller

The program is designed to run on a Seeed XIAO ESP32C3 to control model railway crossing lights based on events published to an MQTT topic. This uses the [EspMQTTClient](https://github.com/plapointe6/EspMQTTClient) library.

## Configuration

JSON file, written to the SPIFFS, needs to define the following:

```JSON
{
  "auth": {
    "username": "username",
    "password": "password"
  },
  "server": {
    "ipAddress": "192.168.1.2",
    "port": 5672
  },
  "signalTopic": "topic/signal",
  "wifi": {
    "ssid": "Some-WIFI-SSID",
    "password": "wifi-password"
  }
}
```

## Signal lights control

The device controls 2 pairs of LEDs with a common anode pin. The device has to sink current to do this. The cathodes have to be connected to the following pins via a ~ 100&#x03A9; resistor.

| LED Pair | Odd Pin | Even Pin |
| -------- | ------- | -------- |
| 0        | D5      | D6       |
| 1        | D7      | D8       |

The content of the MQTT messages should be a ASCII "ON" or "OFF". The program is not case sensitive. Both pairs of LEDs are controlled by the same state. However, whilst flashing the odd pin will be pulled high at a different time to the even pin.
