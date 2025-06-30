//* ************************************************************************
//* ************************ HOMING ***********************************
//* ************************************************************************
// Moves the cutting motor in the negative direction until it hits the home switch 
// using Bounce2 library for reliable debouncing, then moves to home offset position

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
        movingToHome = true;
        
        // Start moving towards home switch (negative direction)
        // Use a large negative move to ensure we reach the home switch
        stepper->setSpeedInHz(Motion::HOMING_SPEED);
        stepper->setAcceleration(Motion::FORWARD_ACCEL);
        stepper->move(-100000); // Move far in negative direction
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
            stepper->move(5); // Small positive move to clear the switch
            waitForMotorComplete(); // Wait for small move to complete
            
            // Now move to the full offset distance (positive direction away from home)
            float offsetSteps = Motion::HOME_OFFSET * Motion::STEPS_PER_INCH;
            setCurrentMotorPosition(0); // Reset position after backup move
            stepper->moveTo((long)offsetSteps); // Move to positive offset position
            
            movingToHome = false;
            movingToOffset = true;
            currentPosition = offsetSteps;
        }
    }
    
    // Check if offset movement is complete
    if (movingToOffset) {
        if (!isMotorRunning()) {
            // Motor has finished moving to offset position
            stopMotor();
            homingComplete = true;
            homingStarted = false;
            movingToOffset = false;
            
            // Update position tracking
            currentPosition = getCurrentMotorPosition();
            
            currentState = IDLE;
        }
    }
} 