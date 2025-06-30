//* ************************************************************************
//* ************************ CYCLE COMPLETE ****************************
//* ************************************************************************
// Signals that the cutting cycle is complete and returns to IDLE state

#include <Stage2_Machine.h>

void state_CYCLE_COMPLETE() {
    static bool signalSent = false;
    
    // First entry - signal transfer arm that cycle is complete
    if (!signalSent) {
        Serial.println("Cutting cycle complete - signaling transfer arm");
        signalTransferArm();
        signalSent = true;
    }
    
    // Wait a moment for signal to be processed
    if (millis() - stateStartTime >= 500) {
        Serial.println("Cycle complete - returning to IDLE");
        
        // Reset flags
        cycleInProgress = false;
        signalSent = false; // Reset for next cycle
        
        // Return to IDLE state to wait for next cycle
        changeState(IDLE);
    }
} 