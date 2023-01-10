#ifndef WIFI_SETTINGS_H
#define WIFI_SETTINGS_H

// WiFi
const char *ssid = "WIFI-SSID";
const char *password = "WIFI-PASSWORD";

// ThingSpeak
unsigned long myChannelNumber = 0;
const char *myReadAPIKey = "TOKEN";
const char *myWriteAPIKey = "TOKEN";

// Email
#define SMTP_HOST "smtp.gmail.com"
#define SMTP_PORT esp_mail_smtp_port_587
#define AUTHOR_NAME "ESP32"
#define AUTHOR_EMAIL "my_name@gmail.com"
#define AUTHOR_PASSWORD                                                        \
  "Google App password, create one via myaccount.google.com/security"
#define RECIPIENT_EMAIL "my_name@gmail.com"
#define EMAIL_SUBJECT "[ALERT] ESP32 thermostat has reset"

#endif