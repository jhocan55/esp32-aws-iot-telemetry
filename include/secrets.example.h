#pragma once

// Copy this file to include/secrets.h and fill in your values.
// secrets.h is gitignored — never commit real credentials or certificates.

#define WIFI_SSID "your-wifi-ssid"
#define WIFI_PASSWORD "your-wifi-password"

// Your account-specific endpoint:
//   aws iot describe-endpoint --endpoint-type iot:Data-ATS --query endpointAddress --output text
#define AWS_IOT_ENDPOINT "xxxxxxxxxxxxxx-ats.iot.eu-west-1.amazonaws.com"

// Amazon Root CA 1 — https://www.amazontrust.com/repository/AmazonRootCA1.pem
static const char AWS_ROOT_CA[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
...paste Amazon Root CA 1 here...
-----END CERTIFICATE-----
)EOF";

// Device certificate — created when you register the Thing (see terraform/README)
static const char DEVICE_CERT[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
...paste device certificate here...
-----END CERTIFICATE-----
)EOF";

// Device private key — download at certificate creation time; AWS never shows it again
static const char DEVICE_PRIVATE_KEY[] PROGMEM = R"EOF(
-----BEGIN RSA PRIVATE KEY-----
...paste device private key here...
-----END RSA PRIVATE KEY-----
)EOF";
