// ESP32 telemetry node — publishes device health metrics to AWS IoT Core
// over MQTT/TLS. Requires no external sensors: it reports uptime, free heap,
// and Wi-Fi RSSI, so any bare ESP32 devkit can run it.

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#include "config.h"
#include "secrets.h"

static WiFiClientSecure tlsClient;
static PubSubClient mqtt(tlsClient);
static unsigned long lastPublishMs = 0;

static void connectWifi() {
  if (WiFi.status() == WL_CONNECTED) return;
  Serial.printf("Connecting to Wi-Fi '%s'", WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print('.');
  }
  Serial.printf("\nWi-Fi connected, IP %s\n", WiFi.localIP().toString().c_str());
}

static void connectMqtt() {
  while (!mqtt.connected()) {
    Serial.printf("Connecting to AWS IoT as '%s'... ", THING_NAME);
    if (mqtt.connect(THING_NAME)) {
      Serial.println("connected");
    } else {
      // rc meanings: https://pubsubclient.knolleary.net/api#state
      Serial.printf("failed, rc=%d — retrying in 5s\n", mqtt.state());
      delay(5000);
    }
  }
}

static void publishTelemetry() {
  JsonDocument doc;
  doc["device"] = THING_NAME;
  doc["uptime_s"] = millis() / 1000;
  doc["free_heap_bytes"] = ESP.getFreeHeap();
  doc["wifi_rssi_dbm"] = WiFi.RSSI();

  char payload[192];
  serializeJson(doc, payload, sizeof(payload));

  if (mqtt.publish(TELEMETRY_TOPIC, payload)) {
    Serial.printf("Published to %s: %s\n", TELEMETRY_TOPIC, payload);
  } else {
    Serial.println("Publish failed");
  }
}

void setup() {
  Serial.begin(115200);
  connectWifi();

  tlsClient.setCACert(AWS_ROOT_CA);
  tlsClient.setCertificate(DEVICE_CERT);
  tlsClient.setPrivateKey(DEVICE_PRIVATE_KEY);
  mqtt.setServer(AWS_IOT_ENDPOINT, MQTT_PORT);

  connectMqtt();
}

void loop() {
  connectWifi();
  connectMqtt();
  mqtt.loop();

  const unsigned long now = millis();
  if (now - lastPublishMs >= TELEMETRY_INTERVAL_MS || lastPublishMs == 0) {
    lastPublishMs = now;
    publishTelemetry();
  }
}
