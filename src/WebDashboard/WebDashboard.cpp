#include "WebDashboard/WebDashboard.h"

#include <Arduino.h>
#include <WebServer.h>

#include "Config/Pins_Definitions.h"
#include "StateMachine/StateMachine.h"
#include "OTA/OTA_Upload.h"   // isRouterClear

// Web dashboard implementation.
// WiFi connection, OTA updates, ESP-NOW router communication live in OTA_Upload;
// this file owns only the human-facing status dashboard ("/" + "/status").

// The shared sync server is owned by OTA_Upload; captured here at setup so the
// route handlers can send responses on it.
static WebServer* dashboardServerPtr = nullptr;

static const char DASHBOARD_HTML[] = R"rawliteral(<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Stage 2 Dashboard</title>
  <style>
    *{box-sizing:border-box;margin:0;padding:0}
    body{font-family:monospace;background:#0d0d1a;color:#e0e0e0;padding:20px}
    h1{color:#00d4ff;font-size:1.5em;margin-bottom:4px}
    .sub{color:#555;font-size:0.75em;margin-bottom:24px}
    .section{margin-bottom:22px}
    .sec-title{color:#666;text-transform:uppercase;font-size:0.65em;letter-spacing:2px;margin-bottom:10px;border-bottom:1px solid #1e1e3a;padding-bottom:4px}
    .row{display:flex;align-items:center;gap:12px;margin-bottom:9px}
    .dot{width:13px;height:13px;border-radius:50%;flex-shrink:0;transition:background 0.2s,box-shadow 0.2s}
    .on{background:#00ff88;box-shadow:0 0 8px #00ff8888}
    .off{background:#2a2a3a}
    .lbl{font-size:0.85em}
    .pin{color:#444;font-size:0.7em;margin-left:auto}
    .state{display:inline-block;padding:5px 18px;border:1px solid #00d4ff;border-radius:3px;color:#00d4ff;font-size:1em;letter-spacing:1px}
  </style>
</head>
<body>
  <h1>Stage 2 Dashboard</h1>
  <div class="sub">Polling every 500ms</div>

  <div class="section">
    <div class="sec-title">Machine State</div>
    <span class="state" id="state">—</span>
  </div>

  <div class="section">
    <div class="sec-title">Switches &mdash; Active HIGH</div>
    <div class="row"><div class="dot off" id="homeSwitch"></div><span class="lbl">Home Switch</span><span class="pin">PIN 1</span></div>
    <div class="row"><div class="dot off" id="startButton"></div><span class="lbl">Start Button</span><span class="pin">PIN 2</span></div>
    <div class="row"><div class="dot off" id="transferArm"></div><span class="lbl">Transfer Arm Start Signal</span><span class="pin">PIN 47</span></div>
  </div>

  <div class="section">
    <div class="sec-title">Sensors &mdash; Active LOW</div>
    <div class="row"><div class="dot off" id="endPosition"></div><span class="lbl">End Position Verification Sensor</span><span class="pin">PIN 35</span></div>
    <div class="row"><div class="dot off" id="routerClearSensor"></div><span class="lbl">Router Clear Sensor</span><span class="pin">PIN 39</span></div>
  </div>

  <div class="section">
    <div class="sec-title">ESP-NOW</div>
    <div class="row"><div class="dot off" id="routerClearEspNow"></div><span class="lbl">Router Clear (received)</span></div>
  </div>

  <script>
    var keys = ['homeSwitch','startButton','transferArm','endPosition','routerClearSensor','routerClearEspNow'];
    function update() {
      fetch('/status').then(function(r){return r.json();}).then(function(d){
        document.getElementById('state').textContent = d.state;
        keys.forEach(function(k){
          var el = document.getElementById(k);
          el.className = 'dot ' + (d[k] ? 'on' : 'off');
        });
      }).catch(function(){});
    }
    update();
    setInterval(update, 500);
  </script>
</body>
</html>)rawliteral";

void handleDashboardRoot() {
    dashboardServerPtr->send(200, "text/html", DASHBOARD_HTML);
}

void handleDashboardStatus() {
    // Read physical pin states interpreted by active logic
    bool homeActive       = digitalRead(Pins::HOME_SWITCH) == HIGH;                        // active HIGH
    bool startActive      = digitalRead(Pins::START_BUTTON) == HIGH;                       // active HIGH
    bool transferActive   = digitalRead(Pins::TRANSFER_ARM_START_SIGNAL) == HIGH;          // active HIGH
    bool endPosActive     = digitalRead(Pins::END_POSITION_VERIFICATION_SENSOR) == LOW;    // active LOW
    bool routerSensorActive = digitalRead(Pins::IS_ROUTER_CLEAR) == LOW;                   // active LOW

    const char* stateName = "UNKNOWN";
    switch (currentState) {
        case STATE_IDLE:               stateName = "IDLE";               break;
        case STATE_HOMING:             stateName = "HOMING";             break;
        case STATE_ALIGNMENT:          stateName = "ALIGNMENT";          break;
        case STATE_CUTTING:            stateName = "CUTTING";            break;
        case STATE_RETURNING:          stateName = "RETURNING";          break;
        case STATE_ROUTER_CLEAR_ERROR: stateName = "ROUTER_CLEAR_ERROR"; break;
    }

    String json = "{";
    json += "\"homeSwitch\":"         + String(homeActive         ? "true" : "false") + ",";
    json += "\"startButton\":"        + String(startActive        ? "true" : "false") + ",";
    json += "\"transferArm\":"        + String(transferActive     ? "true" : "false") + ",";
    json += "\"endPosition\":"        + String(endPosActive       ? "true" : "false") + ",";
    json += "\"routerClearSensor\":"  + String(routerSensorActive ? "true" : "false") + ",";
    json += "\"routerClearEspNow\":"  + String(isRouterClear      ? "true" : "false") + ",";
    json += "\"state\":\""            + String(stateName)         + "\"";
    json += "}";

    dashboardServerPtr->send(200, "application/json", json);
}

void setupWebDashboard(WebServer& server) {
    dashboardServerPtr = &server;
    server.on("/", handleDashboardRoot);
    server.on("/status", handleDashboardStatus);
}
