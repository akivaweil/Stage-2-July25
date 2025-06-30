//* ************************************************************************
//* ************************ IDLE STATE ********************************
//* ************************************************************************
// The machine waits in this state for either a start button press or
// transfer arm signal to begin a cutting cycle

#include <Stage2_Machine.h>

void handleIdleState() {
    //! ************************************************************************
    //! STEP 1: ENSURE SAFE STATE - MOTOR DISABLED, CLAMPS RETRACTED
    //! ************************************************************************
    disableMotor();
    retractBothClamps();
    retractAlignmentCylinder();
    
    //! ************************************************************************
    //! STEP 2: RESET CYCLE FLAGS
    //! ************************************************************************
    cycleInProgress = false;
    
    //! ************************************************************************
    //! STEP 3: CHECK FOR START CONDITIONS
    //! ************************************************************************
    // Check for start conditions - either start button or transfer arm signal
    if (startButton.read() || transferArmSignal.read()) {
        // Only start cycle if homing is complete
        if (homingComplete) {
            currentState = ALIGNMENT;
        } else {
            currentState = HOMING;
        }
    }
    
    //! ************************************************************************
    //! STEP 4: CHECK IF WE NEED TO HOME AGAIN
    //! ************************************************************************
    if (!homingComplete) {
        currentState = HOMING;
    }
} 