#include <Arduino.h>
#ifdef ESP32
#include <WiFi.h>
#include "src/ESPAsyncWebServer/AsyncTCP.h"
#include <esp_task_wdt.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include "SPIFFS.h"
#include "src/ESPAsyncWebServer/ESPAsyncWebServer.h"
#include "src/ESPAsyncWebServer/SPIFFSEditor.h"
#include "src/ThingSpeak/ThingSpeak.h"
#include <OpenTherm.h>

// API settings

const char* ssid = "SSID";
const char* password = "PASSWORD";
unsigned long myChannelNumber = CHN;
const char* myReadAPIKey = "TOKEN";
const char* myWriteAPIKey = "TOKEN";

// HW settings

const int mInPin = 21; //2 for Arduino, 4 for ESP8266 (D2), 21 for ESP32
const int mOutPin = 22; //4 for Arduino, 5 for ESP8266 (D1), 22 for ESP32

const int sInPin = 19; //3 for Arduino, 12 for ESP8266 (D6), 19 for ESP32
const int sOutPin = 23; //5 for Arduino, 13 for ESP8266 (D7), 23 for ESP32

// code

const char index_html[] PROGMEM = R"rawliteral(
<script type="text/javascript">var ipAddr="192.168.88.38"; var readToken="`READ_TOKEN`"; var channelId="`CHANNEL_ID`"; //ipAddr=0; readToken="XZRX9TUVV1OH7I6M"; channelId="1380393";</script><!DOCTYPE html><html lang="en"> <head> <meta charset="UTF-8"/> <meta name="viewport" content="width=device-width, initial-scale=1.0"/> <meta http-equiv="X-UA-Compatible" content="ie=edge"/> <title>Brush charts</title> <link href="../styles.css" rel="stylesheet"/> <script src="https://cdn.jsdelivr.net/npm/apexcharts"></script> <script src="../otgw-core.js" async></script> <style>.wrapr{padding-top: 20px; padding-left: 10px; padding-bottom: 20px; padding-right: 10px; background: #fff; border: 1px solid #ddd; box-shadow: 0 22px 35px -16px rgba(0, 0, 0, 0.1); max-width: 650px; margin: 35px auto;}#chart-line{position: relative; margin-top: -40px;}.center{display: flex; align-items: center; justify-content: center;}.label-column{white-space: nowrap; width: auto;}.switch{position: relative; display: inline-block; width: 40px; height: 24px;}.switch input{opacity: 0; width: 0; height: 0;}.slider{position: absolute; cursor: pointer; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; -webkit-transition: 0.4s; transition: 0.4s;}.slider:before{position: absolute; content: ""; height: 16px; width: 16px; left: 4px; bottom: 4px; background-color: white; -webkit-transition: 0.4s; transition: 0.4s;}input:checked + .slider{background-color: #2196f3;}input:focus + .slider{box-shadow: 0 0 1px #2196f3;}input:checked + .slider:before{-webkit-transform: translateX(16px); -ms-transform: translateX(16px); transform: translateX(16px);}/* Rounded sliders */ .slider.round{border-radius: 24px;}.slider.round:before{border-radius: 50%;}</style> </head> <body> <div class="wrapr"> <table style="width: 100%; background-color: aquamarine" id="idd"> <tr> <td class="label-column">From</td><td style="width: auto"> <input type="datetime-local" id="date-from"/> </td><td style="width: 100%"></td></tr><tr> <td class="label-column">To</td><td style="width: auto"> <input type="datetime-local" id="date-to"/> </td><td style="width: 100%"></td></tr><tr> <td class="label-column">Heating target, &#x00B0;C</td><td style="width: auto"></td><td class="label-column">Heating current, &#x00B0;C</td></tr><tr> <td class="label-column">Hot water target, &#x00B0;C</td><td style="width: auto"></td><td class="label-column">Hot water current, &#x00B0;C</td></tr><tr> <td class="label-column">Heating enable</td><td style="width: auto"></td><td class="label-column"> <label class="switch"> <input type="checkbox" checked id="heatingEnableInput"/> <span class="slider round"></span> </label> </td></tr></table> </div><div class="wrapr center" style="height: 500px" id="chart-container"> <div id="waiting-indicator" style="text-align: center; margin: 0 auto">LOADING...</div><div id="chart-flame"></div><div id="chart-line2"></div><div id="chart-line"></div></div><textarea class="wrapr center" style="height: 150px; width: 100%" id="commands-log"></textarea> </body></html>
)rawliteral";

