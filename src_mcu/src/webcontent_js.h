#include <Arduino.h>

// clang-format off
const char js[] PROGMEM = R"rawliteral(
init();

Date.prototype.addHours = function(h) {
  this.setTime(this.getTime() + (h*60*60*1000));
  return this;
}

function init() {
  if (ipAddr === "`IP_ADDR`" || ipAddr === "") {
    alert("Gateway ip address is unknown");
  }
}

//var gateway = `ws://${window.location.hostname}/ws`;
var gateway = `ws://${ipAddr}/ws`;
var websocket;

function initWebSocket() {
  console.log("Trying to open a WebSocket connection...");
  websocket = new WebSocket(gateway);
  websocket.onopen = onOpen;
  websocket.onclose = onClose;
  websocket.onmessage = onMessage; // <-- add this line
}

function onOpen(event) {
  console.log("Connection opened");
}

function onClose(event) {
  console.log("Connection closed");
  setTimeout(initWebSocket, 2000);
}

function onLoad(event) {
  initWebSocket();
}

function formatDate(date, s = ' ') {
  var d = date,
    month = "" + (d.getMonth() + 1),
    day = "" + d.getDate(),
    year = d.getFullYear(),
    hour = "" + d.getHours(),
    min = "" + d.getMinutes(),
    sec = "" + d.getSeconds();

  if (month.length < 2) month = "0" + month;
  if (day.length < 2) day = "0" + day;
  if (hour.length < 2) hour = "0" + hour;
  if (min.length < 2) min = "0" + min;
  if (sec.length < 2) sec = "0" + sec;

  return `${year}-${month}-${day}${s}${hour}:${min}:${sec}`;
}

function formatTime(date, s = ' ') {
  var d = date,
    hour = "" + d.getHours(),
    min = "" + d.getMinutes(),
    sec = "" + d.getSeconds();

  if (hour.length < 2) hour = "0" + hour;
  if (min.length < 2) min = "0" + min;
  if (sec.length < 2) sec = "0" + sec;

  return `${hour}:${min}:${sec}`;
}

function pad(pad, str, padLeft) {
  if (typeof str === 'undefined')
    return pad;
  if (padLeft) {
    return (pad + str).slice(-pad.length);
  } else {
    return (str + pad).substring(0, pad.length);
  }
}

function dec2flag8(dec) {
  return `b` + pad("00000000", (dec >>> 0).toString(2), true);
}

