/**
 * @file    main.cpp
 * @author  Dennis van Gils (vangils.dennis@gmail.com)
 * @version https://github.com/Dennis-van-Gils/opentherm-gateway-simple
 * @date    02-01-2025
 *
 * @copyright MIT License. See the LICENSE file for details.
 */

#include <Arduino.h>
#include <AsyncTCP.h>
#include <WiFi.h>
#include <esp_task_wdt.h>

#include "ESPAsyncWebServer.h"
#include "ESP_Mail_Client.h"
#include "OpenTherm.h"
#include "ThingSpeak.h"

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
WiFiClient client;

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

// ThingSpeak values to track
bool _IsFlameOn = false;
float _TSet = 0.;
float _TrSet = 0.;
float _RelModLevel = 0.;
float _Tr = 0.;
float _Tboiler = 0.;

/*------------------------------------------------------------------------------
  formattedTextFloat
------------------------------------------------------------------------------*/

/**
 * @brief Adjust the passed character array `buf` of maximum length `buf_len` to
 * contain a string starting with passed character array `text` and appended
 * with a left-aligned float number `value` using two digits of precision.
 * Output looks like "text12.34".
 */
void formattedTextFloat(char *buf, uint16_t buf_len, const char *text,
                        float value) {
  const uint8_t FLOAT_BUF_LEN = 10;
  char float_buf[FLOAT_BUF_LEN] = {'\0'};

  dtostrf(value, -(FLOAT_BUF_LEN - 1), 2, float_buf);
  snprintf(buf, buf_len, "%s%s", text, float_buf);
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
  float value;

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
      _IsFlameOn = mOT.isFlameOn(mOT_response);
      break;

    case OpenThermMessageID::TSet:
      // 1: Control setpoint i.e. CH water temperature setpoint (°C)
      _TSet = mOT.getFloat(mOT_response);
      formattedTextFloat(buf, BUF_LEN, "A:", _TSet);
      sendToWebClients(buf);
      break;

    case OpenThermMessageID::TrSet:
      // 16: Room Setpoint (°C)
      _TrSet = mOT.getFloat(mOT_response);
      formattedTextFloat(buf, BUF_LEN, "E:", _TrSet);
      sendToWebClients(buf);
      break;

    case OpenThermMessageID::RelModLevel:
      // 17: Relative Modulation Level (%)
      _RelModLevel = mOT.getFloat(mOT_response);
      break;

    case OpenThermMessageID::Tr:
      // 24: Room temperature (°C)
      _Tr = mOT.getFloat(mOT_response);
      formattedTextFloat(buf, BUF_LEN, "F:", _Tr);
      sendToWebClients(buf);
      break;

    case OpenThermMessageID::Tboiler:
      // 25: Boiler flow water temperature (°C)
      _Tboiler = mOT.getFloat(mOT_response);
      formattedTextFloat(buf, BUF_LEN, "B:", _Tboiler);
      sendToWebClients(buf);
      break;

    case OpenThermMessageID::Tdhw:
      // 26: DHW temperature (°C)
      value = mOT.getFloat(mOT_response);
      formattedTextFloat(buf, BUF_LEN, "D:", value);
      sendToWebClients(buf);
      break;

    case OpenThermMessageID::TdhwSet:
      // 56: DHW setpoint (°C)
      value = mOT.getFloat(mOT_response);
      formattedTextFloat(buf, BUF_LEN, "C:", value);
      sendToWebClients(buf);
      break;

    case OpenThermMessageID::MaxTSet:
      // 57: Max CH water setpoint (°C)
      value = mOT.getFloat(mOT_response);
      formattedTextFloat(buf, BUF_LEN, "G:", value);
      sendToWebClients(buf);
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

  if (var == "READ_TOKEN")
    return THINGSPEAK_READ_API_KEY;

  if (var == "CHANNEL_ID")
    return String(THINGSPEAK_CHANNEL);

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

  ws.onEvent(onEvent);
  server.addHandler(&ws);
  server.onNotFound(notFound);
  server.begin();

  // Email notification
  MailClient.networkReconnect(true);
  smtp.debug(0);

  // ThingSpeak
  ThingSpeak.begin(client);

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
  static uint32_t tick_ThingSpeak_update = now;
  static bool waiting_for_ESP_restart = false;

  // Periodically reset the watchdog timer
  if (now - tick_WDT_reset > WDT_RESET_PERIOD) {
    tick_WDT_reset = now;
    esp_task_wdt_reset();

    snprintf(buf, BUF_LEN, "FH        %u", ESP.getFreeHeap());
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

  // ThingSpeak
  if (tick_ThingSpeak_update < millis()) {
    tick_ThingSpeak_update = millis() + 20000;

    if (THINGSPEAK_CHANNEL == 0 || THINGSPEAK_READ_API_KEY == "TOKEN" ||
        THINGSPEAK_WRITE_API_KEY == "TOKEN")
      return;

    // DEV-NOTE: It is tempting to place these calls inside `processRequest()`
    // but doing so results in NULL values for some of the set fields. Perhaps
    // a bug in the ThingSpeak library where data of each field gets set back to
    // NULL in between `ThingSpeak.writeFields()` calls? Hence, we always set
    // all fields every `writeFields()`.
    ThingSpeak.setField(1, _Tboiler);
    ThingSpeak.setField(2, _TSet);
    ThingSpeak.setField(3, _RelModLevel);
    ThingSpeak.setField(4, _IsFlameOn);
    ThingSpeak.setField(5, _TrSet);
    ThingSpeak.setField(6, _Tr * 100.f);

    int ans =
        ThingSpeak.writeFields(THINGSPEAK_CHANNEL, THINGSPEAK_WRITE_API_KEY);
#if DEBUG
    if (ans == 200) {
      Serial.println("ThingSpeak channel update successful.");
    } else {
      Serial.print("Problem updating ThingSpeak channel. HTTP error code: ");
      Serial.println(ans);
    }
#endif
  }

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
