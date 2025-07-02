#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

//* ************************************************************************
//* *********************** OTA MANAGER IMPLEMENTATION *********************
//* ************************************************************************
// Handles WiFi connection and Over-The-Air updates for the ESP32.
// All OTA functionality is contained in this single file.

const char* ssid = "Everwood";
const char* password = "Everwood-Staff";

// Global variables
bool otaEnabled = false;

// Function declarations
void setupOTA();
void handleOTA();

void setupOTA() {
    // Initialize WiFi in station mode
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    
    // Non-blocking WiFi connection with shorter timeout
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 10) {
        delay(250);
        attempts++;
    }
    
    // Check WiFi connection status and report
    if (WiFi.status() == WL_CONNECTED) {
        Serial.print("WiFi connected! IP address: ");
        Serial.println(WiFi.localIP());
        
        // Set hostname for OTA identification
        ArduinoOTA.setHostname("stage2-esp32s3");

    // Configure OTA callbacks
    ArduinoOTA
        .onStart([]() {
            String type;
            if (ArduinoOTA.getCommand() == U_FLASH) {
                type = "sketch";
            } else { // U_SPIFFS
                type = "filesystem";
            }
            // NOTE: if updating SPIFFS, ensure SPIFFS is mounted via SPIFFS.begin()
        })
        .onEnd([]() {
            // OTA upload completed
        })
        .onProgress([](unsigned int progress, unsigned int total) {
            // Progress indication could be added here if needed
        })
        .onError([](ota_error_t error) {
            // Error handling for OTA failures
            if (error == OTA_AUTH_ERROR) {
                // Authentication failed
            } else if (error == OTA_BEGIN_ERROR) {
                // Begin failed
            } else if (error == OTA_CONNECT_ERROR) {
                // Connect failed
            } else if (error == OTA_RECEIVE_ERROR) {
                // Receive failed
            } else if (error == OTA_END_ERROR) {
                // End failed
            }
        });

        // Start OTA service
        ArduinoOTA.begin();
        Serial.println("OTA service started");
        otaEnabled = true;
    } else {
        Serial.println("WiFi connection failed - OTA disabled");
        Serial.println("Machine will continue normal operation without OTA");
        otaEnabled = false;
    }
}

void handleOTA() {
    // Only handle OTA if it was successfully enabled
    if (otaEnabled && WiFi.status() == WL_CONNECTED) {
        ArduinoOTA.handle();
    }
} 