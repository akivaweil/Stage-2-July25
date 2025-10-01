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
    
    // Keep clamps in current state (don't change them)
    // Machine remains frozen until manual intervention
    
    // No state transitions - machine stays in ERROR state
    // Requires manual reset or power cycle to recover
}
