//* ************************************************************************
//* ************************ HOMING ***********************************
//* ************************************************************************
// Homing sequence: find home switch, set position to 0, move to offset

#include <Stage2_Machine.h>

void handleHomingState() {
    static bool homingStarted = false;
    static bool movingToHome = false;
    static bool movingToOffset = false;
    static int retryCount = 0;
    static float targetOffsetSteps = 0;
    static unsigned long offsetStartTime = 0;
    
    if (!homingStarted) {
        enableMotor();
        homingComplete = false;
        setCurrentMotorPosition(0);
        currentPosition = 0.0;
        homingStarted = true;
        retryCount = 0;
        movingToHome = false;
        movingToOffset = false;
        updateInputs();
        
        if (homeSwitch.read()) {
            // Already at home switch
            setCurrentMotorPosition(0);
            currentPosition = 0.0;
            targetOffsetSteps = Motion::HOME_OFFSET_POSITION * Motion::STEPS_PER_INCH;
            stepper->setSpeedInHz(Motion::HOMING_SPEED);
            stepper->setAcceleration(Motion::FORWARD_ACCEL);
            stepper->moveTo((long)targetOffsetSteps);
            movingToHome = false;
            movingToOffset = true;
            offsetStartTime = millis();
        } else {
            // Search for home switch
            movingToHome = true;
            stepper->setSpeedInHz(Motion::HOMING_SPEED);
            stepper->setAcceleration(Motion::FORWARD_ACCEL);
            stepper->move(-100000);
        }
    }
    
    if (movingToHome) {
        updateInputs();
        
        if (homeSwitch.read()) {
            // Home switch found
            stepper->forceStopAndNewPosition(stepper->getCurrentPosition());
            waitForMotorComplete();
            setCurrentMotorPosition(0);
            currentPosition = 0.0;
            
            // Move away from switch
            stepper->setSpeedInHz(Motion::HOMING_SPEED);
            stepper->setAcceleration(Motion::FORWARD_ACCEL);
            stepper->move(1);
            waitForMotorComplete();
            
            // Move to offset position
            targetOffsetSteps = Motion::HOME_OFFSET_POSITION * Motion::STEPS_PER_INCH;
            setCurrentMotorPosition(0);
            stepper->moveTo((long)targetOffsetSteps);
            
            movingToHome = false;
            movingToOffset = true;
            offsetStartTime = millis();
        }
    }
    
    if (movingToOffset) {
        if (!isMotorRunning()) {
            // Check if motor reached target position
            long currentPos = getCurrentMotorPosition();
            long targetPos = (long)targetOffsetSteps;
            long positionError = abs(currentPos - targetPos);
            
            // Allow 320 steps of tolerance
            if (positionError <= 320) {
                // Successfully reached target
                setCurrentMotorPosition(0);
                currentPosition = 0.0;
                homingComplete = true;
                homingStarted = false;
                movingToHome = false;
                movingToOffset = false;
                retryCount = 0;
                currentState = IDLE;
            } else {
                // Did not reach target - retry
                retryCount++;
                
                if (retryCount < 3) {
                    // Re-enable motor and retry
                    enableMotor();
                    stepper->setSpeedInHz(Motion::HOMING_SPEED);
                    stepper->setAcceleration(Motion::FORWARD_ACCEL);
                    stepper->moveTo((long)targetOffsetSteps);
                    offsetStartTime = millis();
                } else {
                    // Too many retries - abort and restart homing
                    stopMotor();
                    homingStarted = false;
                    movingToHome = false;
                    movingToOffset = false;
                    retryCount = 0;
                    // Will restart on next cycle
                }
            }
        }
        
        // Timeout check - if motor hasn't completed in 10 seconds, retry
        if (isMotorRunning() && (millis() - offsetStartTime > 10000)) {
            retryCount++;
            
            if (retryCount < 3) {
                // Stop and retry
                stepper->forceStopAndNewPosition(stepper->getCurrentPosition());
                waitForMotorComplete();
                enableMotor();
                stepper->setSpeedInHz(Motion::HOMING_SPEED);
                stepper->setAcceleration(Motion::FORWARD_ACCEL);
                stepper->moveTo((long)targetOffsetSteps);
                offsetStartTime = millis();
            } else {
                // Too many retries - abort and restart homing
                stopMotor();
                homingStarted = false;
                movingToHome = false;
                movingToOffset = false;
                retryCount = 0;
            }
        }
    }
} 