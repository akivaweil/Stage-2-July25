//* ************************************************************************
//* ************************ HOMING ***********************************
//* ************************************************************************
// Moves the cutting motor to the home position using the home switch

#include <Stage2_Machine.h>

void handleHomingState() {
    static bool homingStarted = false;
    static bool movingToHome = false;
    
    // First entry into homing state
    if (!homingStarted) {
        Serial.println("Starting homing sequence");
        enableMotor();
        homingComplete = false;
        currentPosition = 0.0;
        homingStarted = true;
        movingToHome = true;
        
        // Start moving towards home switch
        setDirection(false); // Move towards home (reverse direction)
    }
    
    // Check if home switch is active
    if (homeSwitch.read()) {
        if (movingToHome) {
            // Stop at home switch
            stopMotor();
            delay(Timing::HOME_SETTLE_TIME);
            
            // Move away from home switch by offset distance
            float offsetSteps = inchesToSteps(Motion::HOME_OFFSET);
            setDirection(true); // Move away from home
            moveMotor(offsetSteps, Motion::HOMING_SPEED, Motion::FORWARD_ACCEL);
            
            movingToHome = false;
            currentPosition = Motion::HOME_OFFSET * Motion::STEPS_PER_INCH;
        }
    }
    
    // Check if homing is complete
    if (!movingToHome && !homeSwitch.read()) {
        stopMotor();
        homingComplete = true;
        homingStarted = false;
        
        Serial.println("Homing complete");
        Serial.print("Current position: ");
        Serial.print(stepsToInches(currentPosition));
        Serial.println(" inches");
        
        changeState(IDLE);
    }
    
    // Continue moving if still homing
    if (movingToHome) {
        // Simple step generation for homing
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