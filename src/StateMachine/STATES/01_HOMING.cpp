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
        currentPosition = 0.0;
        homingStarted = true;
        movingToHome = true;
        
        // Start moving towards home switch (negative direction)
        setDirection(false); // Move in negative direction towards home
    }
    
    // Check home switch with 10ms debounce
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
                    Serial.println("Home switch triggered - moving to offset position");
                    
                    // Move away from home switch by offset distance
                    float offsetSteps = inchesToSteps(Motion::HOME_OFFSET);
                    setDirection(true); // Move away from home (positive direction)
                    moveMotor(offsetSteps, Motion::HOMING_SPEED, Motion::FORWARD_ACCEL);
                    
                    movingToHome = false;
                    movingToOffset = true;
                    currentPosition = Motion::HOME_OFFSET * Motion::STEPS_PER_INCH;
                } else {
                    // False trigger - reset debounce
                    homeDebounceActive = false;
                }
            }
        } else {
            // Continue moving towards home - simple step generation
            static unsigned long lastStep = 0;
            unsigned long stepInterval = 1000000 / Motion::HOMING_SPEED; // Microseconds
            
            if (micros() - lastStep >= stepInterval) {
                digitalWrite(Pins::STEP, HIGH);
                delayMicroseconds(2);
                digitalWrite(Pins::STEP, LOW);
                lastStep = micros();
            }
        }
    }
    
    // Check if offset movement is complete
    if (movingToOffset) {
        // Simple check - in real implementation this would be handled by motion controller
        delay(Timing::HOME_SETTLE_TIME);
        
        stopMotor();
        homingComplete = true;
        homingStarted = false;
        movingToOffset = false;
        
        Serial.println("Homing complete - moving to IDLE state");
        Serial.print("Current position: ");
        Serial.print(stepsToInches(currentPosition));
        Serial.println(" inches from home");
        
        currentState = IDLE;
    }
} 