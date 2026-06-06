#pragma once
#include <Arduino.h>
#include <WebServer.h>

// MACHINE CONFIG API (SHARED DASHBOARD REST CONTRACT)
// Implements the canonical cross-machine REST config + status API for Stage 2.
// Stage 2 uses the SYNC WebServer (WebServer.h), so handlers run from loop()
// via dashboardServer.handleClient() and MUST be short and non-blocking.
//
// Routes (all responses set Access-Control-Allow-Origin: *):
//   GET  /api/status  -> buildStatusJson()
//   GET  /api/config  -> buildConfigJson()
//   POST /api/config  -> applyConfigJson(server.arg("plain"), ...)

// Register the three /api/* routes on the given sync server.
void setupConfigApi(WebServer& server);

// Build the GET /api/status response body (live, read-only).
String buildStatusJson();

// Build the GET /api/config response body (self-describing field list).
String buildConfigJson();

// Core of POST /api/config. Parses the flat key->value JSON body, validates each
// key (known + within [min,max]), always persists accepted values, then either
// applies live now (outDeferred=false) or sets configDirty (outDeferred=true).
// Returns false (and leaves everything unchanged) on unknown/out-of-range key.
bool applyConfigJson(const String& body, bool& outDeferred, String& outMsg);

// True only when it is safe to mutate live motion variables (motionless IDLE
// only; HOMING actively drives the steppers, so it defers).
bool isSafeToApplyConfig();

// Set when a POST arrived mid-cycle; the main loop applies it on next IDLE entry.
extern volatile bool configDirty;

// Called from the main loop on entry to IDLE: if configDirty, applySettings()
// and clear the flag.
void applyConfigIfDirty();
