/*
DEV NOTE: Use `unsigned long` instead of `uint32_t`.
  https://github.com/espressif/arduino-esp32/issues/384
  "So the only way to allow libraries to be portable between ESP32 Arduino core
  and other cores is to use unsigned long for the return value. Yes, this is
  ambiguous (there could be a platform with 64-bit long), but language-wise this
  is the only way to remain compatible with other cores."
*/

#include <Arduino.h>
#ifdef ESP32
#  include "AsyncTCP.h"
#  include <WiFi.h>
#  include <esp_task_wdt.h>
#elif defined(ESP8266)
#  include "ESPAsyncTCP.h"
#  include <ESP8266WiFi.h>
#endif
#include "ESPAsyncWebServer.h"
#include "ESP_Mail_Client.h"
#include "OpenTherm.h"
#include "ThingSpeak.h"

// Web content
#include "webcontent_css.h"
#include "webcontent_favicon.h"
#include "webcontent_index.h"
#include "webcontent_js.h"

// API settings
#if __has_include("wifi_settings.h")
#  include "wifi_settings.h"
#else
#  include "wifi_settings_template.h"
#endif

// Enable OTA updates
#include "AsyncElegantOTA.h"

// Allow the user to force-disable the CH and DHW modes via the web interface?
// Comment out to disable
// #define ALLOW_USER_FORCE_DISABLE_CH_DHW

// Print debug info over Serial?
// Comment out to disable
// #define DEBUG

// Common character array, used by many functions.
const uint16_t CHAR_BUFFER_LEN = 64;
char char_buffer[CHAR_BUFFER_LEN] = {'\0'};

// Print a float value left-aligned to character array `buffer` using two digits
// of precision, prepended by character array `text`. Output looks like
// "text12.34".
void build_notify_msg(char *buffer, uint16_t buffer_len, const char *text,
                      float value) {
  const uint8_t FLOAT_BUFFER_LEN = 10;
  char float_buffer[FLOAT_BUFFER_LEN] = {'\0'};

  dtostrf(value, -(FLOAT_BUFFER_LEN - 1), 2, float_buffer);
  snprintf(buffer, buffer_len, "%s%s", text, float_buffer);
}

// HW settings
const int mInPin = 21;  // 2 for Arduino, 4 for ESP8266 (D2), 21 for ESP32
const int mOutPin = 22; // 4 for Arduino, 5 for ESP8266 (D1), 22 for ESP32
const int sInPin = 19;  // 3 for Arduino, 12 for ESP8266 (D6), 19 for ESP32
const int sOutPin = 23; // 5 for Arduino, 13 for ESP8266 (D7), 23 for ESP32

extern const char index_html[];
extern const char css[];
extern const char js[];
extern const int favicon_ico_length;
extern const byte favicon_ico[];

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
WiFiClient client;

OpenTherm mOT(mInPin, mOutPin);
OpenTherm sOT(sInPin, sOutPin, true);
void ICACHE_RAM_ATTR mHandleInterrupt() { mOT.handleInterrupt(); }
void ICACHE_RAM_ATTR sHandleInterrupt() { sOT.handleInterrupt(); }

// clang-format off
int _thingSpeakUpd = 0;
unsigned long _lastRresponse;
bool  _IsFlameOn = false;
float _RelModLevel = 0;
float _TSet = 0;
bool  _TSet_notify = false;
float _Tboiler = 0;
bool  _Tboiler_notify = false;
float _TdhwSet = 0;
bool  _TdhwSet_notify = false;
float _Tdhw = 0;
bool  _Tdhw_notify = false;
float _TrSet = 0;
bool  _TrSet_notify = false;
float _Tr = 0;
bool  _Tr_notify = false;
#ifdef ALLOW_USER_FORCE_DISABLE_CH_DHW
bool  _heating_disable = false;
bool  _dhw_disable = false;
#endif
// clang-format on

// Watchdog timer
const unsigned long WDT_TIMEOUT = 30000;     // Timeout [ms]
const unsigned long WDT_RESET_PERIOD = 1000; // Slow down reset on purpose [ms]
unsigned long t_WDT_reset = millis();        // Time of last watchdog reset

// Boiler communication check
// --------------------------
// Check for any WRITE_ACK messages coming back from the boiler.
// If none are received within the set timeout window, the communication with
// the boiler has probably failed. In that case: Notify the user by email and
// restart the microcontroller by triggering the watchdog timer.
const unsigned long ACK_TIMEOUT = 30000; // Timeout [ms]
unsigned long t_ACK = millis();          // Time of last received WRITE_ACK msg
bool ACK_timeout_notify = false;         // True when timed out
bool force_trigger_ACK_timeout = false;  // DEBUG: Useful for testing
SMTPSession smtp;                        // To allow sending email

void sendAlertEmail() {
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
  message.subject = EMAIL_SUBJECT;
  message.addRecipient("", RECIPIENT_EMAIL);
  message.text.content = "";
  message.priority = esp_mail_smtp_priority::esp_mail_smtp_priority_high;
  // message.addHeader(F("Message-ID: <abcde.fghij@gmail.com>"));

  if (!smtp.connect(&session))
    return;

  if (!MailClient.sendMail(&smtp, &message)) {
#ifdef DEBUG
    Serial.print("Error sending email, ");
    Serial.println(smtp.errorReason());
#endif
  }
}

