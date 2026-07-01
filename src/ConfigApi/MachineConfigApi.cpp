#include "ConfigApi/MachineConfigApi.h"
#include "ConfigApi/MachineSettings.h"

#include <ArduinoJson.h>
#include <WiFi.h>

#include "Config/Config.h"
#include "Config/Pins_Definitions.h"
#include "StateMachine/StateMachine.h"   // currentState, SystemState
#include "OTA/OTA_Upload.h"              // isRouterClear

// MACHINE CONFIG API IMPLEMENTATION

namespace {
constexpr char MACHINE_ID[]   = "stage2";
constexpr char MACHINE_NAME[] = "Stage 2";
constexpr int  CONFIG_SCHEMA  = 1;

// Editable-field descriptor. `target` points at the persisted slot in
// machineSettings; `isInt` distinguishes the single integer field.
struct FieldDef {
    const char* key;
    const char* label;
    bool        isInt;
    float       minVal;
    float       maxVal;
    float       step;
    // Accessors into the persisted struct.
    float (*get)();
    void  (*set)(float);
    // Dashboard grouping / unit-conversion metadata.
    const char* group;      // section heading
    bool        fromSteps;  // true => stored in steps; dashboard shows in/s via stepsPerInch
    bool        collapsed;  // true => group section starts collapsed
};

// Position fields: 0-30 in, step 0.1. ALIGNMENT short forward: 0-2 in.
// Speeds: per recon (generous ranges around the defaults). Cooldown: 0-5000 ms.
const FieldDef FIELDS[] = {
    { "APPROACH_POSITION", "Approach Position (in)", false, 0.0f, 30.0f, 0.1f,
      [] { return machineSettings.approachPosition; },
      [](float v) { machineSettings.approachPosition = v; },
      "Positions", false, false },

    { "CUTTING_POSITION", "Cutting Position (in)", false, 0.0f, 30.0f, 0.1f,
      [] { return machineSettings.cuttingPosition; },
      [](float v) { machineSettings.cuttingPosition = v; },
      "Positions", false, false },

    { "FINAL_POSITION", "Final Position (in)", false, 0.0f, 30.0f, 0.1f,
      [] { return machineSettings.finalPosition; },
      [](float v) { machineSettings.finalPosition = v; },
      "Positions", false, false },

    { "ALIGNMENT_SHORT_FORWARD_POSITION", "Alignment Short Forward (in)", false, 0.0f, 2.0f, 0.1f,
      [] { return machineSettings.alignmentShortForwardPosition; },
      [](float v) { machineSettings.alignmentShortForwardPosition = v; },
      "Positions", false, false },

    { "CUTTING_SPEED", "Cutting Speed (in/s)", false, 1.0f, 105000.0f, 1.0f,
      [] { return machineSettings.cuttingSpeed; },
      [](float v) { machineSettings.cuttingSpeed = v; },
      "Motors", true, true },

    { "APPROACH_SPEED", "Approach Speed (in/s)", false, 1.0f, 200000.0f, 1.0f,
      [] { return machineSettings.approachSpeed; },
      [](float v) { machineSettings.approachSpeed = v; },
      "Motors", true, true },

    { "RETURN_SPEED", "Return Speed (in/s)", false, 1.0f, 200000.0f, 1.0f,
      [] { return machineSettings.returnSpeed; },
      [](float v) { machineSettings.returnSpeed = v; },
      "Motors", true, true },

    { "HOMING_SPEED", "Homing Speed (in/s)", false, 1.0f, 105000.0f, 1.0f,
      [] { return machineSettings.homingSpeed; },
      [](float v) { machineSettings.homingSpeed = v; },
      "Motors", true, true },

    { "FORWARD_ACCEL", "Forward Accel (in/s^2)", false, 677.0f, 300000.0f, 1.0f,
      [] { return machineSettings.forwardAccel; },
      [](float v) { machineSettings.forwardAccel = v; },
      "Motors", true, true },

    { "RETURN_ACCEL", "Return Accel (in/s^2)", false, 677.0f, 300000.0f, 1.0f,
      [] { return machineSettings.returnAccel; },
      [](float v) { machineSettings.returnAccel = v; },
      "Motors", true, true },

    { "CYCLE_COOLDOWN_MS", "Cycle Cooldown (ms)", true, 0.0f, 5000.0f, 1.0f,
      [] { return (float)machineSettings.cycleCooldownMs; },
      [](float v) { machineSettings.cycleCooldownMs = (int)lroundf(v); },
      "Timing", false, false },
};
constexpr size_t FIELD_COUNT = sizeof(FIELDS) / sizeof(FIELDS[0]);

const FieldDef* findField(const char* key) {
    for (size_t i = 0; i < FIELD_COUNT; ++i) {
        if (strcmp(FIELDS[i].key, key) == 0) return &FIELDS[i];
    }
    return nullptr;
}

const char* currentStateName() {
    switch (currentState) {
        case STATE_IDLE:               return "IDLE";
        case STATE_HOMING:             return "HOMING";
        case STATE_ALIGNMENT:          return "ALIGNMENT";
        case STATE_CUTTING:            return "CUTTING";
        case STATE_RETURNING:          return "RETURNING";
        case STATE_ROUTER_CLEAR_ERROR: return "ROUTER_CLEAR_ERROR";
    }
    return "UNKNOWN";
}
}  // namespace

volatile bool configDirty = false;

// SAFETY GATE

bool isSafeToApplyConfig() {
    return currentState == STATE_IDLE;
}

void applyConfigIfDirty() {
    if (configDirty) {
        applySettings();
        configDirty = false;
    }
}

