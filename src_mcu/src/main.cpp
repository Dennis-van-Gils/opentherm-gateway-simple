/**
 * @file    main.cpp
 * @author  Dennis van Gils (vangils.dennis@gmail.com)
 * @version https://github.com/Dennis-van-Gils/opentherm-gateway-simple
 * @date    05-01-2025
 *
 * @copyright MIT License. See the LICENSE file for details.
 */

#include <Arduino.h>
#include <AsyncTCP.h>
#include <WiFi.h>
#include <esp_task_wdt.h>

#include "ESPAsyncWebServer.h"
#include "ESP_Mail_Client.h"
#include "ElegantOTA.h" // Serves a firmware update webpage at http://.../update
#include "OpenTherm.h"
#include "PubSubClient.h"

#include "webcontent_css.h"
#include "webcontent_favicon.h"
#include "webcontent_index.h"
#include "webcontent_js.h"

#if __has_include("wifi_settings.h")
#include "wifi_settings.h"
#else
#include "wifi_settings_template.h"
#endif

// Print debug info over Serial?
#define DEBUG 0

// Common character array used by many functions
const uint16_t BUF_LEN = 64;
char buf[BUF_LEN] = {'\0'};

// Webpage
extern const char index_html[];
extern const char css[];
extern const char js[];
extern const int favicon_ico_length;
extern const byte favicon_ico[];

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
WiFiClient wifi_client;
PubSubClient mqtt_client(MQTT_BROKER, MQTT_PORT, wifi_client);

// OpenTherm shields
const int PIN_MASTER_IN = 21;  // 2: Arduino,  4: ESP8266 (D2), 21: ESP32
const int PIN_MASTER_OUT = 22; // 4: Arduino,  5: ESP8266 (D1), 22: ESP32
const int PIN_SLAVE_IN = 19;   // 3: Arduino, 12: ESP8266 (D6), 19: ESP32
const int PIN_SLAVE_OUT = 23;  // 5: Arduino, 13: ESP8266 (D7), 23: ESP32

OpenTherm mOT(PIN_MASTER_IN, PIN_MASTER_OUT);
OpenTherm sOT(PIN_SLAVE_IN, PIN_SLAVE_OUT, true);
void IRAM_ATTR mHandleInterrupt() { mOT.handleInterrupt(); }
void IRAM_ATTR sHandleInterrupt() { sOT.handleInterrupt(); }

// Watchdog timer
const uint32_t WDT_TIMEOUT = 30000;     // Timeout [ms]
const uint32_t WDT_RESET_PERIOD = 5000; // Slow down reset on purpose [ms]

// Boiler communication check
// --------------------------
// Check for any WRITE_ACK messages coming back from the boiler.
// If none are received within the set timeout window, the communication with
// the boiler has probably failed. In that case: Notify the user by email and
// reset the microcontroller.
const uint32_t WRITE_ACK_TIMEOUT = 30000; // Timeout [ms]
uint32_t tick_WRITE_ACK = millis();       // Time of last WRITE_ACK
bool trigger_reset = false;               // When true will reset the MCU
SMTPSession smtp;                         // To allow sending email

/*------------------------------------------------------------------------------
  mqtt_reconnect
------------------------------------------------------------------------------*/

void mqtt_reconnect() {
#if DEBUG
  Serial.print("Connecting to MQTT broker: ");
#endif

  if (!mqtt_client.connect("OpenTherm", MQTT_USERNAME, MQTT_PASSWORD)) {
#if DEBUG
    Serial.println("FAILED!");
#endif
  } else {
#if DEBUG
    Serial.println("Succes.");
#endif
  }
}

/*------------------------------------------------------------------------------
  Text formatters
------------------------------------------------------------------------------*/

/**
 * Left-aligned float number `value` using two digits of precision.
 * Output looks like "12.34".
 */
void formattedFloat(char *buf, uint16_t buf_len, float value) {
  dtostrf(value, -(buf_len - 1), 2, buf);
}

/**
 * Left-aligned boolean `value`.
 * Output looks like "0" or "1".
 */
void formattedBool(char *buf, uint16_t buf_len, bool value) {
  dtostrf(value, -1, 0, buf);
}

/*------------------------------------------------------------------------------
  sendEmail
------------------------------------------------------------------------------*/

/**
 * @brief Send an email. See `wifi_settings.h` for the SMTP mail server config.
 */
