#include "OTA_Upload.h"
#include <WiFi.h>
#include <ArduinoOTA.h>
#include <esp_now.h>
#include <esp_wifi.h>

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
  
  Serial.println("OTA server started - ready for wireless uploads");
}

void handleOTA() {
  ArduinoOTA.handle();
} 