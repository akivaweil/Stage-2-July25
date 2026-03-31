#include "OTA_Upload.h"
#include <WiFi.h>
#include <ArduinoOTA.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <WebServer.h>

//* ************************************************************************
//* *********************** OTA UPLOAD IMPLEMENTATION *********************
//* ************************************************************************
// WiFi connection, OTA updates, and ESP-NOW router communication.

const char* ssid = "Everwood";
const char* password = "Everwood-Staff";

//* ************************************************************************
//* *********************** ESP-NOW ROUTER COMM ************************
//* ************************************************************************

// Config
static const float MAX_SEND_RETRIES     = 5;   // Max retry attempts on delivery failure
static const float SEND_ACK_TIMEOUT_MS  = 15;  // ms to wait for MAC-layer ACK before giving up
static const float SEND_RETRY_DELAY_MS  = 20;  // ms between retry attempts

uint8_t routerMAC[] = {0xE4, 0xB0, 0x63, 0xB4, 0x49, 0x50};

typedef struct { uint8_t signal; } RouterMessage;

// True = router is clear, False = router is NOT clear (triggers error state)
volatile bool isRouterClear = true;

// Send callback tracking - set by onDataSent() after each esp_now_send()
volatile bool lastSendAckReceived = false;
volatile bool lastSendSuccess     = false;

void onDataSent(const uint8_t *mac, esp_now_send_status_t status) {
    // Called by ESP-NOW stack after MAC-layer ACK (or failure) from peer
    lastSendSuccess     = (status == ESP_NOW_SEND_SUCCESS);
    lastSendAckReceived = true;
}

void onRouterDataReceived(const uint8_t *mac, const uint8_t *data, int len) {
    if (len >= 1) {
        isRouterClear = (data[0] == 1);
    }
}

void sendRouterSignal(uint8_t value) {
    RouterMessage msg;
    msg.signal = value;

    for (float attempt = 0; attempt < MAX_SEND_RETRIES; attempt++) {
        lastSendAckReceived = false;
        lastSendSuccess     = false;

        esp_now_send(routerMAC, (uint8_t*)&msg, sizeof(msg));

        // Wait for MAC-layer ACK callback (or timeout)
        unsigned long start = millis();
        while (!lastSendAckReceived && (millis() - start) < (unsigned long)SEND_ACK_TIMEOUT_MS) {
            delayMicroseconds(100);
        }

        if (lastSendSuccess) {
            return; // Delivery confirmed - no need to retry
        }

        // Delivery failed or timed out - wait before next attempt
        delay((unsigned long)SEND_RETRY_DELAY_MS);
    }
}

//* ************************************************************************
//* *********************** WEB DASHBOARD ******************************
//* ************************************************************************

WebServer webServer(80);

void handleDashboard() {
    String mac = WiFi.macAddress();
    char routerMACStr[18];
    snprintf(routerMACStr, sizeof(routerMACStr), "%02X:%02X:%02X:%02X:%02X:%02X",
        routerMAC[0], routerMAC[1], routerMAC[2],
        routerMAC[3], routerMAC[4], routerMAC[5]);

    String html = R"rawhtml(<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <meta http-equiv="refresh" content="3">
  <title>Stage 2 Dashboard</title>
  <style>
    * { box-sizing: border-box; margin: 0; padding: 0; }
    body { font-family: 'Segoe UI', sans-serif; background: #0f1117; color: #e0e0e0; min-height: 100vh; display: flex; flex-direction: column; align-items: center; justify-content: center; padding: 24px; }
    h1 { font-size: 1.4rem; font-weight: 600; letter-spacing: 0.05em; color: #ffffff; margin-bottom: 24px; text-transform: uppercase; }
    .grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(260px, 1fr)); gap: 16px; width: 100%; max-width: 800px; }
    .card { background: #1a1d27; border: 1px solid #2a2d3a; border-radius: 12px; padding: 20px 24px; }
    .card-label { font-size: 0.72rem; text-transform: uppercase; letter-spacing: 0.1em; color: #6b7280; margin-bottom: 8px; }
    .card-value { font-size: 1.1rem; font-weight: 600; font-family: 'Courier New', monospace; color: #60a5fa; word-break: break-all; }
    .card-value.green { color: #34d399; }
    .card-value.red   { color: #f87171; }
    .card-value.gray  { color: #9ca3af; }
    .footer { margin-top: 20px; font-size: 0.72rem; color: #374151; }
  </style>
</head>
<body>
  <h1>Stage 2 Machine</h1>
  <div class="grid">
    <div class="card">
      <div class="card-label">This Device MAC (Stage 2)</div>
      <div class="card-value">)rawhtml";
    html += mac;
    html += R"rawhtml(</div>
    </div>
    <div class="card">
      <div class="card-label">Registered Router MAC</div>
      <div class="card-value">)rawhtml";
    html += routerMACStr;
    html += R"rawhtml(</div>
    </div>
    <div class="card">
      <div class="card-label">IP Address</div>
      <div class="card-value gray">)rawhtml";
    html += WiFi.localIP().toString();
    html += R"rawhtml(</div>
    </div>
    <div class="card">
      <div class="card-label">WiFi Signal (RSSI)</div>
      <div class="card-value gray">)rawhtml";
    html += String(WiFi.RSSI());
    html += R"rawhtml( dBm</div>
    </div>
    <div class="card">
      <div class="card-label">Router Clear Status</div>
      <div class="card-value )rawhtml";
    html += isRouterClear ? "green\">CLEAR" : "red\">NOT CLEAR";
    html += R"rawhtml(</div>
    </div>
  </div>
  <div class="footer">Auto-refreshes every 3 seconds</div>
</body>
</html>)rawhtml";

    webServer.send(200, "text/html", html);
}

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
  esp_now_register_send_cb(onDataSent);

  // Register router as a peer
  esp_now_peer_info_t peer = {};
  memcpy(peer.peer_addr, routerMAC, 6);
  peer.channel = 0;  // 0 = use current WiFi channel
  peer.encrypt = false;
  esp_now_add_peer(&peer);

  Serial.println("ESP-NOW initialized");

  ArduinoOTA.setHostname("stage2-esp32s3");
  ArduinoOTA.begin();

  webServer.on("/", handleDashboard);
  webServer.begin();

  Serial.println("OTA server started - ready for wireless uploads");
  Serial.print("Dashboard: http://");
  Serial.println(WiFi.localIP());
}

void handleOTA() {
  ArduinoOTA.handle();
  webServer.handleClient();
} 