#include <Arduino.h>

// clang-format off
const char css[] PROGMEM = R"rawliteral(
* {
  font-family: monospace;
}

body {
  height: 100vh;
}

#html {
  display: none;
}

.wrapr {
  padding-top: 10px;
  padding-left: 10px;
  padding-bottom: 10px;
  padding-right: 10px;
  background: #282828;
  border: none;
  border-radius: 12px;
  max-width: 650px;
  margin: 20px auto;
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
  background-color: #a7a1b2;
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
  background-color: #3bd5ef;
}

input:focus + .slider {
  box-shadow: 0 0 1px #3bd5ef;
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

.blank_row
{
  height: 10px !important;
}
)rawliteral";