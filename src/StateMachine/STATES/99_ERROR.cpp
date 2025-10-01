#include <Stage2_Machine.h>

//* ************************************************************************
//* ************************ ERROR STATE **********************************
//* ************************************************************************
// This state handles router jammed errors - machine freezes and disables motor

void handleErrorState() {
    //! ************************************************************************
    //! ERROR STATE: ROUTER JAMMED DETECTED
    //! ************************************************************************
    // Machine is frozen in error state - motor disabled, all movement stopped
    
    // Ensure motor is disabled
    disableMotor();
    
    // Stop any running motor movement
    stopMotor();
    
    //! ************************************************************************
    //! CHECK FOR START BUTTON PRESS - EXIT ERROR STATE
    //! ************************************************************************
    // Check if start button is pressed to exit error state and return to homing
    if (checkStartButtonForHoming()) {
        // Reset cycle flags and transition to homing
        cycleInProgress = false;
        homingComplete = false; // Force homing sequence
        
        // Re-enable motor for homing
        enableMotor();
        
        // Keep both clamps extended for safe material handling
        // Only retract alignment cylinder for safe homing
        retractAlignmentCylinder();
        
        // Transition to homing state
        currentState = HOMING;
        return; // Exit function, state will be changed to HOMING
    }
    
    // Keep clamps in current state (don't change them)
    // Machine remains frozen until start button is pressed
}
