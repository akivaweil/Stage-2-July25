//* ************************************************************************
//* ************************ IDLE STATE ********************************
//* ************************************************************************
// The machine waits in this state with motor disabled for either a start button press or
// transfer arm signal to begin a cutting cycle. Motor is re-enabled when entering cutting cycle.
// Motor automatically disables after 5 seconds of no activity to save power.

#include <Stage2_Machine.h>

namespace IdleConfig {
    const int ROUTER_TEST_PULSE_MS = 200;  // Duration of router signal pin pulse for test
}

void handleIdleState() {
    static bool idleInitialized = false;
    static unsigned long lastActivityTime = 0;
    static bool motorCurrentlyEnabled = false;
    
    // Ensure inputs are updated for reliable Bounce2 operation
    updateInputs();
    
    // Initialize idle state on first entry
    if (!idleInitialized) {
        //! ************************************************************************
        //! STEP 1: ENSURE SAFE STATE - MOTOR ENABLED, CLAMPS RETRACTED
        //! ************************************************************************
        enableMotor(); // Motor initially enabled in idle state
        motorCurrentlyEnabled = true;
        retractBothClamps();
        retractAlignmentCylinder();
        
        //! ************************************************************************
        //! STEP 2: RESET CYCLE FLAGS AND ACTIVITY TIMER
        //! ************************************************************************
        cycleInProgress = false;
        lastActivityTime = millis(); // Reset activity timer
        idleInitialized = true;
    }
    
    //! ************************************************************************
    //! STEP 3: CHECK FOR ACTIVITY AND UPDATE TIMER
    //! ************************************************************************
    // Check for start button rising edge (button press)
    bool startButtonCurrentlyPressed = startButton.read();
    if (startButtonCurrentlyPressed && !startButtonWasPressed) {
        // Rising edge detected - button was just pressed
        startButtonWasPressed = true;
        lastActivityTime = millis(); // Reset activity timer
        
        // Ensure motor is enabled if we have activity
        if (!motorCurrentlyEnabled) {
            enableMotor();
            motorCurrentlyEnabled = true;
        }
        
        //! TESTING ONLY: Pulse CLAMP_RELEASE_SIGNAL for 200ms instead of full sequence
        // Only pulse if router clear sensor is HIGH (clear)
        routerClearSensor.update();
        if (!routerClearSensor.read()) {
            digitalWrite(Pins::CLAMP_RELEASE_SIGNAL, HIGH);
            delay(IdleConfig::ROUTER_TEST_PULSE_MS);
            digitalWrite(Pins::CLAMP_RELEASE_SIGNAL, LOW);
        }
    } else if (!startButtonCurrentlyPressed) {
        // Button is not pressed, reset the tracking variable
        startButtonWasPressed = false;
    }
    
    // Check for transfer arm signal (rising edge)
    bool transferArmCurrentlyActive = transferArmSignal.read();
    if (transferArmCurrentlyActive && !transferArmSignalWasActive) {
        // Rising edge detected - signal was just activated
        transferArmSignalWasActive = true;
        lastActivityTime = millis(); // Reset activity timer
        
        // Ensure motor is enabled if we have activity
        if (!motorCurrentlyEnabled) {
            enableMotor();
            motorCurrentlyEnabled = true;
        }
    } else if (!transferArmCurrentlyActive) {
        // Signal is not active, reset the tracking variable
        transferArmSignalWasActive = false;
    }
    
    //! ************************************************************************
    //! STEP 4: MOTOR TIMEOUT MANAGEMENT
    //! ************************************************************************
    // Check if 5 seconds have passed without activity
    if (motorCurrentlyEnabled && (millis() - lastActivityTime >= Timing::MOTOR_TIMEOUT)) {
        //disableMotor(); // Disable motor after timeout
        motorCurrentlyEnabled = false;
    }
    
    //! ************************************************************************
    //! STEP 5: CHECK IF WE NEED TO HOME AGAIN
    //! ************************************************************************
    if (!homingComplete) {
        enableMotor(); // Ensure motor is enabled before homing
        idleInitialized = false; // Reset for next idle entry
        currentState = HOMING;
    }
} 