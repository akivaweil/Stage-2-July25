#include <Stage2_Machine.h>

//* ************************************************************************
//* ************************ RETURNING STATE *****************************
//* ************************************************************************
// This state returns the motor to home offset with clamps extended.
// After return is complete, transitions to HOMING state for end-of-cycle homing sequence.
// Clamps are retracted in the IDLE state after homing is complete.

void handleReturningState() {
    static bool returnStarted = false;
    static unsigned long returnStartTime = 0;
    
    if (!returnStarted) {
        //! ************************************************************************
        //! PHASE 6: RETURN MOVEMENT - RETURN TO HOME OFFSET
        //! ************************************************************************
        // Transfer arm signal already set HIGH in cutting state (prevents Z-axis interference)
        
        //! ************************************************************************
        //! STEP 1: RETRACT ALIGNMENT CYLINDER (ENSURE CLEAR PATH)
        //! ************************************************************************
        retractAlignmentCylinder();
        
        //! ************************************************************************
        //! STEP 2: RETURN TO HOME OFFSET POSITION AT HIGH SPEED
        //! ************************************************************************
        float homePosition = Motion::HOME_OFFSET_POSITION * Motion::STEPS_PER_INCH; // Return to home offset position
        moveMotorToPosition(homePosition, Motion::RETURN_SPEED, Motion::RETURN_ACCEL);
        
        returnStarted = true;
        returnStartTime = millis();
    }
    
    // Wait for return movement to complete using FastAccelStepper status
    if (!isMotorRunning()) {
        //! ************************************************************************
        //! STEP 3: COMPLETE TRANSFER ARM SIGNAL AND RETURN TO IDLE
        //! ************************************************************************
        // Complete the transfer arm signal pulse
        digitalWrite(Pins::TRANSFER_ARM_SIGNAL, LOW);
        
        // Update current position from stepper
        currentPosition = getCurrentMotorPosition();
        
        // Reset static variables for next cycle
        returnStarted = false;
        returnStartTime = 0;
        
        //! ************************************************************************
        //! STEP 4: TRANSITION TO HOMING FOR END-OF-CYCLE HOMING SEQUENCE
        //! ************************************************************************
        // After each cut cycle, perform homing sequence to ensure accuracy
        homingComplete = false; // Reset homing flag to force homing sequence
        currentState = HOMING;
    }
} 