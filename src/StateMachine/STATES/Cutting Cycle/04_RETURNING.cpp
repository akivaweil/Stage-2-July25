#include <Stage2_Machine.h>

//* ************************************************************************
//* ************************ RETURNING STATE *****************************
//* ************************************************************************
// This state moves the motor back toward the home switch with clamps extended for safe homing.
// After return is complete, transitions to HOMING state for end-of-cycle homing sequence.
// Clamps are retracted in the IDLE state after homing is complete.

//! ************************************************************************
//! RETURNING PHASE CONSTANTS
//! ************************************************************************
#define PHASE_RETURN_PREPARATION      0
#define PHASE_RETURN_MOVEMENT         1
#define PHASE_RETURN_COMPLETION       2

//! ************************************************************************
//! STATIC VARIABLES FOR RETURNING STATE
//! ************************************************************************
static bool returnStarted = false;
static unsigned long returnStartTime = 0;
static int currentPhase = PHASE_RETURN_PREPARATION;

//! ************************************************************************
//! FORWARD DECLARATIONS
//! ************************************************************************
void resetReturningVariables();
void handleReturnPreparationPhase();
void handleReturnMovementPhase();
void handleReturnCompletionPhase();

//! ************************************************************************
//! MAIN RETURNING STATE HANDLER
//! ************************************************************************
void handleReturningState() {
    
    //! ************************************************************************
    //! CHECK FOR START BUTTON PRESS - INTERRUPT TO HOMING
    //! ************************************************************************
    if (checkStartButtonForHoming()) {
        resetReturningVariables();
        return; // Exit function, state will be changed to HOMING
    }
    
    //! ************************************************************************
    //! PHASE ROUTING
    //! ************************************************************************
    switch (currentPhase) {
        case PHASE_RETURN_PREPARATION:
            handleReturnPreparationPhase();
            break;
            
        case PHASE_RETURN_MOVEMENT:
            handleReturnMovementPhase();
            break;
            
        case PHASE_RETURN_COMPLETION:
            handleReturnCompletionPhase();
            break;
    }
}

//! ************************************************************************
//! RESET RETURNING VARIABLES
//! ************************************************************************
void resetReturningVariables() {
    returnStarted = false;
    returnStartTime = 0;
    currentPhase = PHASE_RETURN_PREPARATION;
}

//! ************************************************************************
//! RETURN PREPARATION PHASE HANDLER
//! ************************************************************************
void handleReturnPreparationPhase() {
    if (!returnStarted) {
        //! ************************************************************************
        //! PHASE 6: RETURN MOVEMENT - MOVE BACK TOWARD HOME SWITCH FOR SAFE HOMING
        //! ************************************************************************
        // Transfer arm signal already set HIGH in cutting state (prevents Z-axis interference)
        
        //! ************************************************************************
        //! STEP 1: RETRACT ALIGNMENT CYLINDER (ENSURE CLEAR PATH)
        //! ************************************************************************
        retractAlignmentCylinder();
        
        //! ************************************************************************
        //! STEP 2: MOVE FINAL_POSITION DISTANCE TOWARD HOME SWITCH AT FULL SPEED
        //! ************************************************************************
        float returnDistanceSteps = -Motion::FINAL_POSITION * Motion::STEPS_PER_INCH; // Negative = move toward home
        moveMotor(returnDistanceSteps, Motion::RETURN_SPEED, Motion::RETURN_ACCEL);
        
        returnStarted = true;
        returnStartTime = millis();
        currentPhase = PHASE_RETURN_MOVEMENT;
    }
}

//! ************************************************************************
//! RETURN MOVEMENT PHASE HANDLER
//! ************************************************************************
void handleReturnMovementPhase() {
    //! ************************************************************************
    //! WAIT FOR RETURN MOVEMENT TO COMPLETE
    //! ************************************************************************
    if (!isMotorRunning()) {
        currentPhase = PHASE_RETURN_COMPLETION;
    }
}

//! ************************************************************************
//! RETURN COMPLETION PHASE HANDLER
//! ************************************************************************
void handleReturnCompletionPhase() {
    //! ************************************************************************
    //! STEP 3: COMPLETE TRANSFER ARM SIGNAL AND RETURN TO IDLE
    //! ************************************************************************
    digitalWrite(Pins::TRANSFER_ARM_SIGNAL, LOW);
    currentPosition = getCurrentMotorPosition();
    
    //! ************************************************************************
    //! STEP 4: TRANSITION TO HOMING FOR END-OF-CYCLE HOMING SEQUENCE
    //! ************************************************************************
    homingComplete = false; // Reset homing flag to force homing sequence
    resetReturningVariables();
    currentState = HOMING;
} 