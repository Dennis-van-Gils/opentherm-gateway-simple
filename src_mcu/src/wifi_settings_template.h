#ifndef WIFI_SETTINGS_H
#define WIFI_SETTINGS_H

// WiFi
const char WIFI_SSID[] = "WIFI-SSID";
const char WIFI_PASSWORD[] = "WIFI-PASSWORD";

// MQTT
const int MQTT_PORT = 1883;
const char MQTT_BROKER[] = "192.168.xxx.xxx";
const char MQTT_TOPIC[] = "OpenTherm";
const char MQTT_USERNAME[] = "username";
const char MQTT_PASSWORD[] = "password";

// Email
#define SMTP_HOST "smtp.gmail.com"
#define SMTP_PORT esp_mail_smtp_port_587
#define AUTHOR_NAME "OpenThermGateway"
#define AUTHOR_EMAIL "my_name@gmail.com"
#define AUTHOR_PASSWORD                                                        \
  "Google App password, create one via myaccount.google.com/security"

#endif