#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>

#include "credentials.h"

#define DEEP_SLEEP_ONE_SECOND 1000000

/**
 * MQTT
 */
#include <PubSubClient.h>

WiFiClient espClient;
PubSubClient client(espClient);

void mqttSetup () {
    client.setServer(mqtt_broker, mqtt_port);

    while (!client.connected()) {
      String client_id = "esp8266-client-";
      client_id += String(WiFi.macAddress());
      Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
      if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
          Serial.println("Public mqtt broker connected");
      } else {
          Serial.print("failed with state ");
          Serial.print(client.state());
          delay(1000);
      }
  }
}

/**
 * SHT30
 */
#include <Wire.h>
#include <WEMOS_SHT3X.h>

SHT3X sht30(0x45);

void readAndSendValues () {
    if (sht30.get() == 0) {
        char temperature[8];
        dtostrf(sht30.cTemp, 6, 2, temperature);
        client.publish(topic_temperature, temperature);

        char humidity[8];
        dtostrf(sht30.humidity, 6, 2, humidity);
        client.publish(topic_humidity, humidity);

        Serial.printf("Values sent %.2f %.2f \n", sht30.cTemp, sht30.humidity);
        delay(100); // let mqtt publish finish its work
    }
}
 
void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.println("");
 
    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    
    mqttSetup();

    readAndSendValues();

    Serial.println("Enter to deep sleep.");
    ESP.deepSleep(60 * DEEP_SLEEP_ONE_SECOND, WAKE_RFCAL);
}
void loop() {
  // put your main code here, to run repeatedly:
}