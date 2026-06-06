#pragma once
#include <Arduino.h>

// MACHINE SETTINGS (NVS-PERSISTED, DASHBOARD-EDITABLE)
// Owns the persisted copy of the dashboard-editable motion/timing settings for
// Stage 2. Persistence backend is NVS via the Preferences library, namespace
// "dashcfg". On first boot (sentinel absent) the struct is seeded from the
// current compile-time defaults in Config.cpp and persisted.

struct MachineSettings {
    // Positions (inches)
    float approachPosition;
    float cuttingPosition;
    float finalPosition;
    float alignmentShortForwardPosition;
    // Speeds (steps/sec)
    float cuttingSpeed;
    float approachSpeed;
    float returnSpeed;
    float homingSpeed;
    // Timing (ms)
    int   cycleCooldownMs;
};

// In-RAM copy of the persisted settings.
extern MachineSettings machineSettings;

// Load persisted settings from NVS into machineSettings, then applySettings().
// On first boot, seeds machineSettings from the live compile-time globals and
// persists them. Call once after hardware init.
void loadSettings();

// Persist the current machineSettings struct to NVS.
void saveSettings();

// Copy machineSettings into the live (de-consted) Config globals.
void applySettings();
