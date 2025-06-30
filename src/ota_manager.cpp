//* ************************************************************************
//* ************************ OTA MANAGER ********************************
//* ************************************************************************
// Over-the-Air update manager for Stage 2 cutting machine

#include <Stage2_Machine.h>
#include <ArduinoOTA.h>

// OTA status flag
bool otaInProgress = false;

//* ************************************************************************
//* ************************ OTA SETUP **********************************
//* ************************************************************************

void setupOTA() {
    // Configure ArduinoOTA
    ArduinoOTA.setHostname("Stage2-Saw-Machine");
    ArduinoOTA.setPassword("Everwood-OTA");
    
    ArduinoOTA.onStart([]() {
        otaInProgress = true;
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH) {
            type = "sketch";
        } else { // U_SPIFFS
            type = "filesystem";
        }
        Serial.println("Start updating " + type);
    });
    
    ArduinoOTA.onEnd([]() {
        otaInProgress = false;
        Serial.println("\nOTA Update complete");
    });
    
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    
    ArduinoOTA.onError([](ota_error_t error) {
        otaInProgress = false;
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) {
            Serial.println("Auth Failed");
        } else if (error == OTA_BEGIN_ERROR) {
            Serial.println("Begin Failed");
        } else if (error == OTA_CONNECT_ERROR) {
            Serial.println("Connect Failed");
        } else if (error == OTA_RECEIVE_ERROR) {
            Serial.println("Receive Failed");
        } else if (error == OTA_END_ERROR) {
            Serial.println("End Failed");
        }
    });
    
    ArduinoOTA.begin();
    
    Serial.println("OTA Manager initialized");
    Serial.print("OTA available at IP: ");
    Serial.println(WiFi.localIP());
}

//* ************************************************************************
//* ************************ OTA HANDLER ********************************
//* ************************************************************************

void handleOTA() {
    // Handle ArduinoOTA
    ArduinoOTA.handle();
}

//* ************************************************************************
//* ************************ OTA STATUS *********************************
//* ************************************************************************

bool isOTAInProgress() {
    return otaInProgress;
} 