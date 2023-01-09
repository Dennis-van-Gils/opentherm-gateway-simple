#include <Arduino.h>

// clang-format off
const char css[] PROGMEM = R"rawliteral(
* {
  font-family: monospace;
}

body {
  height: 100vh;
  background: #f9f9f9;
}

select.flat-select {
  -moz-appearance: none;
  -webkit-appearance: none;
  appearance: none;
  background: #008ffb
    url("data:image/svg+xml;utf8,<svg xmlns='http://www.w3.org/2000/svg' width='60px' height='60px'><polyline fill='white' points='46.139,15.518 25.166,36.49 4.193,15.519'/></svg>")
    no-repeat scroll right 2px top 9px / 16px 16px;
  border: 0 none;
  border-radius: 3px;
  color: #fff;
  font-family: arial, tahoma;
  font-size: 16px;
  font-weight: bold;
  outline: 0 none;
  height: 33px;
  padding: 5px 20px 5px 10px;
  text-align: center;
  text-indent: 0.01px;
  text-overflow: "";
  text-shadow: 0 -1px 0 rgba(0, 0, 0, 0.25);
  transition: all 0.3s ease 0s;
  width: auto;
  -webkit-transition: 0.3s ease all;
  -moz-transition: 0.3s ease all;
  -ms-transition: 0.3s ease all;
  -o-transition: 0.3s ease all;
  transition: 0.3s ease all;
}
select.flat-select:focus,
select.flat-select:hover {
  border: 0;
  outline: 0;
}

.apexcharts-canvas {
  margin: 0 auto;
}

#html {
  display: none;
}

.wrapr {
  padding-top: 20px;
  padding-left: 10px;
  padding-bottom: 20px;
  padding-right: 10px;
  background: #fff;
  border: 1px solid #ddd;
  box-shadow: 0 22px 35px -16px rgba(0, 0, 0, 0.1);
  max-width: 650px;
  margin: 35px auto;
}

.center {
  display: flex;
  align-items: center;
  justify-content: center;
}

.label-column {
  white-space: nowrap;
  width: auto;
}

.switch {
  position: relative;
  display: inline-block;
  width: 36px;
  height: 18px;
}

.switch input {
  opacity: 0;
  width: 0;
  height: 0;
}

.slider {
  position: absolute;
  cursor: pointer;
  top: 0;
  left: 0;
  right: 0;
  bottom: 0;
  background-color: #ccc;
  -webkit-transition: 0.4s;
  transition: 0.4s;
}

.slider:before {
  position: absolute;
  content: "";
  height: 12px;
  width: 12px;
  left: 4px;
  bottom: 3px;
  background-color: white;
  -webkit-transition: 0.4s;
  transition: 0.4s;
}

input:checked + .slider {
  background-color: #2196f3;
}

input:focus + .slider {
  box-shadow: 0 0 1px #2196f3;
}

input:checked + .slider:before {
  -webkit-transform: translateX(16px);
  -ms-transform: translateX(16px);
  transform: translateX(16px);
}

.slider.round {
  border-radius: 24px;
}

.slider.round:before {
  border-radius: 50%;
}
)rawliteral";