void sendEmail(MB_String recipient_email, MB_String subject,
               MB_String body_text) {
  ESP_Mail_Session session;
  session.server.host_name = SMTP_HOST;
  session.server.port = SMTP_PORT;
  session.login.email = AUTHOR_EMAIL;
  session.login.password = AUTHOR_PASSWORD;
  session.login.user_domain = F("mydomain.net");
  session.time.ntp_server = F("pool.ntp.org,time.nist.gov");
  session.time.gmt_offset = 3;
  session.time.day_light_offset = 0;

  SMTP_Message message;
  message.sender.name = AUTHOR_NAME;
  message.sender.email = AUTHOR_EMAIL;
  message.addRecipient("", recipient_email);
  message.subject = subject;
  message.text.content = body_text;
  message.priority = esp_mail_smtp_priority::esp_mail_smtp_priority_high;
  // message.addHeader(F("Message-ID: <abcde.fghij@gmail.com>"));

  if (!smtp.connect(&session))
    return;

  if (!MailClient.sendMail(&smtp, &message)) {
#if DEBUG
    Serial.print("Error sending email, ");
    Serial.println(smtp.errorReason());
#endif
  }
}

/*------------------------------------------------------------------------------
  sendToWebClients
------------------------------------------------------------------------------*/

/**
 * @brief Send a text message to all connected web clients.
 */
void sendToWebClients(const char *message) {
#if DEBUG
  Serial.println(message);
#endif
  ws.textAll(message);
}

/*------------------------------------------------------------------------------
  processRequest
------------------------------------------------------------------------------*/

/**
 * @brief Process the received request of the 'slave' OpenTherm shield, i.e. the
 * request coming from the master thermostat. Send this intercepted (and
 * potentially modified) thermostat request further downstream to the boiler and
 * listen for the boiler's response. Finally, send the boiler's response back to
 * the master thermostat.
 */
