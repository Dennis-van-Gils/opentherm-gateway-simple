#ifndef WIFI_SETTINGS_H
#define WIFI_SETTINGS_H

// WiFi
const char *WIFI_SSID = "WIFI-SSID";
const char *WIFI_PASSWORD = "WIFI-PASSWORD";

// ThingSpeak
unsigned long THINGSPEAK_CHANNEL = 0;
const char *THINGSPEAK_READ_API_KEY = "TOKEN";
const char *THINGSPEAK_WRITE_API_KEY = "TOKEN";

// Email
#define SMTP_HOST "smtp.gmail.com"
#define SMTP_PORT esp_mail_smtp_port_587
#define AUTHOR_NAME "OpenThermGateway"
#define AUTHOR_EMAIL "my_name@gmail.com"
#define AUTHOR_PASSWORD                                                        \
  "Google App password, create one via myaccount.google.com/security"

#endif