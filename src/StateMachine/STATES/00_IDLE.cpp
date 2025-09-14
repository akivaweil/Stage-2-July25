//* ************************************************************************
//* ************************ IDLE STATE ********************************
//* ************************************************************************
// The machine waits in this state with motor disabled for either a start button press or
// transfer arm signal to begin a cutting cycle. Motor is re-enabled when entering cutting cycle.
// Motor automatically disables after 5 seconds of no activity to save power.

#include <Stage2_Machine.h>

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
    if (startButton.rose()) {
        lastActivityTime = millis(); // Reset activity timer
        
        // Ensure motor is enabled if we have activity
        if (!motorCurrentlyEnabled) {
            enableMotor();
            motorCurrentlyEnabled = true;
        }
        
        // Only start cycle if homing is complete
        if (homingComplete) {
            cycleInProgress = true;
            idleInitialized = false; // Reset for next idle entry
            currentState = ALIGNMENT;
        } else {
            idleInitialized = false; // Reset for next idle entry
            currentState = HOMING;
        }
    }
    
    // Check for transfer arm signal (rising edge)
    if (transferArmSignal.rose()) {
        lastActivityTime = millis(); // Reset activity timer
        
        // Ensure motor is enabled if we have activity
        if (!motorCurrentlyEnabled) {
            enableMotor();
            motorCurrentlyEnabled = true;
        }
        
        // Only start cycle if homing is complete
        if (homingComplete) {
            cycleInProgress = true;
            idleInitialized = false; // Reset for next idle entry
            currentState = ALIGNMENT;
        } else {
            idleInitialized = false; // Reset for next idle entry
            currentState = HOMING;
        }
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
        idleInitialized = false; // Reset for next idle entry
        currentState = HOMING;
    }
} 