void processRequest(unsigned long sOT_request,
                    OpenThermResponseStatus sOT_status) {

  /*
  Background info:
    sOT: Instance linked to the 'slave' OpenTherm shield. It receives requests
         coming from the master thermostat and sends responses back to it.
    mOT: Instance linked to the 'master' OpenTherm shield. It sends requests to
         the slave boiler and receives responses coming back from it.

  Specific to my boiler Remeha Avanta 28c:
    UNKNOWN_DATA_ID, READ_DATA
    ID18: CHPressure   seldom
    ID26: Tdhw         seldom
    ID27: Toutside     every minute
    ID29: Tstorage     seldom
    ID35: ?            seldom
    ID36: ?            seldom
  */
  OpenThermMessageType sOT_msg_type = sOT.getMessageType(sOT_request);
  OpenThermMessageID sOT_msg_ID = sOT.getDataID(sOT_request);

  // Send the intercepted (and optionally modified) thermostat request further
  // downstream to the boiler and listen for the boiler's response.
  unsigned long mOT_response = mOT.sendRequest(sOT_request);
  OpenThermMessageType mOT_msg_type = mOT.getMessageType(mOT_response);

  if (mOT_response) {
    // A boiler response is waiting to be processed. Send this intercepted
    // response further upstream to the thermostat.
    sOT.sendResponse(mOT_response);

    // Boiler communication check
    if (mOT_msg_type == OpenThermMessageType::WRITE_ACK) {
      tick_WRITE_ACK = millis();
    }
  }

  // Inform the web clients about the thermostat `master` request
  snprintf(buf, BUF_LEN, "T%08x", sOT_request);
  sendToWebClients(buf);

  // Inform the web clients about the boiler 'slave' response
  snprintf(buf, BUF_LEN, "B%08x", mOT_response);
  sendToWebClients(buf);

  // Take action on the boiler's response
  if ((mOT_msg_type == OpenThermMessageType::READ_ACK) ||
      (mOT_msg_type == OpenThermMessageType::WRITE_ACK) ||
      (mOT_msg_type == OpenThermMessageType::DATA_INVALID)) {
    /*
    NOTE: We also accept DATA_INVALID, because e.g.
      request : WRITE_DATA   | 57 | MaxTSet | 90.00
      response: DATA_INVALID | 57 | MaxTSet | 75.00
    Meaning the thermostat requested a max boiler setpoint of 90'C, but was only
    rewarded 75'C by the boiler (hard-limited).
    */

    switch (sOT_msg_ID) {

    case OpenThermMessageID::Status:
      // 0: Status
      formattedBool(buf, BUF_LEN, mOT.isFault(mOT_response));
      mqtt_client.publish("OpenTherm/IsFault", buf);
      formattedBool(buf, BUF_LEN, mOT.isCentralHeatingActive(mOT_response));
      mqtt_client.publish("OpenTherm/IsCentralHeatingActive", buf);
      formattedBool(buf, BUF_LEN, mOT.isHotWaterActive(mOT_response));
      mqtt_client.publish("OpenTherm/IsHotWaterActive", buf);
      formattedBool(buf, BUF_LEN, mOT.isFlameOn(mOT_response));
      mqtt_client.publish("OpenTherm/IsFlameOn", buf);
      formattedBool(buf, BUF_LEN, mOT.isDiagnostic(mOT_response));
      mqtt_client.publish("OpenTherm/IsDiagnostic", buf);
      break;

    case OpenThermMessageID::TSet:
      // 1: Control setpoint i.e. CH water temperature setpoint (°C)
      formattedFloat(buf, BUF_LEN, mOT.getFloat(mOT_response));
      mqtt_client.publish("OpenTherm/TSet", buf);
      break;

    case OpenThermMessageID::MaxRelModLevelSetting:
      // 14: Maximum relative modulation level setting (%)
      formattedFloat(buf, BUF_LEN, mOT.getFloat(mOT_response));
      mqtt_client.publish("OpenTherm/MaxRelModLevelSetting", buf);
      break;

    case OpenThermMessageID::TrSet:
      // 16: Room Setpoint (°C)
      formattedFloat(buf, BUF_LEN, mOT.getFloat(mOT_response));
      mqtt_client.publish("OpenTherm/TrSet", buf);
      break;

    case OpenThermMessageID::RelModLevel:
      // 17: Relative Modulation Level (%)
      formattedFloat(buf, BUF_LEN, mOT.getFloat(mOT_response));
      mqtt_client.publish("OpenTherm/RelModLevel", buf);
      break;

    case OpenThermMessageID::Tr:
      // 24: Room temperature (°C)
      formattedFloat(buf, BUF_LEN, mOT.getFloat(mOT_response));
      mqtt_client.publish("OpenTherm/Tr", buf);
      break;

    case OpenThermMessageID::Tboiler:
      // 25: Boiler supply water temperature (°C)
      formattedFloat(buf, BUF_LEN, mOT.getFloat(mOT_response));
      mqtt_client.publish("OpenTherm/Tboiler", buf);
      break;

    case OpenThermMessageID::Tdhw:
      // 26: DHW temperature (°C)
      formattedFloat(buf, BUF_LEN, mOT.getFloat(mOT_response));
      mqtt_client.publish("OpenTherm/Tdhw", buf);
      break;

    case OpenThermMessageID::Tret:
      // 28: Boiler return water temperature (°C)
      formattedFloat(buf, BUF_LEN, mOT.getFloat(mOT_response));
      mqtt_client.publish("OpenTherm/Tret", buf);
      break;

    case OpenThermMessageID::TdhwSet:
      // 56: DHW setpoint (°C)
      formattedFloat(buf, BUF_LEN, mOT.getFloat(mOT_response));
      mqtt_client.publish("OpenTherm/TdhwSet", buf);
      break;

    case OpenThermMessageID::MaxTSet:
      // 57: Max CH water setpoint (°C)
      formattedFloat(buf, BUF_LEN, mOT.getFloat(mOT_response));
      mqtt_client.publish("OpenTherm/MaxTSet", buf);
      break;
    }
  }
}

/*------------------------------------------------------------------------------
  Webpage functions
------------------------------------------------------------------------------*/

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo *)arg;
  if (info->final && info->index == 0 && info->len == len &&
      info->opcode == WS_TEXT) {
    data[len] = 0;
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
             AwsEventType type, void *arg, uint8_t *data, size_t len) {
  switch (type) {
  case WS_EVT_CONNECT:
#if DEBUG
    Serial.printf("WebSocket client #%u connected from %s\n", client->id(),
                  client->remoteIP().toString().c_str());
#endif
    break;
  case WS_EVT_DISCONNECT:
#if DEBUG
    Serial.printf("WebSocket client #%u disconnected\n", client->id());
#endif
    break;
  case WS_EVT_DATA:
    handleWebSocketMessage(arg, data, len);
    break;
  case WS_EVT_PONG:
  case WS_EVT_ERROR:
    break;
  }
}

void notFound(AsyncWebServerRequest *request) {
  snprintf(buf, BUF_LEN, "%s", "Not found, ");
  // Will truncate an excessively long url
  request->url().toCharArray(&buf[11], BUF_LEN - 12);
#if DEBUG
  Serial.println(buf);
#endif
  request->send(404, "text/plain", buf);
}