function transform_reading(dataId, dataValue) {
  /* TODO: Specific for Remeha, read here:
  https://github.com/rvdbreemen/OTGW-firmware/blob/main/Specification/New%20OT%20data-ids.txt
  https://www.opentherm.eu/request-details/?post_ids=1833
  */
  switch(dataId) {
    // f8.8, rounded to 2 decimals
    case 1: case 7: case 8: case 9: case 14: case 16: case 17: case 18: case 19:
    case 23: case 24: case 25: case 26: case 27: case 28: case 29: case 30:
    case 31: case 32: case 56: case 57: case 58:
      u88 = dataValue & 0xffff;
      ans = (u88 & 0x8000) ? -(0x10000 - u88) / 256.0 : u88 / 256.0;
      ans = (Math.round(ans * 100) / 100).toFixed(2);
      break;

    // f8.8, not rounded
    case 124: case 125:
      u88 = dataValue & 0xffff;
      ans = (u88 & 0x8000) ? -(0x10000 - u88) / 256.0 : u88 / 256.0;
      break;

    // u8 / u8
    case 4: case 10: case 11: case 12: case 13: case 15: case 21: case 126:
    case 127:
      ans = `(` +
        ((dataValue >> 8) & 0xff) + `, ` +
        (dataValue & 0xff) + `)`;
      break;

    // s8 / s8
    case 48: case 49: case 50:
      ans = `(` +
        ((dataValue >> 8) & 0xff - 0x80) + `, ` +
        (dataValue & 0xff - 0x80) + `)`;
      break;

    // flag8 / flag8
    case 0: case 6:
      ans =
        dec2flag8((dataValue >> 8) & 0xff) + ` ` +
        dec2flag8(dataValue & 0xff);
      break;

    // flag8 / -
    case 100:
      ans = dec2flag8(dataValue & 0xff); // Operates on the low-byte
      break;

    // flag8 / u8
    case 2: case 3: case 5:
      ans = `(` +
        dec2flag8((dataValue >> 8) & 0xff) + `, ` +
        (dataValue & 0xff) + `)`;
      break;

    // u16
    case 22: case 115: case 116: case 117: case 118: case 119: case 120:
    case 121: case 122: case 123:
      ans = dataValue;
      break;

    // s16
    case 33:
      ans = dataValue - 0x8000;
      break;

    // Day of Week & Time of Day
    case 20:
      if (dataValue === 0) {
        ans = 0;
      } else {
        switch((dataValue >> 13) & 0xff) {
          case 1:
            ans = `Monday `;
            break;
          case 2:
            ans = `Tuesday `;
            break;
          case 3:
            ans = `Wednesday `;
            break;
          case 4:
            ans = `Thursday `;
            break;
          case 5:
            ans = `Friday `;
            break;
          case 6:
            ans = `Saturday `;
            break;
          case 7:
            ans = `Sunday `;
            break;
          default:
            ans = ``;
        }
        ans = ans +
          pad(`00`, (dataValue >> 8) & 0x1f, true) + `:` +
          pad(`00`, dataValue & 0xff, true);
      }
      break;

    // unknown
    default:
      ans = dataValue + ' (?)';
  }

  return ans;
}

function onMessage(event) {
  const msgData = event.data;
  console.log(`${msgData}`);

  var date = new Date();
  var date_str = formatTime(date);

  var text = document.getElementById("commands-log");
  var log = text.value;
  log = log.substring(log.length - 500000);

  const msgKind = msgData.slice(0, 2);
  if (msgKind == "!!") {
    text.value = log + date_str + ` !! RESET TRIGGERED\r\n`;
  } else if (msgKind == "Fr") { /* Free heap notification: Skip */
  } else {
    const numberData = msgData.slice(1);
    const int = parseInt(Number(`0x${numberData}`), 10);
    const data = int & (~(1<<31));
    const msgType = data >> 28;
    const dataId = (data >> 16) & 0xff;
    const dataValue = data & 65535;
    var unpacked_reading = transform_reading(dataId, dataValue);

    const msgTypeStr = OpenThermMessageType[msgType];
    const dataIdStr = OpenThermMessageID[dataId];
    text.value = log + date_str + ` ` +
      pad(` `.repeat(15), msgTypeStr, false) + ` | ` +
      pad(` `.repeat(3) , dataId, false) + ` | ` +
      pad(` `.repeat(22), dataIdStr, false) + ` | ` +
      unpacked_reading + `\r\n`;

    if (msgType == 4 && dataId == 0)
    {
      document.getElementById("LED_Fault").checked = ((dataValue & 1) != 0);
      document.getElementById("LED_CHmode").checked = ((dataValue & (1<<1)) != 0);
      document.getElementById("LED_DHWmode").checked = ((dataValue & (1<<2)) != 0);
      document.getElementById("LED_FlameStatus").checked = ((dataValue & (1<<3)) != 0);
      document.getElementById("LED_DiagInd").checked = ((dataValue & (1<<6)) != 0);
    }

    if (msgType == 4 || msgType == 5 || msgType == 6)
    {
      if (dataId == 1)  {document.getElementById("lbl_TSet").innerText = unpacked_reading;}
      if (dataId == 16) {document.getElementById("lbl_TrSet").innerText = unpacked_reading;}
      if (dataId == 17) {document.getElementById("lbl_RelModLevel").innerText = unpacked_reading;}
      if (dataId == 24) {document.getElementById("lbl_Tr").innerText = unpacked_reading;}
      if (dataId == 25) {document.getElementById("lbl_Tboiler").innerText = unpacked_reading;}
      if (dataId == 26) {document.getElementById("lbl_Tdhw").innerText = unpacked_reading;}
      if (dataId == 56) {document.getElementById("lbl_TdhwSet").innerText = unpacked_reading;}
      if (dataId == 57) {document.getElementById("lbl_MaxTSet").innerText = unpacked_reading;}
    }
  }

  text.scrollTop = text.scrollHeight;
}

