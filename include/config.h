#pragma once

// Device identity — must match the AWS IoT Thing name (see terraform/)
#define THING_NAME "esp32-telemetry-01"

// MQTT topic the device publishes telemetry to
#define TELEMETRY_TOPIC "devices/" THING_NAME "/telemetry"

// Publish interval in milliseconds
#define TELEMETRY_INTERVAL_MS 30000UL

// AWS IoT Core uses MQTT over TLS on 8883
#define MQTT_PORT 8883
