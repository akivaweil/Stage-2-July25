#include <Stage2_Machine.h>
#include <Config.h>

//* ************************************************************************
//* ************************ ALIGNMENT STATE ****************************
//* ************************************************************************
// This state performs the complete clamp sequence with detailed material alignment choreography

//! ************************************************************************
//! ALIGNMENT STEP CONSTANTS
//! ************************************************************************
#define STEP_INITIAL_MOVEMENT          0
#define STEP_WAIT_INITIAL              1
#define STEP_REVERSE_MOVEMENT          2
#define STEP_WAIT_REVERSE              3
#define STEP_ALIGNMENT_CYLINDER_EXTEND 4
#define STEP_WAIT_ALIGNMENT_PRE        5
#define STEP_LEFT_CLAMP_EXTEND         6
#define STEP_WAIT_LEFT_CLAMP           7
#define STEP_LEFT_CLAMP_RETRACT        8
#define STEP_RIGHT_CLAMP_EXTEND        9
#define STEP_WAIT_RIGHT_EXTEND         10
#define STEP_RIGHT_CLAMP_RETRACT       11
#define STEP_ALIGNMENT_RIGHT_EXTEND    12
#define STEP_WAIT_ALIGNMENT_RIGHT      13
#define STEP_FINAL_MOVEMENT            14
#define STEP_WAIT_FINAL_MOVEMENT       15
#define STEP_FINAL_LEFT_CLAMP          16
#define STEP_FINAL_RIGHT_CLAMP         17
#define STEP_FINAL_SETTLE              18

//! ************************************************************************
//! STATIC VARIABLES FOR ALIGNMENT STATE
//! ************************************************************************
static unsigned long stepStartTime = 0;
static int currentStep = 0;

//! ************************************************************************
//! FORWARD DECLARATIONS
//! ************************************************************************
void resetAlignmentVariables();
void handleInitialMovementPhase();
void handleAlignmentSequencePhase();
void handleFinalClampPhase();

//! ************************************************************************
//! MAIN ALIGNMENT STATE HANDLER
//! ************************************************************************
void handleAlignmentState() {
    
    //! ************************************************************************
    //! CHECK FOR START BUTTON PRESS - INTERRUPT TO HOMING
    //! ************************************************************************
    if (checkStartButtonForHoming()) {
        resetAlignmentVariables();
        return; // Exit function, state will be changed to HOMING
    }
    
    // Initialize step timing
    if (stepStartTime == 0) {
        enableMotor();
        stepStartTime = millis();
    }
    
    //! ************************************************************************
    //! STEP ROUTING
    //! ************************************************************************
    if (currentStep >= STEP_INITIAL_MOVEMENT && currentStep <= STEP_WAIT_REVERSE) {
        handleInitialMovementPhase();
    } else if (currentStep >= STEP_ALIGNMENT_CYLINDER_EXTEND && currentStep <= STEP_WAIT_FINAL_MOVEMENT) {
        handleAlignmentSequencePhase();
    } else if (currentStep >= STEP_FINAL_LEFT_CLAMP && currentStep <= STEP_FINAL_SETTLE) {
        handleFinalClampPhase();
    }
}

//! ************************************************************************
//! RESET ALIGNMENT VARIABLES
//! ************************************************************************
void resetAlignmentVariables() {
    stepStartTime = 0;
    currentStep = 0;
}

//! ************************************************************************
//! INITIAL MOVEMENT PHASE HANDLER
//! ************************************************************************
void handleInitialMovementPhase() {
    switch (currentStep) {
        case STEP_INITIAL_MOVEMENT:
            //! ************************************************************************
            //! STEP 1: INITIAL FORWARD MOVEMENT
            //! ************************************************************************
            stepper->setSpeedInHz(Motion::ALIGNMENT_INITIAL_SPEED);
            stepper->moveTo(Motion::ALIGNMENT_INITIAL_POSITION * Motion::STEPS_PER_INCH);
            stepStartTime = millis();
            currentStep++;
            break;
            
        case STEP_WAIT_INITIAL:
            //! ************************************************************************
            //! WAIT FOR INITIAL MOVEMENT
            //! ************************************************************************
            if (!stepper->isRunning()) {
                stepStartTime = millis();
                currentStep++;
            }
            break;

        case STEP_REVERSE_MOVEMENT:
            //! ************************************************************************
            //! STEP 2: REVERSE A BIT TO MOVE OUT OF THE WAY
            //! ************************************************************************
            stepper->setSpeedInHz(Motion::ALIGNMENT_INITIAL_SPEED);
            stepper->moveTo(Motion::ALIGNMENT_BACKWARD_POSITION * Motion::STEPS_PER_INCH);
            extendLeftClamp();
            stepStartTime = millis();
            currentStep++;
            break;
            
        case STEP_WAIT_REVERSE:
            //! ************************************************************************
            //! WAIT FOR REVERSE MOVEMENT
            //! ************************************************************************
            if (!stepper->isRunning()) {
                retractLeftClamp();
                stepStartTime = millis();
                currentStep++;
            }
            break;
    }
}

