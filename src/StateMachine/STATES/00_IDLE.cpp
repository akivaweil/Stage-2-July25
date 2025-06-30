//* ************************************************************************
//* ************************ IDLE STATE ********************************
//* ************************************************************************
// The machine waits in this state for either a start button press or
// transfer arm signal to begin a cutting cycle

#include <Stage2_Machine.h>

void state_IDLE() {
    // Ensure motor is disabled and clamps are released
    disableMotor();
    releaseClamps();
    releaseAlignCylinder();
    
    // Reset cycle flags
    cycleInProgress = false;
    
    // Check for start conditions - either start button or transfer arm signal
    if (startButton.read() || transferArmSignal.read()) {
        // Only start cycle if homing is complete
        if (homingComplete) {
            Serial.println("Cutting cycle requested - starting cycle");
            changeState(CUTTING_CYCLE);
        } else {
            Serial.println("Homing not complete - cannot start cycle");
            changeState(HOMING);
        }
    }
    
    // Check if we need to home again
    if (!homingComplete) {
        changeState(HOMING);
    }
} 