const int favico_ico_length = 4286;
const byte favico_ico[] PROGMEM = {0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x20, 0x20, 0x00, 0x00, 0x01, 0x00, 0x20, 0x00, 0xA8, 0x10, 0x00, 0x00, 0x16, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x01, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFA, 0xFF, 0xFF, 0xFF, 0xFC, 0xFD, 0xEC, 0xCB, 0xFC, 0xFA, 0xC5, 0x5B, 0xFC, 0xF9, 0xBC, 0x3B, 0xFC, 0xFB, 0xCC, 0x70, 0xFC, 0xFD, 0xE9, 0xC1, 0xFC, 0xFF, 0xFC, 0xF6, 0xFC, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFD, 0xF8, 0xFC, 0xFE, 0xF4, 0xDE, 0xFC, 0xFE, 0xF5, 0xE0, 0xFC, 0xFE, 0xF5, 0xE1, 0xFC, 0xFE, 0xF5, 0xE1, 0xFC, 0xFE, 0xF5, 0xE1, 0xFC, 0xFE, 0xF5, 0xE1, 0xFC, 0xFE, 0xF5, 0xE1, 0xFC, 0xFE, 0xF5, 0xE1, 0xFC, 0xFE, 0xF5, 0xE1, 0xFC, 0xFE, 0xF5, 0xE1, 0xFC, 0xFE, 0xF5, 0xE1, 0xFC, 0xFE, 0xF5, 0xE1, 0xFC, 0xFE, 0xF7, 0xE7, 0xFC, 0xFF, 0xFE, 0xFD, 0xFC, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFA, 0xFF, 0xFF, 0xFF, 0xFC, 0xFE, 0xF3, 0xDE, 0xFF, 0xFA, 0xC6, 0x64, 0xFF, 0xF8, 0xB2, 0x21, 0xFF, 0xF8, 0xAA, 0x07, 0xFF, 0xF8, 0xA8, 0x00, 0xFF, 0xF8, 0xAF, 0x14, 0xFF, 0xFA, 0xC4, 0x59, 0xFF, 0xFD, 0xE7, 0xBD, 0xFF, 0xFF, 0xFD, 0xFA, 0xFF, 0xFF, 0xFB, 0xF3, 0xFF, 0xFC, 0xD3, 0x88, 0xFF, 0xF9, 0xB4, 0x30, 0xFF, 0xF9, 0xB4, 0x26, 0xFF, 0xF9, 0xB5, 0x29, 0xFF, 0xF9, 0xB5, 0x29, 0xFF, 0xF9, 0xB5, 0x29, 0xFF, 0xF9, 0xB5, 0x29, 0xFF, 0xF9, 0xB5, 0x29, 0xFF, 0xF9, 0xB5, 0x29, 0xFF, 0xF9, 0xB5, 0x29, 0xFF, 0xF9, 0xB5, 0x29, 0xFF, 0xF9, 0xB4, 0x26, 0xFF, 0xFA, 0xC2, 0x59, 0xFF, 0xFF, 0xFB, 0xF2, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFD, 0xFC, 0xFD, 0xEA, 0xC7, 0xFF, 0xFE, 0xED, 0xCE, 0xFF, 0xFE, 0xF5, 0xE2, 0xFF, 0xFC, 0xE2, 0xB0, 0xFF, 0xFA, 0xC4, 0x58, 0xFF, 0xF8, 0xAD, 0x12, 0xFF, 0xF8, 0xA7, 0x00, 0xFF, 0xF8, 0xAD, 0x11, 0xFF, 0xFB, 0xCA, 0x6A, 0xFF, 0xFE, 0xF1, 0xD9, 0xFF, 0xFF, 0xFC, 0xF6, 0xFF, 0xFC, 0xD8, 0x94, 0xFF, 0xF9, 0xAE, 0x17, 0xFF, 0xF8, 0xA9, 0x00, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF8, 0xA9, 0x00, 0xFF, 0xF9, 0xBA, 0x3B, 0xFF, 0xFF, 0xFA, 0xF1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFD, 0xE9, 0xC0, 0xFC, 0xF8, 0xB1, 0x26, 0xFF, 0xFC, 0xD6, 0x8E, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xF8, 0xFF, 0xFD, 0xE8, 0xC0, 0xFF, 0xFA, 0xC2, 0x50, 0xFF, 0xF8, 0xAA, 0x08, 0xFF, 0xF8, 0xA8, 0x00, 0xFF, 0xF9, 0xB4, 0x29, 0xFF, 0xFC, 0xDE, 0xA4, 0xFF, 0xFF, 0xFC, 0xF8, 0xFF, 0xFD, 0xE6, 0xB9, 0xFF, 0xF9, 0xB6, 0x2E, 0xFF, 0xF8, 0xA9, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF9, 0xBA, 0x3C, 0xFF, 0xFF, 0xFA, 0xF1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFC, 0xD6, 0x88, 0xFC, 0xF8, 0xA8, 0x00, 0xFF, 0xFA, 0xC4, 0x54, 0xFF, 0xFF, 0xFD, 0xF9, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFB, 0xF4, 0xFF, 0xFC, 0xDD, 0xA0, 0xFF, 0xF9, 0xB5, 0x2B, 0xFF, 0xF8, 0xA8, 0x00, 0xFF, 0xF8, 0xAB, 0x0B, 0xFF, 0xFB, 0xCC, 0x71, 0xFF, 0xFE, 0xF8, 0xEB, 0xFF, 0xFE, 0xEF, 0xD5, 0xFF, 0xFA, 0xBD, 0x43, 0xFF, 0xF8, 0xA9, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF9, 0xBA, 0x3C, 0xFF, 0xFF, 0xFA, 0xF1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFD, 0xE6, 0xB8, 0xFC, 0xF8, 0xAD, 0x0F, 0xFF, 0xF9, 0xB1, 0x1A, 0xFF, 0xFE, 0xF0, 0xD3, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xF0, 0xD5, 0xFF, 0xFD, 0xE5, 0xB9, 0xFF, 0xFF, 0xFE, 0xFC, 0xFF, 0xFE, 0xF1, 0xD7, 0xFF, 0xFA, 0xC2, 0x54, 0xFF, 0xF8, 0xAA, 0x04, 0xFF, 0xF8, 0xA9, 0x01, 0xFF, 0xFA, 0xBE, 0x49, 0xFF, 0xFE, 0xF0, 0xD8, 0xFF, 0xFE, 0xF6, 0xE5, 0xFF, 0xFA, 0xC5, 0x5C, 0xFF, 0xF8, 0xA9, 0x03, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF9, 0xBA, 0x3C, 0xFF, 0xFF, 0xFA, 0xF1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFA, 0xF1, 0xFC, 0xFA, 0xBF, 0x46, 0xFF, 0xF8, 0xA8, 0x00, 0xFF, 0xFB, 0xD3, 0x81, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0xE6, 0xB8, 0xFF, 0xF8, 0xAD, 0x18, 0xFF, 0xFB, 0xCC, 0x70, 0xFF, 0xFE, 0xF4, 0xE2, 0xFF, 0xFF, 0xFA, 0xF2, 0xFF, 0xFB, 0xD2, 0x81, 0xFF, 0xF8, 0xAD, 0x10, 0xFF, 0xF8, 0xA8, 0x00, 0xFF, 0xF9, 0xB6, 0x2E, 0xFF, 0xFD, 0xE7, 0xBD, 0xFF, 0xFF, 0xF9, 0xEF, 0xFF, 0xFB, 0xC9, 0x69, 0xFF, 0xF8, 0xAA, 0x04, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF9, 0xBA, 0x3C, 0xFF, 0xFF, 0xFA, 0xF1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFC, 0xFC, 0xDC, 0x9B, 0xFF, 0xF8, 0xA9, 0x01, 0xFF, 0xF9, 0xB4, 0x27, 0xFF, 0xFE, 0xF3, 0xDB, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0xE7, 0xBA, 0xFF, 0xF8, 0xAB, 0x08, 0xFF, 0xF8, 0xA9, 0x00, 0xFF, 0xF9, 0xB8, 0x35, 0xFF, 0xFD, 0xE6, 0xBA, 0xFF, 0xFF, 0xFD, 0xF9, 0xFF, 0xFC, 0xE0, 0xA7, 0xFF, 0xF9, 0xB1, 0x20, 0xFF, 0xF8, 0xA8, 0x00, 0xFF, 0xF9, 0xB1, 0x1E, 0xFF, 0xFD, 0xE1, 0xAC, 0xFF, 0xFF, 0xF9, 0xEE, 0xFF, 0xFB, 0xCA, 0x6B, 0xFF, 0xF8, 0xA9, 0x04, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF9, 0xBA, 0x3C, 0xFF, 0xFF, 0xFA, 0xF1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFC, 0xFE, 0xF7, 0xE7, 0xFF, 0xF9, 0xB8, 0x35, 0xFF, 0xF8, 0xA8, 0x00, 0xFF, 0xFB, 0xCE, 0x75, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0xE7, 0xBA, 0xFF, 0xF8, 0xAB, 0x08, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xA9, 0x00, 0xFF, 0xF8, 0xAE, 0x14, 0xFF, 0xFC, 0xD5, 0x89, 0xFF, 0xFF, 0xFB, 0xF4, 0xFF, 0xFD, 0xE9, 0xC4, 0xFF, 0xF9, 0xB7, 0x31, 0xFF, 0xF8, 0xA8, 0x00, 0xFF, 0xF9, 0xB0, 0x1E, 0xFF, 0xFE, 0xEF, 0xD3, 0xFF, 0xFF, 0xFA, 0xF2, 0xFF, 0xFB, 0xC9, 0x68, 0xFF, 0xF8, 0xA9, 0x03, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF9, 0xBA, 0x3C, 0xFF, 0xFF, 0xFA, 0xF1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xDC, 0x9D, 0xFF, 0xF8, 0xAA, 0x05, 0xFF, 0xF8, 0xAF, 0x15, 0xFF, 0xFD, 0xE9, 0xC2, 0xFF, 0xFD, 0xE8, 0xBC, 0xFF, 0xF8, 0xAB, 0x08, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF8, 0xAA, 0x06, 0xFF, 0xFA, 0xC6, 0x5F, 0xFF, 0xFE, 0xF5, 0xE4, 0xFF, 0xFE, 0xF0, 0xD5, 0xFF, 0xFA, 0xBF, 0x4B, 0xFF, 0xF8, 0xAC, 0x12, 0xFF, 0xFC, 0xD9, 0x95, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFA, 0xF0, 0xFF, 0xFB, 0xCA, 0x6B, 0xFF, 0xF8, 0xAA, 0x03, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF9, 0xBA, 0x3C, 0xFF, 0xFF, 0xFA, 0xF1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF9, 0xEF, 0xFF, 0xFA, 0xBF, 0x48, 0xFF, 0xF8, 0xA7, 0x00, 0xFF, 0xFA, 0xBE, 0x46, 0xFF, 0xFC, 0xDF, 0xA3, 0xFF, 0xF8, 0xAC, 0x09, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xA9, 0x01, 0xFF, 0xF9, 0xBB, 0x3D, 0xFF, 0xFE, 0xEE, 0xCF, 0xFF, 0xFF, 0xFA, 0xF2, 0xFF, 0xFD, 0xEC, 0xCA, 0xFF, 0xFE, 0xEE, 0xCE, 0xFF, 0xFE, 0xED, 0xCF, 0xFF, 0xFC, 0xD5, 0x8A, 0xFF, 0xFC, 0xE0, 0xA8, 0xFF, 0xFB, 0xC5, 0x5C, 0xFF, 0xF8, 0xA8, 0x00, 0xFF, 0xF9, 0xBA, 0x3B, 0xFF, 0xFF, 0xFA, 0xF1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0xE8, 0xBE, 0xFF, 0xF9, 0xAE, 0x15, 0xFF, 0xF8, 0xAB, 0x0B, 0xFF, 0xFB, 0xCD, 0x70, 0xFF, 0xF8, 0xAD, 0x0C, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xA9, 0x00, 0xFF, 0xF9, 0xB5, 0x2C, 0xFF, 0xFD, 0xE7, 0xBC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xF6, 0xE4, 0xFF, 0xF9, 0xB8, 0x35, 0xFF, 0xF8, 0xA8, 0x00, 0xFF, 0xF9, 0xB0, 0x1B, 0xFF, 0xFD, 0xE3, 0xB1, 0xFF, 0xFA, 0xC1, 0x50, 0xFF, 0xF9, 0xB8, 0x3B, 0xFF, 0xFF, 0xFA, 0xF1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFD, 0xFF, 0xFB, 0xD2, 0x80, 0xFF, 0xF8, 0xAA, 0x0D, 0xFF, 0xFB, 0xCC, 0x6C, 0xFF, 0xF8, 0xAD, 0x0C, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF9, 0xB1, 0x1D, 0xFF, 0xFD, 0xE3, 0xB1, 0xFF, 0xFE, 0xEF, 0xD2, 0xFF, 0xF8, 0xAE, 0x17, 0xFF, 0xF8, 0xA9, 0x00, 0xFF, 0xF8, 0xA9, 0x06, 0xFF, 0xFD, 0xE2, 0xAE, 0xFF, 0xFE, 0xF3, 0xDE, 0xFF, 0xFB, 0xCB, 0x74, 0xFF, 0xFF, 0xF9, 0xEE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xF8, 0xEA, 0xFF, 0xFA, 0xBF, 0x51, 0xFF, 0xFB, 0xCA, 0x69, 0xFF, 0xF8, 0xAD, 0x0C, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF9, 0xB0, 0x1B, 0xFF, 0xFC, 0xDF, 0xA5, 0xFF, 0xFB, 0xD2, 0x81, 0xFF, 0xF8, 0xB3, 0x27, 0xFF, 0xFA, 0xC6, 0x61, 0xFF, 0xFD, 0xE5, 0xB7, 0xFF, 0xFD, 0xE7, 0xBE, 0xFF, 0xFF, 0xF9, 0xEC, 0xFF, 0xFF, 0xFD, 0xFA, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xEE, 0xCF, 0xFF, 0xFB, 0xD5, 0x85, 0xFF, 0xF8, 0xAC, 0x0B, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF9, 0xB0, 0x1B, 0xFF, 0xFD, 0xE2, 0xAB, 0xFF, 0xFE, 0xF7, 0xEA, 0xFF, 0xFD, 0xEC, 0xCA, 0xFF, 0xF9, 0xB6, 0x30, 0xFF, 0xF9, 0xB1, 0x1F, 0xFF, 0xFD, 0xE9, 0xC1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0xE7, 0xB8, 0xFF, 0xF8, 0xAB, 0x08, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF8, 0xB1, 0x1E, 0xFF, 0xFD, 0xE7, 0xBC, 0xFF, 0xFE, 0xF1, 0xDA, 0xFF, 0xF9, 0xB7, 0x32, 0xFF, 0xF8, 0xA8, 0x00, 0xFF, 0xF9, 0xB7, 0x31, 0xFF, 0xFE, 0xF1, 0xD7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0xE7, 0xBA, 0xFF, 0xF8, 0xAB, 0x08, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF9, 0xB4, 0x28, 0xFF, 0xFD, 0xEC, 0xCA, 0xFF, 0xFD, 0xE9, 0xC0, 0xFF, 0xF9, 0xB0, 0x1B, 0xFF, 0xF8, 0xA8, 0x00, 0xFF, 0xFA, 0xC2, 0x51, 0xFF, 0xFF, 0xF9, 0xED, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0xE7, 0xBA, 0xFF, 0xF8, 0xAB, 0x08, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xA9, 0x00, 0xFF, 0xF9, 0xB9, 0x37, 0xFF, 0xFE, 0xF3, 0xDC, 0xFF, 0xFC, 0xDD, 0xA0, 0xFF, 0xF8, 0xAC, 0x0D, 0xFF, 0xF8, 0xA9, 0x02, 0xFF, 0xFB, 0xCF, 0x7A, 0xFF, 0xFF, 0xFD, 0xFA, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0xE7, 0xBA, 0xFF, 0xF8, 0xAB, 0x08, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xA9, 0x00, 0xFF, 0xFA, 0xC2, 0x54, 0xFF, 0xFE, 0xF7, 0xE8, 0xFF, 0xFB, 0xCF, 0x78, 0xFF, 0xF8, 0xA9, 0x01, 0xFF, 0xF8, 0xAC, 0x0E, 0xFF, 0xFC, 0xE1, 0xAA, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0xE7, 0xBA, 0xFF, 0xF8, 0xAB, 0x08, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xA9, 0x02, 0xFF, 0xFB, 0xCD, 0x73, 0xFF, 0xFE, 0xF5, 0xE5, 0xFF, 0xFA, 0xC0, 0x4C, 0xFF, 0xF8, 0xA8, 0x00, 0xFF, 0xF9, 0xB6, 0x2C, 0xFF, 0xFE, 0xF1, 0xD8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0xE7, 0xBA, 0xFF, 0xF8, 0xAB, 0x08, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x0A, 0xFF, 0xFC, 0xDB, 0x9A, 0xFF, 0xFE, 0xED, 0xCD, 0xFF, 0xF9, 0xB2, 0x23, 0xFF, 0xF8, 0xA8, 0x00, 0xFF, 0xFB, 0xC8, 0x62, 0xFF, 0xFF, 0xFC, 0xF7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0xE7, 0xBA, 0xFF, 0xF8, 0xAB, 0x08, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xA9, 0x00, 0xFF, 0xF9, 0xBB, 0x3F, 0xFF, 0xFF, 0xFB, 0xF3, 0xFF, 0xFC, 0xDB, 0x99, 0xFF, 0xF8, 0xAA, 0x07, 0xFF, 0xF8, 0xAC, 0x0B, 0xFF, 0xFD, 0xE2, 0xAD, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0xE7, 0xBA, 0xFF, 0xF8, 0xAB, 0x08, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF9, 0xBA, 0x3B, 0xFF, 0xFF, 0xFA, 0xF1, 0xFF, 0xFF, 0xFC, 0xF6, 0xFF, 0xFA, 0xC6, 0x5E, 0xFF, 0xF8, 0xA7, 0x00, 0xFF, 0xFA, 0xBC, 0x3E, 0xFF, 0xFF, 0xF8, 0xEB, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0xE7, 0xBA, 0xFF, 0xF8, 0xAB, 0x08, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF9, 0xBA, 0x3C, 0xFF, 0xFF, 0xFA, 0xF1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xEF, 0xD2, 0xFF, 0xF9, 0xB2, 0x20, 0xFF, 0xF8, 0xA9, 0x02, 0xFF, 0xFC, 0xDB, 0x9A, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0xE7, 0xBA, 0xFF, 0xF8, 0xAB, 0x08, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF9, 0xBA, 0x3C, 0xFF, 0xFF, 0xFA, 0xF1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFB, 0xD3, 0x82, 0xFF, 0xF8, 0xA8, 0x00, 0xFF, 0xFA, 0xBB, 0x3B, 0xFF, 0xFE, 0xF8, 0xEB, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0xE7, 0xBA, 0xFF, 0xF8, 0xAB, 0x08, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF9, 0xBA, 0x3C, 0xFF, 0xFF, 0xFA, 0xF1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xF4, 0xE0, 0xFF, 0xF9, 0xB6, 0x2C, 0xFF, 0xF8, 0xA9, 0x03, 0xFF, 0xFC, 0xDF, 0xA5, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0xE7, 0xBA, 0xFF, 0xF8, 0xAB, 0x08, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF9, 0xBA, 0x3C, 0xFF, 0xFF, 0xFA, 0xF1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFB, 0xD5, 0x86, 0xFF, 0xF8, 0xA7, 0x00, 0xFF, 0xFA, 0xC3, 0x52, 0xFF, 0xFF, 0xFC, 0xF6, 0xFC, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0xE7, 0xBA, 0xFF, 0xF8, 0xAB, 0x08, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF9, 0xBA, 0x3C, 0xFF, 0xFF, 0xFA, 0xF1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xF1, 0xD5, 0xFF, 0xF9, 0xB1, 0x1C, 0xFF, 0xF9, 0xB0, 0x19, 0xFF, 0xFE, 0xED, 0xCD, 0xFC, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0xE7, 0xBA, 0xFF, 0xF8, 0xAB, 0x08, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF9, 0xBA, 0x3B, 0xFF, 0xFD, 0xEC, 0xCC, 0xFF, 0xFF, 0xFA, 0xF2, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0xF9, 0xFF, 0xFA, 0xC2, 0x4F, 0xFF, 0xF8, 0xAB, 0x0E, 0xFF, 0xFD, 0xE8, 0xBF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0xE7, 0xB9, 0xFF, 0xF8, 0xAA, 0x08, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF8, 0xA9, 0x00, 0xFF, 0xF9, 0xBA, 0x3D, 0xFF, 0xFA, 0xC7, 0x60, 0xFF, 0xFA, 0xBE, 0x4A, 0xFF, 0xFC, 0xE1, 0xAC, 0xFF, 0xFE, 0xF7, 0xEA, 0xFF, 0xFF, 0xFE, 0xFE, 0xFF, 0xFB, 0xCD, 0x70, 0xFF, 0xF9, 0xBA, 0x3D, 0xFF, 0xFE, 0xF7, 0xE8, 0xFC, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0xEC, 0xC8, 0xFF, 0xF9, 0xBB, 0x40, 0xFF, 0xF9, 0xBB, 0x3B, 0xFF, 0xF9, 0xBB, 0x3B, 0xFF, 0xF9, 0xBB, 0x3B, 0xFF, 0xF9, 0xBB, 0x3B, 0xFF, 0xF9, 0xBB, 0x3B, 0xFF, 0xF9, 0xBB, 0x3B, 0xFF, 0xF9, 0xBB, 0x3B, 0xFF, 0xF9, 0xBB, 0x3B, 0xFF, 0xF9, 0xBB, 0x3B, 0xFF, 0xF9, 0xBB, 0x3B, 0xFF, 0xF9, 0xBB, 0x3B, 0xFF, 0xF9, 0xBB, 0x3B, 0xFF, 0xF9, 0xBB, 0x3B, 0xFF, 0xF9, 0xBB, 0x3B, 0xFF, 0xF9, 0xBA, 0x38, 0xFF, 0xFB, 0xC8, 0x6A, 0xFF, 0xFA, 0xC7, 0x5F, 0xFF, 0xF8, 0xA6, 0x00, 0xFF, 0xF8, 0xAA, 0x07, 0xFF, 0xF9, 0xB5, 0x2F, 0xFF, 0xFC, 0xDE, 0xA5, 0xFF, 0xFC, 0xDA, 0x99, 0xFF, 0xFD, 0xEB, 0xCB, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFA, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFD, 0xFA, 0xFC, 0xFE, 0xF9, 0xEE, 0xFC, 0xFE, 0xF9, 0xEE, 0xFC, 0xFE, 0xF9, 0xEE, 0xFC, 0xFE, 0xF9, 0xEE, 0xFC, 0xFE, 0xF9, 0xEE, 0xFC, 0xFE, 0xF9, 0xEE, 0xFC, 0xFE, 0xF9, 0xEE, 0xFC, 0xFE, 0xF9, 0xEE, 0xFC, 0xFE, 0xF9, 0xEE, 0xFC, 0xFE, 0xF9, 0xEE, 0xFC, 0xFE, 0xF9, 0xEE, 0xFC, 0xFE, 0xF9, 0xEE, 0xFC, 0xFE, 0xF9, 0xEE, 0xFC, 0xFE, 0xF9, 0xEE, 0xFC, 0xFE, 0xF9, 0xEE, 0xFC, 0xFE, 0xF9, 0xEE, 0xFC, 0xFE, 0xFA, 0xF3, 0xFC, 0xFD, 0xEB, 0xC8, 0xFC, 0xFB, 0xCA, 0x69, 0xFC, 0xF9, 0xBA, 0x37, 0xFC, 0xFA, 0xC1, 0x4F, 0xFC, 0xFD, 0xEC, 0xCA, 0xFC, 0xFF, 0xF9, 0xED, 0xFC, 0xFF, 0xFF, 0xFE, 0xFC, 0xFF, 0xFF, 0xFF, 0xFA, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

OpenTherm mOT(mInPin, mOutPin);
OpenTherm sOT(sInPin, sOutPin, true);

bool _heatingDisable = false;
bool _dhwDisable = false;

void ICACHE_RAM_ATTR mHandleInterrupt() {
  mOT.handleInterrupt();
}

void ICACHE_RAM_ATTR sHandleInterrupt() {
  sOT.handleInterrupt();
}

void notifyClients(String s) {
  ws.textAll(s);
}

float otGetFloat(const unsigned long response) {
  const uint16_t u88 = response & 0xffff;
  const float f = (u88 & 0x8000) ? -(0x10000L - u88) / 256.0f : u88 / 256.0f;
  return f;
}


bool _boilerTempNotify = false;
float _boilerTemp = 0;

bool _dhwTempNotify = false;
float _dhwTemp = 0;

bool _dhwSetNotify = false;
float _dhwSet = 0;

bool _chSetNotify = false;
float _chSet = 0;

unsigned long _lastRresponse;

void processRequest(unsigned long request, OpenThermResponseStatus status) {
  const int msgType = (request << 1) >> 29;
  const int dataId = (request >> 16) & 0xff;

  if (msgType == 0 && dataId == 0) { // read && status flag
    if (_heatingDisable) {
      request &= ~(1 << (0 + 8));
    }
    if (_dhwDisable) {
      request &= ~(1 << (1 + 8));
    }
  }

  String masterRequest = "T" + String(request, HEX);
  notifyClients(masterRequest);
  Serial.println(masterRequest + " " + String(request, BIN));  //master/thermostat request
  _lastRresponse = mOT.sendRequest(request);
  if (_lastRresponse) {
    String slaveResponse = "B" + String(_lastRresponse, HEX);
    Serial.println(slaveResponse); //slave/boiler response
    notifyClients(slaveResponse);
    sOT.sendResponse(_lastRresponse);

    if (msgType == 0 && dataId == 25) { // read && boiler temp
      _boilerTempNotify = true;
      _boilerTemp = otGetFloat(_lastRresponse);
    }
    if (msgType == 0 && dataId == 26) { // read && dhw temp
      _dhwTempNotify = true;
      _dhwTemp = otGetFloat(_lastRresponse);
    }
    if (dataId == 56) { // dhw setpoint
      _dhwSetNotify = true;
      _dhwSet = otGetFloat(_lastRresponse);
    }
    if (dataId == 1) { // ch setpoint
      _chSetNotify = true;
      _chSet = otGetFloat(_lastRresponse);
    }
  }
}

const char* PARAM_MESSAGE = "message";

WiFiClient  client;

bool ledState = false;

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    if (strcmp((char*)data, "toggle") == 0) {
      ledState = !ledState;
      //notifyClients();
    }
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
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

String processor(const String& var) {
  Serial.println(var);
  if (var == "STATE") {
    if (ledState) {
      return "ON";
    }
    else {
      return "OFF";
    }
  }
}


void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found " + request->url());
  Serial.println("Not found " + request->url());
}