//! ************************************************************************
//! ALIGNMENT SEQUENCE PHASE HANDLER
//! ************************************************************************
void handleAlignmentSequencePhase() {
    switch (currentStep) {
        case STEP_ALIGNMENT_CYLINDER_EXTEND:
            //! ************************************************************************
            //! STEP 3: ALIGNMENT CYLINDER FIRST, THEN LEFT CLAMP SEQUENCE
            //! ************************************************************************
            extendAlignmentCylinder();
            stepStartTime = millis();
            currentStep++;
            break;
            
        case STEP_WAIT_ALIGNMENT_PRE:
            //! ************************************************************************
            //! WAIT FOR ALIGNMENT CYLINDER TO POSITION MATERIAL
            //! ************************************************************************
            if (millis() - stepStartTime >= Timing::ALIGNMENT_CYLINDER_PRE_EXTEND_MS) {
                stepStartTime = millis();
                currentStep++;
            }
            break;
            
        case STEP_LEFT_CLAMP_EXTEND:
            //! ************************************************************************
            //! EXTEND LEFT CLAMP (SECURE MATERIAL AFTER ALIGNMENT)
            //! ************************************************************************
            extendLeftClamp();
            stepStartTime = millis();
            currentStep++;
            break;
            
        case STEP_WAIT_LEFT_CLAMP:
            //! ************************************************************************
            //! WAIT FOR LEFT CLAMP TO FULLY EXTEND
            //! ************************************************************************
            if (millis() - stepStartTime >= Timing::ALIGNMENT_LEFT_CLAMP_EXTEND_MS) {
                stepStartTime = millis();
                currentStep++;
            }
            break;
            
        case STEP_LEFT_CLAMP_RETRACT:
            //! ************************************************************************
            //! RETRACT LEFT CLAMP (RELEASE TO ALLOW FINE ADJUSTMENT)
            //! ************************************************************************
            retractLeftClamp();
            stepStartTime = millis();
            currentStep++;
            break;
            
        case STEP_RIGHT_CLAMP_EXTEND:
            //! ************************************************************************
            //! STEP 4: RIGHT CLAMP SEQUENCE WITH ALIGNMENT
            //! ************************************************************************
            extendRightClamp();
            stepStartTime = millis();
            currentStep++;
            break;
            
        case STEP_WAIT_RIGHT_EXTEND:
            //! ************************************************************************
            //! WAIT, THEN RETRACT ALIGNMENT CYLINDER
            //! ************************************************************************
            if (millis() - stepStartTime >= Timing::ALIGNMENT_SHORT_SETTLE_MS) {
                retractAlignmentCylinder();
                stepStartTime = millis();
                currentStep++;
            }
            break;
            
        case STEP_RIGHT_CLAMP_RETRACT:
            //! ************************************************************************
            //! WAIT (RIGHT CLAMP EXTENSION TIME)
            //! ************************************************************************
            if (millis() - stepStartTime >= Timing::ALIGNMENT_RIGHT_CLAMP_WAIT_MS) {
                retractRightClamp();
                stepStartTime = millis();
                currentStep++;
            }
            break;
            
        case STEP_ALIGNMENT_RIGHT_EXTEND:
            //! ************************************************************************
            //! EXTEND ALIGNMENT CYLINDER AND RIGHT CLAMP (PREP FOR FINAL POSITIONING)
            //! ************************************************************************
            extendAlignmentCylinder();
            extendRightClamp();
            stepStartTime = millis();
            currentStep++;
            break;
            
        case STEP_WAIT_ALIGNMENT_RIGHT:
            //! ************************************************************************
            //! WAIT, THEN RETRACT ALIGNMENT CYLINDER (CLEAR FOR CUTTING)
            //! ************************************************************************
            if (millis() - stepStartTime >= Timing::ALIGNMENT_SHORT_SETTLE_MS) {
                retractAlignmentCylinder();
                stepStartTime = millis();
                currentStep++;
            }
            break;
            
        case STEP_FINAL_MOVEMENT:
            //! ************************************************************************
            //! FINAL BACKWARD MOVEMENT TO FINAL ALIGNMENT POSITION
            //! ************************************************************************
            if (millis() - stepStartTime >= Timing::ALIGNMENT_LONG_SETTLE_MS) {
                stepper->setSpeedInHz(Motion::ALIGNMENT_INITIAL_SPEED);
                stepper->moveTo(Motion::ALIGNMENT_FINAL_POSITION * Motion::STEPS_PER_INCH);
                stepStartTime = millis();
                currentStep++;
            }
            break;
            
        case STEP_WAIT_FINAL_MOVEMENT:
            //! ************************************************************************
            //! WAIT FOR FINAL BACKWARD MOVEMENT TO COMPLETE
            //! ************************************************************************
            if (!stepper->isRunning()) {
                currentStep++;
                stepStartTime = millis();
            }
            break;
    }
}

//! ************************************************************************
//! FINAL CLAMP PHASE HANDLER
//! ************************************************************************
void handleFinalClampPhase() {
    switch (currentStep) {
        case STEP_FINAL_LEFT_CLAMP:
            //! ************************************************************************
            //! STEP 5: FINAL CLAMP ENGAGEMENT FOR CUTTING
            //! ************************************************************************
            extendLeftClamp();
            stepStartTime = millis();
            currentStep++;
            break;
            
        case STEP_FINAL_RIGHT_CLAMP:
            //! ************************************************************************
            //! EXTEND RIGHT CLAMP (DUAL-CLAMP SECURE HOLD)
            //! ************************************************************************
            extendRightClamp();
            stepStartTime = millis();
            currentStep++;
            break;
            
        case STEP_FINAL_SETTLE:
            //! ************************************************************************
            //! WAIT FOR FINAL CLAMP ENGAGEMENT BEFORE PROCEEDING TO CUTTING CYCLE
            //! ************************************************************************
            if (millis() - stepStartTime >= Timing::CLAMP_SETTLE_TIME) {
                setCurrentMotorPosition((long)(Motion::ALIGNMENT_FINAL_POSITION * Motion::STEPS_PER_INCH));
                resetAlignmentVariables();
                currentState = CUTTING;
            }
            break;
    }
} 