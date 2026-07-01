#include "OTA/OTA_Upload.h"

#include <WiFi.h>
#include <ArduinoOTA.h>
#include <esp_task_wdt.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <WebServer.h>

#include "Config/Pins_Definitions.h"
#include "StateMachine/StateMachine.h"
#include "ConfigApi/MachineConfigApi.h"
#include "ConfigApi/MachineSettings.h"
#include "WebDashboard/WebDashboard.h"

// OTA upload implementation.
// WiFi connection, OTA updates, and ESP-NOW router communication. The human web
// dashboard lives in WebDashboard; the shared REST config API lives in ConfigApi.

const char* ssid = "Everwood";
const char* password = "Everwood-Staff";

// ESP-NOW router comm

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

// Shared sync web server. Owns the dashboard ("/", "/status"), the REST config
// API ("/api/*"), and is serviced from handleOTA() each loop.
WebServer dashboardServer(80);

// OTA + WiFi setup

void setupOTA() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  // Bounded connect attempt, then proceed so the machine still boots and runs
  // its cutting cycle on locally-saved NVS settings if the network/TA is down.
  // WiFi keeps retrying in the background after the loop falls through.
  const uint32_t WIFI_CONNECT_TIMEOUT_MS = 15000;
  const uint32_t WIFI_CONNECT_POLL_MS = 250;
  WiFi.setAutoReconnect(true);
  uint32_t wifiConnectStart = millis();
  while (WiFi.status() != WL_CONNECTED &&
         millis() - wifiConnectStart < WIFI_CONNECT_TIMEOUT_MS) {
    delay(WIFI_CONNECT_POLL_MS);
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("[Stage2] wifi ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("[Stage2] wifi unavailable - standalone");
  }

  // Max TX power for strongest signal
  esp_wifi_set_max_tx_power(84);

  // Init ESP-NOW (requires WiFi to be connected first)
  if (esp_now_init() != ESP_OK) {
    return;
  }

  esp_now_register_recv_cb(onRouterDataReceived);

  // Register router as a peer
  esp_now_peer_info_t peer = {};
  memcpy(peer.peer_addr, routerMAC, 6);
  peer.channel = 0;  // 0 = use current WiFi channel
  peer.encrypt = false;
  esp_now_add_peer(&peer);

  // Load dashboard-editable settings from NVS and apply them to the live globals
  // before the cutting cycle can run (overrides compile-time defaults).
  loadSettings();

  // Web dashboard ("/", "/status").
  setupWebDashboard(dashboardServer);
  // Shared cross-machine REST config + status API (/api/status, /api/config).
  setupConfigApi(dashboardServer);
  dashboardServer.begin();

  ArduinoOTA.setHostname("stage2-esp32s3");
  ArduinoOTA.onStart([]() {
    Serial.println("[Stage2] OTA start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("[Stage2] OTA done");
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("[Stage2] OTA error %u\n", error);
  });
  // Feed the watchdog during an upload: ArduinoOTA.handle() blocks for the whole
  // transfer in a single loop iteration, which would otherwise trip the WDT.
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    esp_task_wdt_reset();
  });
  ArduinoOTA.begin();
}

void handleOTA() {
  if (currentState == STATE_IDLE) {
    ArduinoOTA.handle();
  }
  dashboardServer.handleClient();
}