String htmlVarProcessor(const String& var)
{
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
    Serial.printf("WiFi Failed!\n");
    return;
  }

  Serial.println("SPIFFS...");
  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/html/index.html", String(), false, htmlVarProcessor);
    //request->send_P(200, "text/html", index_html, htmlVarProcessor);
  });

  server.on("/heating-false", HTTP_GET, [](AsyncWebServerRequest * request) {
    Serial.println("Heating disable override");
    request->send(200, "text/plain", "OK: heating off");
   _heatingDisable = true;
  });

  server.on("/heating-true", HTTP_GET, [](AsyncWebServerRequest * request) {
    Serial.println("Heating enable");
    request->send(200, "text/plain", "OK: heating on");
    _heatingDisable = false;
  });

  server.on("/dhw-false", HTTP_GET, [](AsyncWebServerRequest * request) {
    Serial.println("Domestic hot water disable override");
    request->send(200, "text/plain", "OK: dhw off");
    _dhwDisable = true;
  });

  server.on("/dhw-true", HTTP_GET, [](AsyncWebServerRequest * request) {
    Serial.println("Domestic hot water enable");
    request->send(200, "text/plain", "OK: dhw on");
    _dhwDisable = false;
  });

  server.on("/otgw-core.js", HTTP_GET, [](AsyncWebServerRequest * request) {
    AsyncWebServerResponse* response = request->beginResponse(SPIFFS, "/otgw-core.js");
    response->addHeader("cache-control", "max-age=7776000");
    request->send(response);
  });

  server.on("/styles.css", HTTP_GET, [](AsyncWebServerRequest * request) {
    AsyncWebServerResponse* response = request->beginResponse(SPIFFS, "/styles.css");
    response->addHeader("cache-control", "max-age=7776000");
    request->send(response);
  });

  server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest * request) {
    //AsyncWebServerResponse* response = request->beginResponse(SPIFFS, "/favicon.ico");
    AsyncWebServerResponse* response = request->beginResponse_P(200, "image/x-icon", favico_ico, favico_ico_length);
    response->addHeader("cache-control", "max-age=7776000");
    request->send(response);
  });

  // Send a GET request to <IP>/get?message=<message>
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest * request) {
    String message;
    if (request->hasParam(PARAM_MESSAGE)) {
      message = request->getParam(PARAM_MESSAGE)->value();
    } else {
      message = "No message sent";
    }
    request->send(200, "text/plain", "Hello, GET: " + message);
  });

  // Send a POST request to <IP>/post with a form field message set to <message>
  server.on("/post", HTTP_POST, [](AsyncWebServerRequest * request) {
    String message;
    if (request->hasParam(PARAM_MESSAGE, true)) {
      message = request->getParam(PARAM_MESSAGE, true)->value();
    } else {
      message = "No message sent";
    }
    request->send(200, "text/plain", "Hello, POST: " + message);
  });

  server.onNotFound(notFound);

  server.addHandler(new SPIFFSEditor(SPIFFS, "open", "therm"));
  server.begin();

  ThingSpeak.begin(client);

  initWebSocket();

  mOT.begin(mHandleInterrupt);
  sOT.begin(sHandleInterrupt, processRequest);

