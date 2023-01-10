#include <Arduino.h>

// clang-format off
const char index_html[] PROGMEM = R"rawliteral(
<script type="text/javascript">
  var ipAddr = "`IP_ADDR`";
  var readToken = "`READ_TOKEN`";
  var channelId = "`CHANNEL_ID`";
</script>
<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="UTF-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0" />
    <meta http-equiv="X-UA-Compatible" content="ie=edge" />
    <title>OpenTherm Gateway</title>

    <link href="../styles.css" rel="stylesheet" />

    <script src="https://cdn.jsdelivr.net/npm/apexcharts"></script>
    <script src="../otgw-core.js" async></script>
  </head>

  <body>
    <div class="wrapr">
      <table style="width: 100%" id="idd">
        <tr>
          <button onclick="refreshCharts()">Refresh charts</button>
        </tr>
        <tr>
          <td class="label-column">From</td>
          <td style="width: auto">
            <input type="datetime-local" id="date-from" />
          </td>
          <td class="label-column">Room setpoint, &#x00B0;C</td>
          <td style="width: auto">
            <div id="lbl_TrSet">--</div>
          </td>
        </tr>
        <tr>
          <td class="label-column">To</td>
          <td style="width: auto">
            <input type="datetime-local" id="date-to" />
          </td>
          <td class="label-column">Room temperature, &#x00B0;C</td>
          <td style="width: auto">
            <div id="lbl_Tr">--</div>
          </td>
        </tr>
      </table>
      <table style="width: 100%" id="idd">
        <tr>
          <td class="label-column">Central Heating setpoint, &#x00B0;C</td>
          <td style="width: auto">
            <div id="lbl_TSet">--</div>
          </td>
          <td class="label-column">Hot Water setpoint, &#x00B0;C</td>
          <td style="width: auto">
            <div id="lbl_TdhwSet">--</div>
          </td>
        </tr>
        <tr>
          <td class="label-column">Boiler temperature, &#x00B0;C</td>
          <td style="width: auto">
            <div id="lbl_Tboiler">--</div>
          </td>
          <td class="label-column">Hot Water temperature, &#x00B0;C</td>
          <td style="width: auto">
            <div id="lbl_Tdhw">--</div>
          </td>
        </tr>
        <tr>
          <td class="label-column">Central Heating enable</td>
          <td style="width: auto">
            <label class="switch">
              <input type="checkbox" id="heatingEnableInput" disabled />
              <span class="slider round"></span>
            </label>
          </td>
          <td class="label-column">Hot Water enable</td>
          <td style="width: auto">
            <label class="switch">
              <input type="checkbox" id="dhwEnableInput" disabled />
              <span class="slider round"></span>
            </label>
          </td>
        </tr>
      </table>
      <table>
        <tr>
          <td class="label-column">Boiler fault</td>
          <td style="width: auto">
            <label class="switch">
              <input type="checkbox" id="LED_Fault" disabled />
              <span class="slider round"></span>
            </label>
          </td>
        </tr>
        <tr>
          <td class="label-column">Boiler CH mode</td>
          <td style="width: auto">
            <label class="switch">
              <input type="checkbox" id="LED_CHmode" disabled />
              <span class="slider round"></span>
            </label>
          </td>
        </tr>
        <tr>
          <td class="label-column">Boiler DHW mode</td>
          <td style="width: auto">
            <label class="switch">
              <input type="checkbox" id="LED_DHWmode" disabled />
              <span class="slider round"></span>
            </label>
          </td>
        </tr>
        <tr>
          <td class="label-column">Boiler Flame status</td>
          <td style="width: auto">
            <label class="switch">
              <input type="checkbox" id="LED_FlameStatus" disabled />
              <span class="slider round"></span>
            </label>
          </td>
        </tr>
        <tr>
          <td class="label-column">Boiler Diag. ind.</td>
          <td style="width: auto">
            <label class="switch">
              <input type="checkbox" id="LED_DiagInd" disabled />
              <span class="slider round"></span>
            </label>
          </td>
        </tr>
      </table>
    </div>
    <div class="wrapr center" style="height: 1000px" id="chart-container">
      <div id="waiting-indicator" style="text-align: center; margin: 0 auto">LOADING...</div>
      Flame on/off
      <div id="chart-Flame"></div>
      Central Heating setpoint, &#x00B0;C
      <div id="chart-Tset"></div>
      Boiler temperature, &#x00B0;C
      <div id="chart-Tboiler"></div>
      Modulation level, &#x0025;
      <div id="chart-RelModLevel"></div>
      Room temperature, &#x00B0;C
      <div id="chart-Tr"></div>
    </div>

    <div class="wrapr center">
      <textarea style="height: 150px; width: 100%; min-width: 100%" id="commands-log"></textarea>
    </div>
  </body>
</html>
)rawliteral";
