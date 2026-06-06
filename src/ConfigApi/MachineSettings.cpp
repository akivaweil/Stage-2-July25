#include "ConfigApi/MachineSettings.h"
#include <Preferences.h>
#include "Config/Config.h"

// MACHINE SETTINGS IMPLEMENTATION

namespace {
// NVS namespace + per-key names. Keys must stay <= 15 chars for Preferences.
constexpr char NVS_NAMESPACE[] = "dashcfg";
constexpr char KEY_MAGIC[]     = "magic";
constexpr uint32_t MAGIC_VALUE = 0x53324347;  // 'S2CG' — bump if struct layout changes

Preferences prefs;
}  // namespace

MachineSettings machineSettings;

// Seed the in-RAM struct from the current compile-time defaults (the live globals
// still hold their initial values at first call).
static void seedFromDefaults() {
    machineSettings.approachPosition              = Motion::APPROACH_POSITION;
    machineSettings.cuttingPosition               = Motion::CUTTING_POSITION;
    machineSettings.finalPosition                 = Motion::FINAL_POSITION;
    machineSettings.alignmentShortForwardPosition = Motion::ALIGNMENT_SHORT_FORWARD_POSITION;
    machineSettings.cuttingSpeed                  = Motion::CUTTING_SPEED;
    machineSettings.approachSpeed                 = Motion::APPROACH_SPEED;
    machineSettings.returnSpeed                   = Motion::RETURN_SPEED;
    machineSettings.homingSpeed                   = Motion::HOMING_SPEED;
    machineSettings.cycleCooldownMs               = Timing::CYCLE_COOLDOWN_MS;
}

void saveSettings() {
    prefs.begin(NVS_NAMESPACE, false);  // read-write
    prefs.putFloat("approachPos",  machineSettings.approachPosition);
    prefs.putFloat("cuttingPos",   machineSettings.cuttingPosition);
    prefs.putFloat("finalPos",     machineSettings.finalPosition);
    prefs.putFloat("alignShortPos", machineSettings.alignmentShortForwardPosition);
    prefs.putFloat("cuttingSpd",   machineSettings.cuttingSpeed);
    prefs.putFloat("approachSpd",  machineSettings.approachSpeed);
    prefs.putFloat("returnSpd",    machineSettings.returnSpeed);
    prefs.putFloat("homingSpd",    machineSettings.homingSpeed);
    prefs.putInt("cooldownMs",     machineSettings.cycleCooldownMs);
    prefs.putUInt(KEY_MAGIC,       MAGIC_VALUE);
    prefs.end();
}

void loadSettings() {
    prefs.begin(NVS_NAMESPACE, true);  // read-only
    uint32_t magic = prefs.getUInt(KEY_MAGIC, 0);

    if (magic != MAGIC_VALUE) {
        // First boot (or layout change): seed from compile-time defaults & persist.
        prefs.end();
        seedFromDefaults();
        saveSettings();
        applySettings();
        return;
    }

    machineSettings.approachPosition              = prefs.getFloat("approachPos",  Motion::APPROACH_POSITION);
    machineSettings.cuttingPosition               = prefs.getFloat("cuttingPos",   Motion::CUTTING_POSITION);
    machineSettings.finalPosition                 = prefs.getFloat("finalPos",     Motion::FINAL_POSITION);
    machineSettings.alignmentShortForwardPosition = prefs.getFloat("alignShortPos", Motion::ALIGNMENT_SHORT_FORWARD_POSITION);
    machineSettings.cuttingSpeed                  = prefs.getFloat("cuttingSpd",   Motion::CUTTING_SPEED);
    machineSettings.approachSpeed                 = prefs.getFloat("approachSpd",  Motion::APPROACH_SPEED);
    machineSettings.returnSpeed                   = prefs.getFloat("returnSpd",    Motion::RETURN_SPEED);
    machineSettings.homingSpeed                   = prefs.getFloat("homingSpd",    Motion::HOMING_SPEED);
    machineSettings.cycleCooldownMs               = prefs.getInt("cooldownMs",     Timing::CYCLE_COOLDOWN_MS);
    prefs.end();

    applySettings();
}

void applySettings() {
    Motion::APPROACH_POSITION                = machineSettings.approachPosition;
    Motion::CUTTING_POSITION                 = machineSettings.cuttingPosition;
    Motion::FINAL_POSITION                   = machineSettings.finalPosition;
    Motion::ALIGNMENT_SHORT_FORWARD_POSITION = machineSettings.alignmentShortForwardPosition;
    Motion::CUTTING_SPEED                    = machineSettings.cuttingSpeed;
    Motion::APPROACH_SPEED                   = machineSettings.approachSpeed;
    Motion::RETURN_SPEED                     = machineSettings.returnSpeed;
    Motion::HOMING_SPEED                     = machineSettings.homingSpeed;
    Timing::CYCLE_COOLDOWN_MS                = machineSettings.cycleCooldownMs;
}
