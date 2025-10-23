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
#define PHASE_RETURN_WAITING          3

//! ************************************************************************
//! STATIC VARIABLES FOR RETURNING STATE
//! ************************************************************************
static bool returnStarted = false;
static unsigned long returnStartTime = 0;
static unsigned long returnStateStartTime = 0;
static int currentPhase = PHASE_RETURN_PREPARATION;

//! ************************************************************************
//! FORWARD DECLARATIONS
//! ************************************************************************
void resetReturningVariables();
void handleReturnPreparationPhase();
void handleReturnMovementPhase();
void handleReturnCompletionPhase();
void handleReturnWaitingPhase();

//! ************************************************************************
//! MAIN RETURNING STATE HANDLER
//! ************************************************************************
void handleReturningState() {
    
    //! ************************************************************************
    //! INITIALIZE STATE START TIME ON FIRST ENTRY
    //! ************************************************************************
    if (!returnStarted && returnStateStartTime == 0) {
        returnStateStartTime = millis();
    }
    
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
            
        case PHASE_RETURN_WAITING:
            handleReturnWaitingPhase();
            break;
    }
}

//! ************************************************************************
//! RESET RETURNING VARIABLES
//! ************************************************************************
void resetReturningVariables() {
    returnStarted = false;
    returnStartTime = 0;
    returnStateStartTime = 0;
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
    currentPosition = getCurrentMotorPosition();
    
    //! ************************************************************************
    //! STEP 3: CHECK IF TIMEOUT HAS ELAPSED
    //! ************************************************************************
    unsigned long elapsedTime = millis() - returnStateStartTime;
    if (elapsedTime >= Timing::RETURN_TIMEOUT) {
        // Timeout reached - wait for start button press
        startButtonWasPressed = false; // Reset button tracking for waiting phase
        currentPhase = PHASE_RETURN_WAITING;
    } else {
        // No timeout yet - transition directly to homing
        homingComplete = false; // Reset homing flag to force homing sequence
        enableMotor(); // Ensure motor is enabled before homing
        resetReturningVariables();
        currentState = HOMING;
    }
}

//! ************************************************************************
//! RETURN WAITING PHASE HANDLER
//! ************************************************************************
void handleReturnWaitingPhase() {
    //! ************************************************************************
    //! WAIT FOR START BUTTON PRESS
    //! ************************************************************************
    updateInputs();
    
    bool startButtonCurrentlyPressed = startButton.read();
    if (startButtonCurrentlyPressed && !startButtonWasPressed) {
        // Start button pressed - reset motor and home
        startButtonWasPressed = true;
        
        // Stop any running motor movement
        stopMotor();
        
        // Disable and re-enable motor to clear any jams
        disableMotor();
        delay(100); // Brief pause while motor is disabled
        enableMotor();
        
        // Keep both clamps extended for safe material handling
        // Only retract alignment cylinder for safe homing
        retractAlignmentCylinder();
        
        // Reset homing flag to force homing sequence
        homingComplete = false;
        
        // Ensure motor is enabled before homing
        enableMotor();
        
        // Reset variables and transition to homing
        resetReturningVariables();
        currentState = HOMING;
    } else if (!startButtonCurrentlyPressed) {
        // Button is not pressed, reset the tracking variable
        startButtonWasPressed = false;
    }
} 