void notifyClients(const char *s) { ws.textAll(s); }

void processRequest(unsigned long request, OpenThermResponseStatus status) {
  OpenThermMessageType msgType = mOT.getMessageType(request);
  OpenThermMessageID dataId = mOT.getDataID(request);

#ifdef ALLOW_USER_FORCE_DISABLE_CH_DHW
  // Potentially modify thermostat request
  if (msgType == OpenThermMessageType::READ_DATA &&
      dataId == OpenThermMessageID::Status) {
    if (_heating_disable) {
#  ifdef DEBUG
      Serial.println("Disable Heating");
#  endif
      request &= ~(1ul << (0 + 8));
    }
    if (_dhw_disable) {
#  ifdef DEBUG
      Serial.println("Disable DHW");
#  endif
      request &= ~(1ul << (1 + 8));
    }
    request &= ~(1ul << 31);
    if (mOT.parity(request))
      request |= (1ul << 31);
  }
#endif

  _lastRresponse = mOT.sendRequest(request);
  sOT.sendResponse(_lastRresponse);

  // Boiler communication check
  OpenThermMessageType msgTypeBoiler = mOT.getMessageType(_lastRresponse);
  if (msgTypeBoiler == OpenThermMessageType::WRITE_ACK) {
    t_ACK = millis();
  }
  if (millis() - t_ACK > ACK_TIMEOUT) {
    ACK_timeout_notify = true;
  }

  // Thermostat `master` request
  snprintf(char_buffer, CHAR_BUFFER_LEN, "T%08x", request);
#ifdef DEBUG
  Serial.println(char_buffer);
#endif
  notifyClients(char_buffer);

  // Boiler 'slave' response
  snprintf(char_buffer, CHAR_BUFFER_LEN, "B%08x", _lastRresponse);
#ifdef DEBUG
  Serial.println(char_buffer);
#endif
  notifyClients(char_buffer);

  // Update the variables that you want logged
  // -----------------------------------------

  // 0: Status
  if (dataId == OpenThermMessageID::Status) {
    _IsFlameOn = mOT.isFlameOn(_lastRresponse);
  }
  // 1: Control setpoint ie CH water temperature setpoint (°C)
  if (dataId == OpenThermMessageID::TSet) {
    _TSet_notify = true;
    _TSet = mOT.getFloat(_lastRresponse);
  }
  // 16: Room Setpoint (°C)
  if (dataId == OpenThermMessageID::TrSet) {
    _TrSet_notify = true;
    _TrSet = mOT.getFloat(_lastRresponse);
  }
  // 17: Relative Modulation Level (%)
  if (dataId == OpenThermMessageID::RelModLevel) {
    _RelModLevel = mOT.getFloat(_lastRresponse);
  }
  // 24: Room temperature (°C)
  if (dataId == OpenThermMessageID::Tr) {
    _Tr_notify = true;
    _Tr = mOT.getFloat(_lastRresponse);
  }
  // 25: Boiler flow water temperature (°C)
  if (dataId == OpenThermMessageID::Tboiler) {
    _Tboiler_notify = true;
    _Tboiler = mOT.getFloat(_lastRresponse);
  }
  // 26: DHW temperature (°C)
  if (dataId == OpenThermMessageID::Tdhw) {
    _Tdhw_notify = true;
    _Tdhw = mOT.getFloat(_lastRresponse);
  }
  // 56: DHW setpoint (°C)
  if (dataId == OpenThermMessageID::TdhwSet) {
    _TdhwSet_notify = true;
    _TdhwSet = mOT.getFloat(_lastRresponse);
  }
}

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
#ifdef DEBUG
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(),
                    client->remoteIP().toString().c_str());
#endif
      break;
    case WS_EVT_DISCONNECT:
#ifdef DEBUG
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

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

void notFound(AsyncWebServerRequest *request) {
  snprintf(char_buffer, CHAR_BUFFER_LEN, "%s", "Not found, ");
  // Will truncate an excessively long url
  request->url().toCharArray(&char_buffer[11], CHAR_BUFFER_LEN - 12);
#ifdef DEBUG
  Serial.println(char_buffer);
#endif
  request->send(404, "text/plain", char_buffer);
}

String htmlVarProcessor(const String &var) {
  if (var == "IP_ADDR")
    return WiFi.localIP().toString();

  if (var == "READ_TOKEN")
    return myReadAPIKey;

  if (var == "CHANNEL_ID")
    return String(myChannelNumber);

  return String();
}

/*------------------------------------------------------------------------------
  setup
------------------------------------------------------------------------------*/

void setup() {
#ifdef DEBUG
  Serial.begin(9600);
  Serial.println("WiFi...");
#endif
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
#ifdef DEBUG
    Serial.println("WiFi Failed!");
#endif
    return;
  }

