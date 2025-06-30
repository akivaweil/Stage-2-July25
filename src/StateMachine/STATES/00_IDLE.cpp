//* ************************************************************************
//* ************************ IDLE STATE ********************************
//* ************************************************************************
// The machine waits in this state with motor disabled for either a start button press or
// transfer arm signal to begin a cutting cycle. Motor is re-enabled when entering cutting cycle.

#include <Stage2_Machine.h>

void handleIdleState() {
    static bool idleInitialized = false;
    
    // Ensure inputs are updated for reliable Bounce2 operation
    updateInputs();
    
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
    }
    
    //! ************************************************************************
    //! STEP 3: CHECK FOR START CONDITIONS
    //! ************************************************************************
    // Check for start conditions - using .read() for continuous monitoring
    if (startButton.read() || transferArmSignal.read()) {
        // Only start cycle if homing is complete
        if (homingComplete) {
            enableMotor(); // Re-enable motor for cutting cycle
            cycleInProgress = true;
            idleInitialized = false; // Reset for next idle entry
            currentState = ALIGNMENT;
        } else {
            idleInitialized = false; // Reset for next idle entry
            currentState = HOMING;
        }
    }
    
    //! ************************************************************************
    //! STEP 4: CHECK IF WE NEED TO HOME AGAIN
    //! ************************************************************************
    if (!homingComplete) {
        idleInitialized = false; // Reset for next idle entry
        currentState = HOMING;
    }
} 