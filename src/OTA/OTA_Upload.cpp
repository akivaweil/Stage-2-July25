#include "OTA_Upload.h"
#include <WiFi.h>
#include <ArduinoOTA.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <Pins_Definitions.h>
#include <WebServer.h>
#include <Stage2_Machine.h>

//* ************************************************************************
//* *********************** OTA UPLOAD IMPLEMENTATION *********************
//* ************************************************************************
// WiFi connection, OTA updates, ESP-NOW router communication, and web dashboard.

const char* ssid = "Everwood";
const char* password = "Everwood-Staff";

//* ************************************************************************
//* *********************** ESP-NOW ROUTER COMM ************************
//* ************************************************************************

uint8_t routerMAC[] = {0xE4, 0xB0, 0x63, 0xB4, 0x49, 0x50};

typedef struct { uint8_t signal; } RouterMessage;

// True = router is clear, False = router is NOT clear (triggers error state)
volatile bool isRouterClear = true;

void onRouterDataReceived(const uint8_t *mac, const uint8_t *data, int len) {
    if (len >= 1) {
        isRouterClear = (data[0] == 1);
    }
}

void sendRouterSignal(uint8_t value) {
    RouterMessage msg;
    msg.signal = value;
    // Physical signal on pin 17 mirrors the ESP-NOW signal
    digitalWrite(Pins::ROUTER_START_SIGNAL, value ? HIGH : LOW);
    // Send 3x for redundancy in case of packet loss
    esp_now_send(routerMAC, (uint8_t*)&msg, sizeof(msg));
    delay(5);
    esp_now_send(routerMAC, (uint8_t*)&msg, sizeof(msg));
    delay(5);
    esp_now_send(routerMAC, (uint8_t*)&msg, sizeof(msg));
}

//* ************************************************************************
//* *********************** WEB DASHBOARD ******************************
//* ************************************************************************

WebServer dashboardServer(80);

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
    dashboardServer.send(200, "text/html", DASHBOARD_HTML);
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
        case IDLE:               stateName = "IDLE";               break;
        case HOMING:             stateName = "HOMING";             break;
        case ALIGNMENT:          stateName = "ALIGNMENT";          break;
        case CUTTING:            stateName = "CUTTING";            break;
        case RETURNING:          stateName = "RETURNING";          break;
        case ROUTER_CLEAR_ERROR: stateName = "ROUTER_CLEAR_ERROR"; break;
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

    dashboardServer.send(200, "application/json", json);
}

//* ************************************************************************
//* *********************** OTA + WIFI SETUP ***************************
//* ************************************************************************

void setupOTA() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    delay(5000);
    ESP.restart();
  }

  Serial.print("WiFi connected to: ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("MAC address: ");
  Serial.println(WiFi.macAddress());
  Serial.print("Signal strength (RSSI): ");
  Serial.print(WiFi.RSSI());
  Serial.println(" dBm");

  // Max TX power for strongest signal
  esp_wifi_set_max_tx_power(84);

  // Init ESP-NOW (requires WiFi to be connected first)
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    return;
  }

  esp_now_register_recv_cb(onRouterDataReceived);

  // Register router as a peer
  esp_now_peer_info_t peer = {};
  memcpy(peer.peer_addr, routerMAC, 6);
  peer.channel = 0;  // 0 = use current WiFi channel
  peer.encrypt = false;
  esp_now_add_peer(&peer);

  Serial.println("ESP-NOW initialized");

  // Web dashboard
  dashboardServer.on("/", handleDashboardRoot);
  dashboardServer.on("/status", handleDashboardStatus);
  dashboardServer.begin();
  Serial.print("Dashboard: http://");
  Serial.println(WiFi.localIP());

  ArduinoOTA.setHostname("stage2-esp32s3");
  ArduinoOTA.begin();
  
  Serial.println("OTA server started - ready for wireless uploads");
}

void handleOTA() {
  ArduinoOTA.handle();
  dashboardServer.handleClient();
}
