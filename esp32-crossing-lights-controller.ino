#include <Arduino.h>
#include <ArduinoJson.h>
#include <EspMQTTClient.h>
#include "config.h"

EspMQTTClient client;
String *ipAdress;
String *password;
String *signalTopic;
String *username;
String *wifiPassword;
String *wifiSsid;
bool flashing = false;

const uint8_t evenPins[2] = {D6, D8};
const uint8_t oddPins[2] = {D5, D7};

void setup()
{
#if ARDUHAL_LOG_LEVEL >= ARDUHAL_LOG_LEVEL_ERROR
    Serial.begin(115200);
#endif
    log_d("Initialising config document");
    DynamicJsonDocument config(4096);
    loadConfig(config);

    log_d("Loading parameters");
    ipAdress = new String((const char *)config["server"]["ipAddress"]);
    password = new String((const char *)config["auth"]["password"]);
    signalTopic = new String((const char *)config["signalTopic"]);
    username = new String((const char *)config["auth"]["username"]);
    wifiPassword = new String((const char *)config["wifi"]["password"]);
    wifiSsid = new String((const char *)config["wifi"]["ssid"]);
    short port = config["server"]["port"];

    log_d("Signal topic: %s", signalTopic->c_str());

#if ARDUHAL_LOG_LEVEL >= ARDUHAL_LOG_LEVEL_DEBUG
    client.enableDebuggingMessages();
#endif

    log_d("Connecting to WIFI SSID: %s", wifiSsid->c_str());
    client.setWifiCredentials(wifiSsid->c_str(), wifiPassword->c_str());

    log_d("Client name: %s", username->c_str());
    client.setMqttClientName(username->c_str());

    log_d("Connecting to the MQTT server: %s on port: %d", ipAdress->c_str(), port);
    client.setMqttServer(ipAdress->c_str(), username->c_str(), password->c_str(), port);
    log_i("Connected to the MQTT server");

    for (int i = 0; i < 2; i++)
    {
        pinMode(evenPins[i], OUTPUT);
        pinMode(oddPins[i], OUTPUT);
    }

    log_d("Setup complete");
}

void loop()
{
    client.loop();
    if (!flashing)
    {
        for (int i = 0; i < 2; i++)
        {
            digitalWrite(evenPins[i], LOW);
            digitalWrite(oddPins[i], LOW);
        }
        return;
    }

    unsigned long time = millis();
    time = time % 1000;
    if (time < 500)
    {
        for (int i = 0; i < 2; i++)
        {
            digitalWrite(evenPins[i], LOW);
            digitalWrite(oddPins[i], HIGH);
        }
    }
    else
    {
        for (int i = 0; i < 2; i++)
        {
            digitalWrite(evenPins[i], HIGH);
            digitalWrite(oddPins[i], LOW);
        }
    }
}

void onConnectionEstablished()
{
    log_d("Adding subscription to topic: %s", signalTopic->c_str());
    client.subscribe(*signalTopic, onReceive, 0);
}

void onReceive(const String &topicStr, const String &message)
{
    String content = message;
    content.toLowerCase();
    if (content.equals("on"))
    {
        flashing = true;
        log_d("Turning lights on");
    }
    else if (content.equals("off"))
    {
        flashing = false;
        log_d("Turning lights off");
    }
}
