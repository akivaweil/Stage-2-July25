// Homing
// Homing sequence: find home switch, set position to 0, move to offset

#include "StateMachine/StateMachine.h"

// Homing configuration — named constants to avoid magic numbers
namespace HomingConfig {
    const long HOME_SEARCH_DISTANCE_STEPS = -100000;  // Long move toward switch while searching for home
    const long HOME_BACKOFF_DISTANCE_STEPS = 1;       // Tiny move off the switch after it triggers
    const long OFFSET_POSITION_TOLERANCE_STEPS = 5;   // Allowable error when reaching the offset position
    const int  MAX_HOMING_RETRIES = 3;                // Retries before aborting/restarting homing
    const unsigned long OFFSET_MOVE_TIMEOUT_MS = 10000;  // Max time for the offset move before retrying
}  // namespace HomingConfig

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
            stepper->move(HomingConfig::HOME_SEARCH_DISTANCE_STEPS);
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
            stepper->move(HomingConfig::HOME_BACKOFF_DISTANCE_STEPS);
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
            
            // Allow a small tolerance when checking the offset position
            if (positionError <= HomingConfig::OFFSET_POSITION_TOLERANCE_STEPS) {
                // Successfully reached target
                setCurrentMotorPosition(0);
                currentPosition = 0.0;
                homingComplete = true;
                homingStarted = false;
                movingToHome = false;
                movingToOffset = false;
                retryCount = 0;
                currentState = STATE_IDLE;
            } else {
                // Did not reach target - retry
                retryCount++;

                if (retryCount < HomingConfig::MAX_HOMING_RETRIES) {
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
        
        // Timeout check - if motor hasn't completed in time, retry
        if (isMotorRunning() && (millis() - offsetStartTime > HomingConfig::OFFSET_MOVE_TIMEOUT_MS)) {
            retryCount++;

            if (retryCount < HomingConfig::MAX_HOMING_RETRIES) {
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