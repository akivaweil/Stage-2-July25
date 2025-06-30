//* ************************************************************************
//* ************************ IDLE STATE ********************************
//* ************************************************************************
// The machine waits in this state with motor disabled for either a start button press or
// transfer arm signal to begin a cutting cycle. Motor is re-enabled when entering cutting cycle.

#include <Stage2_Machine.h>

void handleIdleState() {
    static bool idleInitialized = false;
    
    // Initialize idle state on first entry
    if (!idleInitialized) {
        //! ************************************************************************
        //! STEP 1: ENSURE SAFE STATE - MOTOR DISABLED, CLAMPS RETRACTED
        //! ************************************************************************
        disableMotor(); // Motor disabled in idle state
        retractBothClamps();
        retractAlignmentCylinder();
        
        //! ************************************************************************
        //! STEP 2: RESET CYCLE FLAGS
        //! ************************************************************************
        cycleInProgress = false;
        idleInitialized = true;
        
        Serial.println("IDLE state initialized - motor disabled, waiting for start signal");
    }
    
    //! ************************************************************************
    //! STEP 3: CHECK FOR START CONDITIONS
    //! ************************************************************************
    // Check for start conditions - either start button OR transfer arm signal
    if (startButton.read() || transferArmSignal.read()) {
        Serial.println("Start signal detected");
        
        // Only start cycle if homing is complete
        if (homingComplete) {
            Serial.println("Starting cutting cycle - enabling motor");
            enableMotor(); // Re-enable motor for cutting cycle
            cycleInProgress = true;
            idleInitialized = false; // Reset for next idle entry
            currentState = ALIGNMENT;
        } else {
            Serial.println("Homing required before cutting cycle");
            idleInitialized = false; // Reset for next idle entry
            currentState = HOMING;
        }
    }
    
    //! ************************************************************************
    //! STEP 4: CHECK IF WE NEED TO HOME AGAIN
    //! ************************************************************************
    if (!homingComplete) {
        Serial.println("Homing not complete - moving to HOMING state");
        idleInitialized = false; // Reset for next idle entry
        currentState = HOMING;
    }
} 