#ifdef DEBUG
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
#endif

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html, htmlVarProcessor);
  });

#ifdef ALLOW_USER_FORCE_DISABLE_CH_DHW
  server.on("/heating-false", HTTP_GET, [](AsyncWebServerRequest *request) {
#  ifdef DEBUG
    Serial.println("Heating disable override");
#  endif
    request->send(200, "text/plain", "OK: heating off");
    _heating_disable = true;
  });

  server.on("/heating-true", HTTP_GET, [](AsyncWebServerRequest *request) {
#  ifdef DEBUG
    Serial.println("Heating enable");
#  endif
    request->send(200, "text/plain", "OK: heating on");
    _heating_disable = false;
  });

  server.on("/dhw-false", HTTP_GET, [](AsyncWebServerRequest *request) {
#  ifdef DEBUG
    Serial.println("Domestic hot water disable override");
#  endif
    request->send(200, "text/plain", "OK: dhw off");
    _dhw_disable = true;
  });

  server.on("/dhw-true", HTTP_GET, [](AsyncWebServerRequest *request) {
#  ifdef DEBUG
    Serial.println("Domestic hot water enable");
#  endif
    request->send(200, "text/plain", "OK: dhw on");
    _dhw_disable = false;
  });
#endif

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

  server.on("/ack_timeout", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "Force triggering an `ACK_TIMEOUT`");
    force_trigger_ACK_timeout = true;
  });

  // Enable OTA updates
  AsyncElegantOTA.begin(&server);

  server.onNotFound(notFound);
  server.begin();

  ThingSpeak.begin(client);

  initWebSocket();

  mOT.begin(mHandleInterrupt);
  sOT.begin(sHandleInterrupt, processRequest);

  // Set the network reconnection option
  MailClient.networkReconnect(true);
  smtp.debug(0);

#ifdef ESP32
  // Watchdog timer
  esp_task_wdt_init(WDT_TIMEOUT, true);
  esp_task_wdt_add(NULL);
#endif
}

/*------------------------------------------------------------------------------
  loop
------------------------------------------------------------------------------*/

void loop() {
  unsigned long now = millis();

#ifdef ESP32
  // Watchdog timer
  if (now - t_WDT_reset > WDT_RESET_PERIOD) {
    t_WDT_reset = now;
    esp_task_wdt_reset();
  }
#endif

  // DEBUG: Useful for testing
  if (force_trigger_ACK_timeout) {
    ACK_timeout_notify = true;
  }

  // Boiler communication check
  if (ACK_timeout_notify) {
    // Send an `!! ACK_TIMEOUT` email to the user and reset the microcontroller
    ACK_timeout_notify = false;
    snprintf(char_buffer, CHAR_BUFFER_LEN, "!! ACK_TIMEOUT %u", now - t_ACK);
    notifyClients(char_buffer);
    sendAlertEmail();
    sleep(WDT_TIMEOUT + 1000); // Trigger the watchdog timer
  }

  sOT.process();
  ws.cleanupClients();

  if (_TSet_notify) {
    _TSet_notify = false;
    build_notify_msg(char_buffer, CHAR_BUFFER_LEN, "A:", _TSet);
    notifyClients(char_buffer);
  }
  if (_Tboiler_notify) {
    _Tboiler_notify = false;
    build_notify_msg(char_buffer, CHAR_BUFFER_LEN, "B:", _Tboiler);
    notifyClients(char_buffer);
  }
  if (_TdhwSet_notify) {
    _TdhwSet_notify = false;
    build_notify_msg(char_buffer, CHAR_BUFFER_LEN, "C:", _TdhwSet);
    notifyClients(char_buffer);
  }
  if (_Tdhw_notify) {
    _Tdhw_notify = false;
    build_notify_msg(char_buffer, CHAR_BUFFER_LEN, "D:", _Tdhw);
    notifyClients(char_buffer);
  }
  if (_TrSet_notify) {
    _TrSet_notify = false;
    build_notify_msg(char_buffer, CHAR_BUFFER_LEN, "E:", _TrSet);
    notifyClients(char_buffer);
  }
  if (_Tr_notify) {
    _Tr_notify = false;
    build_notify_msg(char_buffer, CHAR_BUFFER_LEN, "F:", _Tr);
    notifyClients(char_buffer);
  }

  if (_thingSpeakUpd < millis()) {
    _thingSpeakUpd = millis() + 20000;

    if (myChannelNumber == 0 || myReadAPIKey == "TOKEN" ||
        myWriteAPIKey == "TOKEN")
      return;

    ThingSpeak.setField(1, _Tboiler);
    ThingSpeak.setField(2, _TSet);
    ThingSpeak.setField(3, _RelModLevel);
    ThingSpeak.setField(4, _IsFlameOn);
    ThingSpeak.setField(5, _TrSet);
    ThingSpeak.setField(6, _Tr * 100);

    int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
#ifdef DEBUG
    if (x == 200) {
      Serial.println("Channel update successful.");
    } else {
      Serial.print("Problem updating channel. HTTP error code ");
      Serial.println(x);
    }
#endif
  }
}
