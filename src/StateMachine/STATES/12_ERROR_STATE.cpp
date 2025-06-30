//* ************************************************************************
//* ************************ ERROR STATE *******************************
//* ************************************************************************
// Handles system errors and stops all operations

#include <Stage2_Machine.h>

void state_ERROR_STATE() {
    static bool errorHandled = false;
    
    // First entry into error state
    if (!errorHandled) {
        Serial.println("ERROR STATE ENTERED - Stopping all operations");
        
        // Stop motor and disable
        stopMotor();
        disableMotor();
        
        // Release all pneumatics
        releaseClamps();
        releaseAlignCylinder();
        
        // Set error flag
        emergencyStop = true;
        cycleInProgress = false;
        homingComplete = false;
        
        errorHandled = true;
    }
    
    // Stay in error state - requires manual reset or power cycle
    Serial.println("System in ERROR state - Manual intervention required");
    delay(1000);
} 