#ifdef ESP32
  esp_task_wdt_init(10, true); //enable panic so ESP32 restarts
  esp_task_wdt_add(NULL); //add current thread to WDT watch
#endif

}

int _thingSpeakUpd = 0;

void loop() {
  esp_task_wdt_reset();
  sOT.process();
  ws.cleanupClients();

  if (_boilerTempNotify) {
    _boilerTempNotify = false;
    notifyClients("B:" + String(_boilerTemp));
  }
  if (_dhwTempNotify) {
    _dhwTempNotify = false;
    notifyClients("D:" + String(_dhwTemp));
  }
  if (_dhwSetNotify) {
    _dhwSetNotify = false;
    notifyClients("F:" + String(_dhwSet));
  }
  if (_chSetNotify) {
    _chSetNotify = false;
    notifyClients("G:" + String(_chSet));
  }

  if (_thingSpeakUpd < millis()) {
    _thingSpeakUpd = millis() + 10000;

    ThingSpeak.setField(1, _boilerTemp);
    ThingSpeak.setField(2, _chSet);
    ThingSpeak.setField(3, mOT.getModulation());
    ThingSpeak.setField(4, mOT.isFlameOn(_lastRresponse));

    int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
    if (x == 200) {
      Serial.println("Channel update successful.");
    }
    else {
      Serial.println("Problem updating channel. HTTP error code " + String(x));
    }
  }
}
