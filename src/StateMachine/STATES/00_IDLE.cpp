// Idle state
// The machine waits in this state with motor disabled for either a start button press or
// transfer arm signal to begin a cutting cycle. Motor is re-enabled when entering cutting cycle.
// Motor automatically disables after 5 seconds of no activity to save power.

#include "StateMachine/StateMachine.h"
#include "ConfigApi/MachineConfigApi.h"

void handleIdleState() {
    static bool idleInitialized = false;
    static unsigned long lastActivityTime = 0;
    static bool motorCurrentlyEnabled = false;
    
    // Ensure inputs are updated for reliable Bounce2 operation
    updateInputs();
    
    // Initialize idle state on first entry
    if (!idleInitialized) {
        // Step 1: ensure safe state - motor enabled, clamps retracted
        enableMotor(); // Motor initially enabled in idle state
        motorCurrentlyEnabled = true;
        retractBothClamps();
        retractAlignmentCylinder();

        // Step 2: reset cycle flags and activity timer
        cycleInProgress = false;
        lastActivityTime = millis(); // Reset activity timer
        idleInitialized = true;

        // Step 2b: apply any dashboard config changes deferred during a cycle
        // A POST /api/config that arrived mid-cycle set configDirty; now that we
        // are safely idle, push the persisted settings into the live globals.
        applyConfigIfDirty();
    }

    // Step 3: check for activity and update timer
    // Enforce inter-cycle cooldown: ignore start triggers for CYCLE_COOLDOWN_MS
    // after a cycle finishes. cycleEndTime == 0 means no cycle has run yet.
    bool cooldownActive = (cycleEndTime != 0) &&
                          ((millis() - cycleEndTime) < (unsigned long)Timing::CYCLE_COOLDOWN_MS);

    // Check for start button rising edge (button press)
    bool startButtonCurrentlyPressed = startButton.read();
    if (!cooldownActive && startButtonCurrentlyPressed && !startButtonWasPressed) {
        // Rising edge detected - button was just pressed
        startButtonWasPressed = true;
        lastActivityTime = millis(); // Reset activity timer
        
        // Ensure motor is enabled if we have activity
        if (!motorCurrentlyEnabled) {
            enableMotor();
            motorCurrentlyEnabled = true;
        }
        
        // Only start cycle if homing is complete
        if (homingComplete) {
            cycleInProgress = true;
            cycleStartTime = millis(); // Record cycle start for cancel ignore window
            idleInitialized = false; // Reset for next idle entry
            currentState = STATE_ALIGNMENT;
        } else {
            enableMotor(); // Ensure motor is enabled before homing
            idleInitialized = false; // Reset for next idle entry
            currentState = STATE_HOMING;
        }
    } else if (!startButtonCurrentlyPressed) {
        // Button is not pressed, reset the tracking variable
        startButtonWasPressed = false;
    }
    
    // Check for transfer arm signal (rising edge)
    bool transferArmCurrentlyActive = transferArmSignal.read();
    if (!cooldownActive && transferArmCurrentlyActive && !transferArmSignalWasActive) {
        // Rising edge detected - signal was just activated
        transferArmSignalWasActive = true;
        lastActivityTime = millis(); // Reset activity timer
        
        // Ensure motor is enabled if we have activity
        if (!motorCurrentlyEnabled) {
            enableMotor();
            motorCurrentlyEnabled = true;
        }
        
        // Only start cycle if homing is complete
        if (homingComplete) {
            cycleInProgress = true;
            cycleStartTime = millis();
            idleInitialized = false; // Reset for next idle entry
            currentState = STATE_ALIGNMENT;
        } else {
            enableMotor(); // Ensure motor is enabled before homing
            idleInitialized = false; // Reset for next idle entry
            currentState = STATE_HOMING;
        }
    } else if (!transferArmCurrentlyActive) {
        // Signal is not active, reset the tracking variable
        transferArmSignalWasActive = false;
    }
    
    // Step 4: motor timeout management
    // Check if 5 seconds have passed without activity
    if (motorCurrentlyEnabled && (millis() - lastActivityTime >= Timing::MOTOR_TIMEOUT)) {
        //disableMotor(); // Disable motor after timeout
        motorCurrentlyEnabled = false;
    }

    // Step 5: check if we need to home again
    if (!homingComplete) {
        enableMotor(); // Ensure motor is enabled before homing
        idleInitialized = false; // Reset for next idle entry
        currentState = STATE_HOMING;
    }
} 