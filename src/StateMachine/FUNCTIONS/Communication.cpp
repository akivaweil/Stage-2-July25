//* ************************************************************************
//* ************************ COMMUNICATION ******************************
//* ************************************************************************
// Functions for WiFi connectivity and status reporting

#include <Stage2_Machine.h>

//* ************************************************************************
//* ************************ WIFI SETUP *********************************
//* ************************************************************************

void setupWiFi() {
    Serial.println("Connecting to WiFi...");
    
    // Configure static IP for reliable OTA access
    IPAddress local_IP(192, 168, 1, 239);
    IPAddress gateway(192, 168, 1, 1);
    IPAddress subnet(255, 255, 255, 0);
    IPAddress primaryDNS(8, 8, 8, 8);
    IPAddress secondaryDNS(8, 8, 4, 4);
    
    // Configure static IP
    if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
        Serial.println("Static IP configuration failed");
    }
    
    // Start WiFi connection
    WiFi.begin(Config::WIFI_SSID, Config::WIFI_PASSWORD);
    
    // Wait for connection with timeout
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        Serial.print(".");
        attempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("");
        Serial.println("WiFi connected successfully!");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
        Serial.print("Board ID: ");
        Serial.println(Config::BOARD_ID);
        Serial.print("Description: ");
        Serial.println(Config::BOARD_DESCRIPTION);
        Serial.println("OTA Ready - Use: pio run -e freenove_esp32_s3_wroom_ota -t upload");
    } else {
        Serial.println("");
        Serial.println("WiFi connection failed - continuing without network");
    }
}

//* ************************************************************************
//* ************************ STATUS REPORTING ***************************
//* ************************************************************************

void sendStatus() {
    // Send current machine status
    Serial.println("=== MACHINE STATUS ===");
    Serial.print("State: ");
    Serial.println(currentState);
    Serial.print("Position: ");
    Serial.print(stepsToInches(currentPosition));
    Serial.println(" inches");
    Serial.print("Homing Complete: ");
    Serial.println(homingComplete ? "YES" : "NO");
    Serial.print("Cycle In Progress: ");
    Serial.println(cycleInProgress ? "YES" : "NO");
    Serial.print("Emergency Stop: ");
    Serial.println(emergencyStop ? "YES" : "NO");
    
    // Input status
    Serial.println("--- INPUT STATUS ---");
    Serial.print("Home Switch: ");
    Serial.println(homeSwitch.read() ? "ACTIVE" : "INACTIVE");
    Serial.print("Start Button: ");
    Serial.println(startButton.read() ? "ACTIVE" : "INACTIVE");
    Serial.print("Transfer Signal: ");
    Serial.println(transferArmSignal.read() ? "ACTIVE" : "INACTIVE");
    
    Serial.println("======================");
} 