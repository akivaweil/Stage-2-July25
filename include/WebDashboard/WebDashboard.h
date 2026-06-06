#pragma once
#include <Arduino.h>
#include <WebServer.h>

// Web dashboard: serves the live status HTML page ("/") and the JSON status
// feed ("/status") used by that page. Registered on the shared sync WebServer
// owned by OTA_Upload; served from loop() via dashboardServer.handleClient().

// Register the dashboard routes ("/" and "/status") on the given sync server.
void setupWebDashboard(WebServer& server);

// Route handlers (registered by setupWebDashboard).
void handleDashboardRoot();
void handleDashboardStatus();
