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
float _MaxTSet = 0;
bool  _MaxTSet_notify = false;
#ifdef ALLOW_USER_FORCE_DISABLE_CH_DHW
bool  _heating_disable = false;
bool  _dhw_disable = false;
#endif
// clang-format on

// Watchdog timer
const unsigned long WDT_TIMEOUT = 30000;     // Timeout [ms]
const unsigned long WDT_RESET_PERIOD = 5000; // Slow down reset on purpose [ms]
unsigned long t_WDT_reset = millis();        // Time of last watchdog reset

// Boiler communication check
// --------------------------
// Check for any WRITE_ACK messages coming back from the boiler.
// If none are received within the set timeout window, the communication with
// the boiler has probably failed. In that case: Notify the user by email and
// reset the microcontroller.
const unsigned long WRITE_ACK_TIMEOUT = 30000; // Timeout [ms]
unsigned long t_WRITE_ACK = millis();          // Time of last WRITE_ACK
bool trigger_reset = false;                    // When true will reset the MCU
SMTPSession smtp;                              // To allow sending email

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

void processRequest(unsigned long sOT_request,
                    OpenThermResponseStatus sOT_status) {
  /* Some explanation:
  sOT: Instance linked to the 'slave' OpenTherm shield. It receives requests
       coming from the master thermostat and sends responses back to it.
  mOT: Instance linked to the 'master' OpenTherm shield. It sends requests to
       the slave boiler and receives responses coming back from it.
  */
  OpenThermMessageType sOT_msgType = sOT.getMessageType(sOT_request);
  OpenThermMessageID sOT_dataId = sOT.getDataID(sOT_request);

#ifdef ALLOW_USER_FORCE_DISABLE_CH_DHW
  // Optionally modify thermostat request
  if (sOT_msgType == OpenThermMessageType::READ_DATA &&
      sOT_dataId == OpenThermMessageID::Status) {

    if (_heating_disable) {
#  ifdef DEBUG
      Serial.println("Disable Heating");
#  endif
      sOT_request &= ~(1ul << (0 + 8));
    }

    if (_dhw_disable) {
#  ifdef DEBUG
      Serial.println("Disable DHW");
#  endif
      sOT_request &= ~(1ul << (1 + 8));
    }
    sOT_request &= ~(1ul << 31);

    if (mOT.parity(sOT_request)) {
      sOT_request |= (1ul << 31);
    }
  }
#endif

  // Specific to my boiler Remeha Avanta 28c
  //
  // UNKNOWN_DATA_ID, READ_DATA
  //   ID18: CHPressure   seldom
  //   ID26: Tdhw         seldom
  //   ID27: Toutside     every minute
  //   ID29: Tstorage     seldom
  //   ID35: ?            seldom
  //   ID36: ?            seldom

  // Send the intercepted (and optionally modified) thermostat request further
  // downstream to the boiler and listen for the boiler's response.
  unsigned long mOT_response = mOT.sendRequest(sOT_request);

  if (mOT_response) {
    // A boiler response is waiting to be processed. Send this intercepted
    // response further upstream to the thermostat
    sOT.sendResponse(mOT_response);

    // Boiler communication check
    if (mOT.getMessageType(mOT_response) == OpenThermMessageType::WRITE_ACK) {
      t_WRITE_ACK = millis();
    }
  }

  // Inform the webclients about the thermostat `master` request
  snprintf(char_buffer, CHAR_BUFFER_LEN, "T%08x", sOT_request);
#ifdef DEBUG
  Serial.println(char_buffer);
#endif
  notifyClients(char_buffer);

  // Inform the webclients about the boiler 'slave' response
  snprintf(char_buffer, CHAR_BUFFER_LEN, "B%08x", mOT_response);
#ifdef DEBUG
  Serial.println(char_buffer);
#endif
  notifyClients(char_buffer);

  // Update the variables that you want logged
  // -----------------------------------------
  if ((mOT_response == OpenThermMessageType::READ_ACK) ||
      (mOT_response == OpenThermMessageType::WRITE_ACK) ||
      (mOT_response == OpenThermMessageType::DATA_INVALID)) {
    // NOTE: We also accept DATA_INVALID, because e.g.
    //   request : WRITE_DATA   | 57 | MaxTSet | 90.00
    //   response: DATA_INVALID | 57 | MaxTSet | 75.00
    // Meaning the thermostat requested a max boiler setpoint of 90'C,
    // but was only rewarded 75'C by the boiler (hard-limited).

    // 0: Status
    if (sOT_dataId == OpenThermMessageID::Status) {
      _IsFlameOn = mOT.isFlameOn(mOT_response);
    }
    // 1: Control setpoint ie CH water temperature setpoint (°C)
    if (sOT_dataId == OpenThermMessageID::TSet) {
      _TSet_notify = true;
      _TSet = mOT.getFloat(mOT_response);
    }
    // 16: Room Setpoint (°C)
    if (sOT_dataId == OpenThermMessageID::TrSet) {
      _TrSet_notify = true;
      _TrSet = mOT.getFloat(mOT_response);
    }
    // 17: Relative Modulation Level (%)
    if (sOT_dataId == OpenThermMessageID::RelModLevel) {
      _RelModLevel = mOT.getFloat(mOT_response);
    }
    // 24: Room temperature (°C)
    if (sOT_dataId == OpenThermMessageID::Tr) {
      _Tr_notify = true;
      _Tr = mOT.getFloat(mOT_response);
    }
    // 25: Boiler flow water temperature (°C)
    if (sOT_dataId == OpenThermMessageID::Tboiler) {
      _Tboiler_notify = true;
      _Tboiler = mOT.getFloat(mOT_response);
    }
    // 26: DHW temperature (°C)
    if (sOT_dataId == OpenThermMessageID::Tdhw) {
      _Tdhw_notify = true;
      _Tdhw = mOT.getFloat(mOT_response);
    }
    // 56: DHW setpoint (°C)
    if (sOT_dataId == OpenThermMessageID::TdhwSet) {
      _TdhwSet_notify = true;
      _TdhwSet = mOT.getFloat(mOT_response);
    }
    // 57: Max CH water setpoint (°C)
    if (sOT_dataId == OpenThermMessageID::MaxTSet) {
      _MaxTSet_notify = true;
      _MaxTSet = mOT.getFloat(mOT_response);
    }
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
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

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

  server.on("/trigger_reset", HTTP_GET, [](AsyncWebServerRequest *request) {
    snprintf(char_buffer, CHAR_BUFFER_LEN,
             "Force triggering a reset in %u seconds", WDT_TIMEOUT / 1000);
    request->send(200, "text/plain", char_buffer);
    trigger_reset = true;
  });

  server.onNotFound(notFound);
  ws.onEvent(onEvent); // Init websocket
  server.addHandler(&ws);
  server.begin();

  // Email notification
  MailClient.networkReconnect(true);
  smtp.debug(0);

  // Over-the-air updates
  AsyncElegantOTA.begin(&server);

  // ThingSpeak
  ThingSpeak.begin(client);

  // OpenTherm shields
  mOT.begin(mHandleInterrupt);
  sOT.begin(sHandleInterrupt, processRequest);

#ifdef ESP32
  // Watchdog timer
  esp_task_wdt_init(WDT_TIMEOUT, true);
  esp_task_wdt_add(NULL);
#endif

  delay(3000);
  digitalWrite(LED_BUILTIN, LOW);
}

/*------------------------------------------------------------------------------
  loop
------------------------------------------------------------------------------*/

bool waiting_for_ESP_restart = false;
unsigned long ESP_restart_timer = 0;

void loop() {
  unsigned long now = millis();
  static unsigned long still_alive_blinker_tick = now;

#ifdef ESP32
  // Watchdog timer
  if (now - t_WDT_reset > WDT_RESET_PERIOD) {
    t_WDT_reset = now;
    esp_task_wdt_reset();

    // DEBUG info
    snprintf(char_buffer, CHAR_BUFFER_LEN, "FH          %u", ESP.getFreeHeap());
    notifyClients(char_buffer);
  }
#endif

  if (waiting_for_ESP_restart) {
    // Wait a long time before restarting; prevents fast-cycling restarts.
    // While we wait, still execute `loop()` to allow any websocket and
    // OpenTherm communication.
    if (now - ESP_restart_timer > WDT_TIMEOUT) {
      ESP.restart();
    }
  } else {
    // Boiler communication check
    if (now - t_WRITE_ACK > WRITE_ACK_TIMEOUT) {
      trigger_reset = true;
    }
    if (trigger_reset) {
      snprintf(char_buffer, CHAR_BUFFER_LEN, "!! RESET TRIGGERED, %u",
               now - t_WRITE_ACK);
      notifyClients(char_buffer);
      sendAlertEmail();
#ifdef ESP32
      waiting_for_ESP_restart = true;
      ESP_restart_timer = now;
#else
      delay(WDT_TIMEOUT); // Trigger the watchdog timer
#endif
    }
  }

  sOT.process();
  // ws.cleanupClients(); // DEBUG: Commented out to investigate ESP32 hang

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
  if (_MaxTSet_notify) {
    _MaxTSet_notify = false;
    build_notify_msg(char_buffer, CHAR_BUFFER_LEN, "G:", _MaxTSet);
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

  if (now - still_alive_blinker_tick >= 900) {
    digitalWrite(LED_BUILTIN, HIGH);
  }

  if (now - still_alive_blinker_tick >= 1000) {
    still_alive_blinker_tick = now;
    digitalWrite(LED_BUILTIN, LOW);
  }

  delay(10); // Necessary idle time to allow the ESP task manager to run
}
