//* ************************************************************************
//* ************************ HOMING ***********************************
//* ************************************************************************
// Enhanced homing sequence that handles both initial homing and end-of-cycle homing:
// 1. If not at home switch: moves in negative direction until it hits the home switch
// 2. If already at home switch: skips search and moves directly to offset
// 3. Continues moving away from home switch until it's no longer active
// Uses Bounce2 library for reliable debouncing and ensures accurate positioning

#include <Stage2_Machine.h>

void handleHomingState() {
    static bool homingStarted = false;
    static bool movingToHome = false;
    static bool movingToOffset = false;
    
    // First entry into homing state
    if (!homingStarted) {
        enableMotor();
        homingComplete = false;
        
        // Set current position to 0 for reference
        setCurrentMotorPosition(0);
        currentPosition = 0.0;
        
        homingStarted = true;
        
        // Update inputs to check current home switch state
        updateInputs();
        
        // Check if we're already at the home switch
        if (homeSwitch.read()) {
            // Already at home switch - skip search phase and go directly to offset movement
            movingToHome = false;
            movingToOffset = true;
            
            // Move to the offset distance (positive direction away from home)
            float offsetSteps = Motion::HOME_OFFSET * Motion::STEPS_PER_INCH;
            stepper->setSpeedInHz(Motion::HOMING_SPEED);
            stepper->setAcceleration(Motion::FORWARD_ACCEL);
            stepper->moveTo((long)offsetSteps);
        } else {
            // Not at home switch - start search phase
            movingToHome = true;
            
            // Start moving towards home switch (negative direction)
            // Use a large negative move to ensure we reach the home switch
            stepper->setSpeedInHz(Motion::HOMING_SPEED);
            stepper->setAcceleration(Motion::FORWARD_ACCEL);
            stepper->move(-100000); // Move far in negative direction
        }
    }
    
    // Check for home switch activation while moving to home
    if (movingToHome) {
        // Update inputs immediately before checking switch state for real-time response
        updateInputs();
        
        // Check if home switch is currently active (use .read() for continuous monitoring)
        if (homeSwitch.read()) {
            // Home switch just activated - stop immediately
            stepper->forceStopAndNewPosition(stepper->getCurrentPosition());
            waitForMotorComplete(); // Wait for motor to fully stop
            
            // Set current position as home (0)
            setCurrentMotorPosition(0);
            
            // First, make a small move away from the switch to ensure we're clear
            stepper->setSpeedInHz(Motion::HOMING_SPEED);
            stepper->setAcceleration(Motion::FORWARD_ACCEL);
            stepper->move(1); // Small positive move to clear the switch
            waitForMotorComplete(); // Wait for small move to complete
            
            // Now move to the full offset distance (positive direction away from home)
            float offsetSteps = Motion::HOME_OFFSET * Motion::STEPS_PER_INCH;
            setCurrentMotorPosition(0); // Reset position after backup move
            stepper->moveTo((long)offsetSteps); // Move to positive offset position
            
            movingToHome = false;
            movingToOffset = true;
            currentPosition = 0.0; // Will be set to 0 when we reach offset position
        }
    }
    
    // Check if offset movement is complete
    if (movingToOffset) {
        if (!isMotorRunning()) {
            // Update inputs to check current home switch state
            updateInputs();
            
            // Check if home switch is still active after offset move
            if (homeSwitch.read()) {
                // Home switch is still HIGH - continue moving away from home
                float additionalOffset = Motion::HOME_OFFSET * Motion::STEPS_PER_INCH;
                stepper->setSpeedInHz(Motion::HOMING_SPEED);
                stepper->setAcceleration(Motion::FORWARD_ACCEL);
                stepper->move((long)additionalOffset); // Move additional offset distance
                
                // Stay in movingToOffset state to check again after this move
            } else {
                // Home switch is now clear - homing sequence complete
                stopMotor();
                setCurrentMotorPosition(0); // Set current position as zero reference
                currentPosition = 0.0; // Reset position tracking to 0
                
                homingComplete = true;
                homingStarted = false;
                movingToOffset = false;
                
                currentState = IDLE;
            }
        }
    }
} 