window.addEventListener("load", onLoad);

var OpenThermMessageType = [];
/* Master to Slave */
OpenThermMessageType[0] = "READ_DATA";
OpenThermMessageType[1] = "WRITE_DATA";
OpenThermMessageType[2] = "INVALID_DATA";
OpenThermMessageType[3] = "RESERVED";
/* Slave to Master */
OpenThermMessageType[4] = "READ_ACK";
OpenThermMessageType[5] = "WRITE_ACK";
OpenThermMessageType[6] = "DATA_INVALID";
OpenThermMessageType[7] = "UNKNOWN_DATA_ID";

var OpenThermMessageID = [];
OpenThermMessageID[0] = "Status"; // flag8 / flag8  Master and Slave Status flags.
OpenThermMessageID[1] = "TSet"; // f8.8  Control setpoint  ie CH  water temperature setpoint (°C)
OpenThermMessageID[2] = "MConfigMMemberIDcode"; // flag8 / u8  Master Configuration Flags /  Master MemberID Code
OpenThermMessageID[3] = "SConfigSMemberIDcode"; // flag8 / u8  Slave Configuration Flags /  Slave MemberID Code
OpenThermMessageID[4] = "Command"; // u8 / u8  Remote Command
OpenThermMessageID[5] = "ASFflags"; // / OEM-fault-code  flag8 / u8  Application-specific fault flags and OEM fault code
OpenThermMessageID[6] = "RBPflags"; // flag8 / flag8  Remote boiler parameter transfer-enable & read/write flags
OpenThermMessageID[7] = "CoolingControl"; // f8.8  Cooling control signal (%)
OpenThermMessageID[8] = "TsetCH2"; // f8.8  Control setpoint for 2e CH circuit (°C)
OpenThermMessageID[9] = "TrOverride"; // f8.8  Remote override room setpoint
OpenThermMessageID[10] = "TSP"; // u8 / u8  Number of Transparent-Slave-Parameters supported by slave
OpenThermMessageID[11] = "TSPindexTSPvalue"; // u8 / u8  Index number / Value of referred-to transparent slave parameter.
OpenThermMessageID[12] = "FHBsize"; // u8 / u8  Size of Fault-History-Buffer supported by slave
OpenThermMessageID[13] = "FHBindexFHBvalue"; // u8 / u8  Index number / Value of referred-to fault-history buffer entry.
OpenThermMessageID[14] = "MaxRelModLevelSetting"; // f8.8  Maximum relative modulation level setting (%)
OpenThermMessageID[15] = "MaxCapacityMinModLevel"; // u8 / u8  Maximum boiler capacity (kW) / Minimum boiler modulation level(%)
OpenThermMessageID[16] = "TrSet"; // f8.8  Room Setpoint (°C)
OpenThermMessageID[17] = "RelModLevel"; // f8.8  Relative Modulation Level (%)
OpenThermMessageID[18] = "CHPressure"; // f8.8  Water pressure in CH circuit  (bar)
OpenThermMessageID[19] = "DHWFlowRate"; // f8.8  Water flow rate in DHW circuit. (litres/minute)
OpenThermMessageID[20] = "DayTime"; // special / u8  Day of Week and Time of Day
OpenThermMessageID[21] = "Date"; // u8 / u8  Calendar date
OpenThermMessageID[22] = "Year"; // u16  Calendar year
OpenThermMessageID[23] = "TrSetCH2"; // f8.8  Room Setpoint for 2nd CH circuit (°C)
OpenThermMessageID[24] = "Tr"; // f8.8  Room temperature (°C)
OpenThermMessageID[25] = "Tboiler"; // f8.8  Boiler flow water temperature (°C)
OpenThermMessageID[26] = "Tdhw"; // f8.8  DHW temperature (°C)
OpenThermMessageID[27] = "Toutside"; // f8.8  Outside temperature (°C)
OpenThermMessageID[28] = "Tret"; // f8.8  Return water temperature (°C)
OpenThermMessageID[29] = "Tstorage"; // f8.8  Solar storage temperature (°C)
OpenThermMessageID[30] = "Tcollector"; // f8.8  Solar collector temperature (°C)
OpenThermMessageID[31] = "TflowCH2"; // f8.8  Flow water temperature CH2 circuit (°C)
OpenThermMessageID[32] = "Tdhw2"; // f8.8  Domestic hot water temperature 2 (°C)
OpenThermMessageID[33] = "Texhaust"; // s16  Boiler exhaust temperature (°C)
OpenThermMessageID[48] = "TdhwSetUBTdhwSetLB"; //= 48, // s8 / s8  DHW setpoint upper & lower bounds for adjustment  (°C)
OpenThermMessageID[49] = "MaxTSetUBMaxTSetLB"; // s8 / s8  Max CH water setpoint upper & lower bounds for adjustment  (°C)
OpenThermMessageID[50] = "HcratioUBHcratioLB"; // s8 / s8  OTC heat curve ratio upper & lower bounds for adjustment
OpenThermMessageID[56] = "TdhwSet"; //= 56, // f8.8  DHW setpoint (°C)    (Remote parameter 1)
OpenThermMessageID[57] = "MaxTSet"; // f8.8  Max CH water setpoint (°C)  (Remote parameters 2)
OpenThermMessageID[58] = "Hcratio"; // f8.8  OTC heat curve ratio (°C)  (Remote parameter 3)
OpenThermMessageID[100] = "RemoteOverrideFunction"; //= 100, // flag8 / -  Function of manual and program changes in master and remote room setpoint.
OpenThermMessageID[115] = "OEMDiagnosticCode"; //= 115, // u16  OEM-specific diagnostic/service code
OpenThermMessageID[116] = "BurnerStarts"; // u16  Number of starts burner
OpenThermMessageID[117] = "CHPumpStarts"; // u16  Number of starts CH pump
OpenThermMessageID[118] = "DHWPumpValveStarts"; // u16  Number of starts DHW pump/valve
OpenThermMessageID[119] = "DHWBurnerStarts"; // u16  Number of starts burner during DHW mode
OpenThermMessageID[120] = "BurnerOperationHours"; // u16  Number of hours that burner is in operation (i.e. flame on)
OpenThermMessageID[121] = "CHPumpOperationHours"; // u16  Number of hours that CH pump has been running
OpenThermMessageID[122] = "DHWPumpValveOperationHours"; // u16  Number of hours that DHW pump has been running or DHW valve has been opened
OpenThermMessageID[123] = "DHWBurnerOperationHours"; // u16  Number of hours that burner is in operation during DHW mode
OpenThermMessageID[124] = "OpenThermVersionMaster"; // f8.8  The implemented version of the OpenTherm Protocol Specification in the master.
OpenThermMessageID[125] = "OpenThermVersionSlave"; // f8.8  The implemented version of the OpenTherm Protocol Specification in the slave.
OpenThermMessageID[126] = "MasterVersion"; // u8 / u8  Master product version number and type
OpenThermMessageID[127] = "SlaveVersion"; // u8 / u8  Slave product version number and type
)rawliteral";