String htmlVarProcessor(const String &var) {
#if DEBUG
  Serial.print("Token received for htmlVarProcessor: ");
  Serial.println(var);
#endif

  if (var == "IP_ADDR")
    return WiFi.localIP().toString();

  return String();
}

/*------------------------------------------------------------------------------
  setup
------------------------------------------------------------------------------*/

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

#if DEBUG
  Serial.begin(9600);
  Serial.print("\nObtaining IP address: ");
#endif

  // WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
#if DEBUG
    Serial.println("FAILED!");
#endif
    return;
  }

#if DEBUG
  Serial.println(WiFi.localIP());
#endif

  // MQTT
  mqtt_reconnect();

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html, htmlVarProcessor);
  });

  server.on("/otgw-core.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    AsyncWebServerResponse *response =
        request->beginResponse_P(200, "text/javascript", js);
    response->addHeader("cache-control", "max-age=7776000");
    request->send(response);
  });

  server.on("/styles.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    AsyncWebServerResponse *response =
        request->beginResponse_P(200, "text/css", css);
    response->addHeader("cache-control", "max-age=7776000");
    request->send(response);
  });

  server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request) {
    AsyncWebServerResponse *response = request->beginResponse_P(
        200, "image/x-icon", favicon_ico, favicon_ico_length);
    response->addHeader("cache-control", "max-age=7776000");
    request->send(response);
  });

  server.on("/trigger_reset", HTTP_GET, [](AsyncWebServerRequest *request) {
    snprintf(buf, BUF_LEN, "Force triggering a reset in %u seconds",
             WDT_TIMEOUT / 1000);
    request->send(200, "text/plain", buf);
    trigger_reset = true;
  });

  // Over-the-air updates
  ElegantOTA.begin(&server);

  // Webpage
  ws.onEvent(onEvent);
  server.addHandler(&ws);
  server.onNotFound(notFound);
  server.begin();

  // Email notification
  MailClient.networkReconnect(true);
  smtp.debug(0);

  // OpenTherm shields
  mOT.begin(mHandleInterrupt);
  sOT.begin(sHandleInterrupt, processRequest);

  // Deliberately slow down end of setup
  delay(3000);
  digitalWrite(LED_BUILTIN, LOW);

  // Start watchdog timer
  esp_task_wdt_init(WDT_TIMEOUT, true);
  esp_task_wdt_add(NULL);
}

/*------------------------------------------------------------------------------
  loop
------------------------------------------------------------------------------*/

void loop() {
  uint32_t now = millis();
  static uint32_t tick_WDT_reset = now;
  static uint32_t tick_ESP_restart = now;
  static uint32_t tick_alive_blinker = now;
  static bool waiting_for_ESP_restart = false;

  // Periodically reset the watchdog timer
  if (now - tick_WDT_reset > WDT_RESET_PERIOD) {
    tick_WDT_reset = now;
    esp_task_wdt_reset();

    snprintf(buf, BUF_LEN, "Free heap: %u, RRSI: %d dB", ESP.getFreeHeap(),
             WiFi.RSSI());
    sendToWebClients(buf);
  }

  if (waiting_for_ESP_restart) {
    // Wait a long time before restarting; prevents fast-cycling restarts.
    // While we wait, still execute `loop()` to allow any websocket and
    // OpenTherm communication.
    if (now - tick_ESP_restart > WDT_TIMEOUT) {
      ESP.restart();
    }
  } else {
    // Boiler communication check
    if (now - tick_WRITE_ACK > WRITE_ACK_TIMEOUT) {
      trigger_reset = true;
    }
    if (trigger_reset) {
      snprintf(buf, BUF_LEN, "!! RESET TRIGGERED, %u", now - tick_WRITE_ACK);
      sendToWebClients(buf);
      sendEmail(F("vangils.dennis@gmail.com"),
                F("[ALERT] OpenThermGateway has triggered an ESP32 reset"),
                F(""));

      waiting_for_ESP_restart = true;
      tick_ESP_restart = now;
    }
  }

  sOT.process();
  ws.cleanupClients();

  // MQTT
  if (!mqtt_client.connected()) {
    mqtt_reconnect();
  }
  mqtt_client.loop();

  // Over-the-air updates
  ElegantOTA.loop();

  // Alive blinker
  if (now - tick_alive_blinker >= 500) {
    digitalWrite(LED_BUILTIN, HIGH);
  }
  if (now - tick_alive_blinker >= 1000) {
    tick_alive_blinker = now;
    digitalWrite(LED_BUILTIN, LOW);
  }

  delay(10); // Necessary idle time to allow the ESP task manager to run
}
