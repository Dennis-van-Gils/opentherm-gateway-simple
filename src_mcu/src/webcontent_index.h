#include <Arduino.h>

// clang-format off
  const char index_html[] PROGMEM = R"rawliteral(
  <script type="text/javascript">
    var ipAddr = "`IP_ADDR`";
  </script>
  <!DOCTYPE html>
  <html lang="en">

  <head>
    <meta charset="UTF-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0" />
    <meta http-equiv="X-UA-Compatible" content="ie=edge" />
    <meta name="color-scheme" content="dark" />
    <title>OpenTherm Gateway</title>
    <link href="../styles.css" rel="stylesheet" />
    <script src="../otgw-core.js" async></script>
  </head>

  <body>
    <div class="wrapr">
      <table style="width: 100%">
        <tr>
          <td style="width: auto" colspan="2">
            <font size="+1"><b>Room</b></font>
          </td>
          <td style="width: auto" colspan="2">
            <font size="+1"><b>Hot Water</b></font>
          </td>
        </tr>
        <tr>
          <td class="label-column">Temperature, &#x00B0;C</td>
          <td style="width: auto">
            <div id="lbl_Tr">--</div>
          </td>
          <td class="label-column">Temperature, &#x00B0;C</td>
          <td style="width: auto">
            <div id="lbl_Tdhw">--</div>
          </td>
        </tr>
        <tr>
          <td class="label-column">Setpoint, &#x00B0;C</td>
          <td style="width: auto">
            <div id="lbl_TrSet">--</div>
          </td>
          <td class="label-column">Setpoint, &#x00B0;C</td>
          <td style="width: auto">
            <div id="lbl_TdhwSet">--</div>
          </td>
        </tr>
        <tr class="blank_row">
          <td colspan="4"></td>
        </tr>
        <tr>
          <td style="width: auto" colspan="2">
            <font size="+1"><b>Central Heating</b></font>
          </td>
          <td style="width: auto" colspan="2">
            <font size="+1"><b>Boiler Status</b></font>
          </td>
        </tr>
        <tr>
          <td class="label-column">Boiler temperature, &#x00B0;C</td>
          <td style="width: auto">
            <div id="lbl_Tboiler">--</div>
          </td>
          <td class="label-column">Service</td>
          <td style="width: auto">
            <label class="switch">
              <input type="checkbox" id="LED_DiagInd" disabled />
              <span class="slider round"></span>
            </label>
          </td>
        </tr>
        <tr>
          <td class="label-column">Setpoint, &#x00B0;C</td>
          <td style="width: auto">
            <div id="lbl_TSet">--</div>
          </td>
          <td class="label-column">Fault</td>
          <td style="width: auto">
            <label class="switch">
              <input type="checkbox" id="LED_Fault" disabled />
              <span class="slider round"></span>
            </label>
          </td>
        </tr>
        <tr>
          <td class="label-column">Max setpoint, &#x00B0;C</td>
          <td style="width: auto">
            <div id="lbl_MaxTSet">--</div>
          </td>
          <td class="label-column">Central heating</td>
          <td style="width: auto">
            <label class="switch">
              <input type="checkbox" id="LED_CHmode" disabled />
              <span class="slider round"></span>
            </label>
          </td>
        </tr>
        <tr>
          <td class="label-column">Modulation, &#37;</td>
          <td style="width: auto">
            <div id="lbl_RelModLevel">--</div>
          </td>
          <td class="label-column">Hot water</td>
          <td style="width: auto">
            <label class="switch">
              <input type="checkbox" id="LED_DHWmode" disabled />
              <span class="slider round"></span>
            </label>
          </td>
        </tr>
        <tr>
          <td class="label-column">
          </td>
          <td style="width: auto;"></td>
          <td class="label-column">Flame on</td>
          <td style="width: auto">
            <label class="switch">
              <input type="checkbox" id="LED_FlameStatus" disabled />
              <span class="slider round"></span>
            </label>
          </td>
        </tr>
      </table>
    </div>

    <div class="wrapr center">
      <textarea style="height: 400px; width: 100%; min-width: 100%" id="commands-log"></textarea>
    </div>
  </body>

  </html>
  )rawliteral";