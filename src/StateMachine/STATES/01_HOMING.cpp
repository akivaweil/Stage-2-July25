//* ************************************************************************
//* ************************ HOMING ***********************************
//* ************************************************************************
// Moves the cutting motor in the negative direction until it hits the home switch 
// (10ms debounce) and moves to the home offset position before switching to idle state

#include <Stage2_Machine.h>

void handleHomingState() {
    static bool homingStarted = false;
    static bool movingToHome = false;
    static bool movingToOffset = false;
    static unsigned long homeHitTime = 0;
    static bool homeDebounceActive = false;
    
    // First entry into homing state
    if (!homingStarted) {
        Serial.println("Starting homing sequence - moving in negative direction");
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
        
        Serial.println("Moving toward home switch...");
    }
    
    // Check home switch with 10ms debounce while moving to home
    if (movingToHome) {
        if (homeSwitch.read() && !homeDebounceActive) {
            // Home switch just activated - start debounce timer
            homeHitTime = millis();
            homeDebounceActive = true;
        }
        
        if (homeDebounceActive) {
            // Check if 10ms have passed and switch is still active
            if (millis() - homeHitTime >= 10) {
                if (homeSwitch.read()) {
                    // Home switch confirmed active after debounce
                    stopMotor();
                    waitForMotorComplete(); // Wait for motor to fully stop
                    
                    Serial.println("Home switch triggered - moving to offset position");
                    
                    // Set current position as home (0)
                    setCurrentMotorPosition(0);
                    
                    // Move away from home switch by offset distance
                    float offsetSteps = inchesToSteps(Motion::HOME_OFFSET);
                    stepper->setSpeedInHz(Motion::HOMING_SPEED);
                    stepper->setAcceleration(Motion::FORWARD_ACCEL);
                    stepper->moveTo((long)offsetSteps); // Move to positive offset position
                    
                    movingToHome = false;
                    movingToOffset = true;
                    currentPosition = offsetSteps;
                } else {
                    // False trigger - reset debounce
                    homeDebounceActive = false;
                }
            }
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
            
            Serial.println("Homing complete - moving to IDLE state");
            Serial.print("Current position: ");
            Serial.print(stepsToInches(currentPosition));
            Serial.println(" inches from home");
            
            currentState = IDLE;
        }
    }
} 