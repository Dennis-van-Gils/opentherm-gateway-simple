#include <Arduino.h>
#ifdef ESP32
#  include "AsyncTCP.h"
#  include <WiFi.h>
#  include <esp_task_wdt.h>
#elif defined(ESP8266)
#  include "src/ESPAsyncTCP/ESPAsyncTCP.h"
#  include <ESP8266WiFi.h>
#endif
#include "ESPAsyncWebServer.h"
#include "ThingSpeak.h"
#include <OpenTherm.h>

// Web content
#include "webcontent_css.h"
#include "webcontent_favicon.h"
#include "webcontent_index.h"
#include "webcontent_js.h"

// API settings
#if __has_include("wifi_settings.h")
#  include <wifi_settings.h>
#else
#  include <wifi_settings_template.h>
#endif

// Enable OTA updates
#include <AsyncElegantOTA.h>

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
bool  _heating_disable = false;
bool  _dhw_disable = false;
bool  _IsFlameOn = false;
float _Tboiler = 0;
bool  _Tboiler_notify = false;
float _Tdhw = 0;
bool  _Tdhw_notify = false;
float _TdhwSet = 0;
bool  _TdhwSet_notify = false;
float _TSet = 0;
bool  _TSet_notify = false;
float _Tr = 0;
bool  _Tr_notify = false;
float _TrSet = 0;
bool  _TrSet_notify = false;
float _RelModLevel = 0;
// clang-format on

void notifyClients(String s) { ws.textAll(s); }

void processRequest(unsigned long request, OpenThermResponseStatus status) {
  OpenThermMessageType msgType = mOT.getMessageType(request);
  OpenThermMessageID dataId = mOT.getDataID(request);

  // Potentially modify thermostat request
  if (msgType == OpenThermMessageType::READ_DATA &&
      dataId == OpenThermMessageID::Status) {
    if (_heating_disable) {
      Serial.println("Disable Heating");
      request &= ~(1ul << (0 + 8));
    }
    if (_dhw_disable) {
      Serial.println("Disable DHW");
      request &= ~(1ul << (1 + 8));
    }
    request &= ~(1ul << 31);
    if (mOT.parity(request))
      request |= (1ul << 31);
  }

  /*
  // Test writing an external room temperature
  if (msgType == OpenThermMessageType::WRITE_DATA &&
      dataId == OpenThermMessageID::Tr) {}
  */

  _lastRresponse = mOT.sendRequest(request);
  sOT.sendResponse(_lastRresponse);

  // Thermostat request
  String masterRequest = "T" + String(request, HEX);
  Serial.println(masterRequest + " " + String(request, BIN));
  notifyClients(masterRequest);

  // Boiler response
  String slaveResponse = "B" + String(_lastRresponse, HEX);
  Serial.println(slaveResponse);
  notifyClients(slaveResponse);

  // Update the variables that you want logged
  // -----------------------------------------

  // Status
  if (dataId == OpenThermMessageID::Status) {
    _IsFlameOn = mOT.isFlameOn(_lastRresponse);
  }
  // Control setpoint ie CH water temperature setpoint (°C)
  if (dataId == OpenThermMessageID::TSet) {
    _TSet_notify = true;
    _TSet = mOT.getFloat(_lastRresponse);
  }
  // Room Setpoint (°C)
  if (dataId == OpenThermMessageID::TrSet) {
    _TrSet_notify = true;
    _TrSet = mOT.getFloat(_lastRresponse);
  }
  // Relative Modulation Level (%)
  if (dataId == OpenThermMessageID::RelModLevel) {
    _RelModLevel = mOT.getFloat(_lastRresponse);
  }
  // Room temperature (°C)
  if (dataId == OpenThermMessageID::Tr) {
    _Tr_notify = true;
    _Tr = mOT.getFloat(_lastRresponse);
  }
  // Boiler flow water temperature (°C)
  if (dataId == OpenThermMessageID::Tboiler) {
    _Tboiler_notify = true;
    _Tboiler = mOT.getFloat(_lastRresponse);
  }
  // DHW temperature (°C)
  if (dataId == OpenThermMessageID::Tdhw) {
    _Tdhw_notify = true;
    _Tdhw = mOT.getFloat(_lastRresponse);
  }
  // DHW setpoint (°C)
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
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(),
                    client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
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
  request->send(404, "text/plain", "Not found " + request->url());
  Serial.println("Not found " + request->url());
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

void setup() {
  Serial.begin(9600);
  Serial.println("WiFi...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.print("WiFi Failed!\n");
    return;
  }

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html, htmlVarProcessor);
  });

  server.on("/heating-false", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("Heating disable override");
    request->send(200, "text/plain", "OK: heating off");
    _heating_disable = true;
  });

  server.on("/heating-true", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("Heating enable");
    request->send(200, "text/plain", "OK: heating on");
    _heating_disable = false;
  });

  server.on("/dhw-false", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("Domestic hot water disable override");
    request->send(200, "text/plain", "OK: dhw off");
    _dhw_disable = true;
  });

  server.on("/dhw-true", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("Domestic hot water enable");
    request->send(200, "text/plain", "OK: dhw on");
    _dhw_disable = false;
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

  // Enable OTA updates
  AsyncElegantOTA.begin(&server);

  server.onNotFound(notFound);
  server.begin();

  ThingSpeak.begin(client);

  initWebSocket();

  mOT.begin(mHandleInterrupt);
  sOT.begin(sHandleInterrupt, processRequest);

#ifdef ESP32
  esp_task_wdt_init(10, true); // enable panic so ESP32 restarts
  esp_task_wdt_add(NULL);      // add current thread to WDT watch
#endif
}

void loop() {
#ifdef ESP32
  esp_task_wdt_reset();
#endif
  sOT.process();
  ws.cleanupClients();

  if (_TSet_notify) {
    _TSet_notify = false;
    notifyClients("A:" + String(_TSet));
  }
  if (_Tboiler_notify) {
    _Tboiler_notify = false;
    notifyClients("B:" + String(_Tboiler));
  }
  if (_TdhwSet_notify) {
    _TdhwSet_notify = false;
    notifyClients("C:" + String(_TdhwSet));
  }
  if (_Tdhw_notify) {
    _Tdhw_notify = false;
    notifyClients("D:" + String(_Tdhw));
  }
  if (_TrSet_notify) {
    _TrSet_notify = false;
    notifyClients("E:" + String(_TrSet));
  }
  if (_Tr_notify) {
    _Tr_notify = false;
    notifyClients("F:" + String(_Tr));
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
    if (x == 200) {
      Serial.println("Channel update successful.");
    } else {
      Serial.println("Problem updating channel. HTTP error code " + String(x));
    }
  }
}