// GET /api/status

String buildStatusJson() {
    JsonDocument doc;
    doc["id"]       = MACHINE_ID;
    doc["name"]     = MACHINE_NAME;
    doc["state"]    = currentStateName();
    // HEALTHY normally; WARNING until homed; ERROR while in the router-clear error state.
    const char* health = "HEALTHY";
    if (currentState == STATE_ROUTER_CLEAR_ERROR) {
        health = "ERROR";
    } else if (!homingComplete) {
        health = "WARNING";
    }
    doc["health"]   = health;
    doc["uptimeMs"] = (uint32_t)millis();
    doc["freeHeap"] = (uint32_t)ESP.getFreeHeap();
    doc["rssi"]     = (int)WiFi.RSSI();

    JsonObject sensors = doc["sensors"].to<JsonObject>();
    sensors["homeSwitch"]        = digitalRead(Pins::HOME_SWITCH) == HIGH;                     // active HIGH
    sensors["startButton"]       = digitalRead(Pins::START_BUTTON) == HIGH;                    // active HIGH
    sensors["transferArm"]       = digitalRead(Pins::TRANSFER_ARM_START_SIGNAL) == HIGH;       // active HIGH
    sensors["endPosition"]       = digitalRead(Pins::END_POSITION_VERIFICATION_SENSOR) == LOW; // active LOW
    sensors["routerClearSensor"] = digitalRead(Pins::IS_ROUTER_CLEAR) == LOW;                  // active LOW
    sensors["routerClearEspNow"] = isRouterClear;
    sensors["homingComplete"]    = homingComplete;

    String out;
    serializeJson(doc, out);
    return out;
}

// GET /api/config

String buildConfigJson() {
    JsonDocument doc;
    doc["id"]           = MACHINE_ID;
    doc["schema"]       = CONFIG_SCHEMA;
    doc["stepsPerInch"] = Motion::STEPS_PER_INCH;

    JsonArray fields = doc["fields"].to<JsonArray>();
    for (size_t i = 0; i < FIELD_COUNT; ++i) {
        const FieldDef& f = FIELDS[i];
        JsonObject obj = fields.add<JsonObject>();
        obj["key"]   = f.key;
        obj["label"] = f.label;
        obj["type"]  = f.isInt ? "int" : "float";
        if (f.isInt) {
            obj["value"] = (int)lroundf(f.get());
            obj["min"]   = (int)lroundf(f.minVal);
            obj["max"]   = (int)lroundf(f.maxVal);
            obj["step"]  = (int)lroundf(f.step);
        } else {
            obj["value"] = f.get();
            obj["min"]   = f.minVal;
            obj["max"]   = f.maxVal;
            obj["step"]  = f.step;
        }
        obj["group"] = f.group;
        if (f.fromSteps) obj["fromSteps"] = true;
        if (f.collapsed) obj["collapsed"] = true;
    }

    String out;
    serializeJson(doc, out);
    return out;
}

// POST /api/config CORE

bool applyConfigJson(const String& body, bool& outDeferred, String& outMsg) {
    outDeferred = false;

    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, body);
    if (err) {
        outMsg = "invalid JSON";
        return false;
    }

    JsonObject root = doc.as<JsonObject>();
    if (root.isNull()) {
        outMsg = "body must be a flat object";
        return false;
    }

    // Validate EVERYTHING first; change nothing until all keys pass.
    for (JsonPair kv : root) {
        const char* key = kv.key().c_str();
        const FieldDef* f = findField(key);
        if (!f) {
            outMsg = String(key) + " invalid/out of range";
            return false;
        }
        if (!kv.value().is<float>() && !kv.value().is<int>()) {
            outMsg = String(key) + " invalid/out of range";
            return false;
        }
        float v = kv.value().as<float>();
        if (v < f->minVal || v > f->maxVal) {
            outMsg = String(key) + " invalid/out of range";
            return false;
        }
    }

    // All valid: write into the persisted struct.
    for (JsonPair kv : root) {
        const FieldDef* f = findField(kv.key().c_str());
        f->set(kv.value().as<float>());
    }

    // ALWAYS persist accepted values immediately.
    saveSettings();

    // Apply live now if safe; otherwise defer to next IDLE entry.
    if (isSafeToApplyConfig()) {
        applySettings();
        configDirty = false;
        outDeferred = false;
        outMsg = "saved";
    } else {
        configDirty = true;
        outDeferred = true;
        outMsg = "pending — applies at next idle";
    }
    return true;
}

// ROUTE REGISTRATION (SYNC WebServer)

namespace {
void sendJson(WebServer& server, int code, const String& body) {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(code, "application/json", body);
}
}  // namespace

void setupConfigApi(WebServer& server) {
    server.on("/api/status", HTTP_GET, [&server]() {
        sendJson(server, 200, buildStatusJson());
    });

    server.on("/api/config", HTTP_GET, [&server]() {
        sendJson(server, 200, buildConfigJson());
    });

    server.on("/api/config", HTTP_POST, [&server]() {
        // POST arrives as Content-Type text/plain (CORS simple request); body in "plain".
        String body = server.arg("plain");
        bool deferred = false;
        String msg;
        bool ok = applyConfigJson(body, deferred, msg);

        JsonDocument doc;
        doc["ok"] = ok;
        if (ok) {
            doc["applied"]  = !deferred;
            doc["deferred"] = deferred;
        }
        doc["message"] = msg;

        String out;
        serializeJson(doc, out);
        sendJson(server, ok ? 200 : 400, out);
    });
}
