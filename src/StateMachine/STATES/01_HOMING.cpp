//* ************************************************************************
//* ************************ HOMING ***********************************
//* ************************************************************************
// Homing sequence: find home switch, set position to 0, move to offset

#include <Stage2_Machine.h>

void handleHomingState() {
    static bool homingStarted = false;
    static bool movingToHome = false;
    static bool movingToOffset = false;
    
    if (!homingStarted) {
        enableMotor();
        homingComplete = false;
        setCurrentMotorPosition(0);
        currentPosition = 0.0;
        homingStarted = true;
        updateInputs();
        
        if (homeSwitch.read()) {
            // Already at home switch
            setCurrentMotorPosition(0);
            currentPosition = 0.0;
            float offsetSteps = Motion::HOME_OFFSET_POSITION * Motion::STEPS_PER_INCH;
            stepper->setSpeedInHz(Motion::HOMING_SPEED);
            stepper->setAcceleration(Motion::FORWARD_ACCEL);
            stepper->moveTo((long)offsetSteps);
            movingToHome = false;
            movingToOffset = true;
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
            float offsetSteps = Motion::HOME_OFFSET_POSITION * Motion::STEPS_PER_INCH;
            setCurrentMotorPosition(0);
            stepper->moveTo((long)offsetSteps);
            
            movingToHome = false;
            movingToOffset = true;
        }
    }
    
    if (movingToOffset) {
        if (!isMotorRunning()) {
            stopMotor();
            setCurrentMotorPosition(0);
            currentPosition = 0.0;
            homingComplete = true;
            homingStarted = false;
            movingToOffset = false;
            
            // Retract clamps before returning to IDLE
            retractBothClamps();
            retractAlignmentCylinder();
            
            currentState = IDLE;
        }
    }
} 