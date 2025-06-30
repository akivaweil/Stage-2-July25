#include <Stage2_Machine.h>

//* ************************************************************************
//* ************************ RETURNING STATE *****************************
//* ************************************************************************
// This state returns the motor to home offset (clamps already released in cutting state)

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
        float homeOffsetSteps = Motion::HOME_OFFSET * Motion::STEPS_PER_INCH;
        moveMotorToPosition(homeOffsetSteps, Motion::RETURN_SPEED, Motion::RETURN_ACCEL);
        
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
        //! STEP 4: RETURN TO IDLE STATE
        //! ************************************************************************
        currentState = IDLE;
    }
} 