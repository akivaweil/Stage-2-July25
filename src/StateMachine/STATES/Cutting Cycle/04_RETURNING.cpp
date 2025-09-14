#include <Stage2_Machine.h>

//* ************************************************************************
//* ************************ RETURNING STATE *****************************
//* ************************************************************************
// This state moves the motor back toward the home switch with clamps extended for safe homing.
// After return is complete, transitions to HOMING state for end-of-cycle homing sequence.
// Clamps are retracted in the IDLE state after homing is complete.

void handleReturningState() {
    static bool returnStarted = false;
    static unsigned long returnStartTime = 0;
    
    if (!returnStarted) {
        //! ************************************************************************
        //! PHASE 6: RETURN MOVEMENT - MOVE BACK TOWARD HOME SWITCH FOR SAFE HOMING
        //! ************************************************************************
        // Transfer arm signal already set HIGH in cutting state (prevents Z-axis interference)
        
        //! ************************************************************************
        //! STEP 1: RETRACT ALIGNMENT CYLINDER (ENSURE CLEAR PATH)
        //! ************************************************************************
        retractAlignmentCylinder();
        
        //! ************************************************************************
        //! STEP 2: MOVE 24.0 INCHES TOWARD HOME SWITCH AT FULL SPEED
        //! ************************************************************************
        // Move 24.0 inches toward home switch at full speed (negative = toward home)
        float returnDistanceSteps = -24.0 * Motion::STEPS_PER_INCH; // Negative = move toward home
        moveMotor(returnDistanceSteps, Motion::RETURN_SPEED, Motion::RETURN_ACCEL);
        
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