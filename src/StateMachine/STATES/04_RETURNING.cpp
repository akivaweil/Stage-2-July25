#include "StateMachine/StateMachine.h"

// Returning state
// This state moves the motor back toward the home switch with clamps extended for safe homing.
// After return is complete, transitions to HOMING state for end-of-cycle homing sequence.
// Clamps are retracted in the IDLE state after homing is complete.

// Returning phase constants
#define PHASE_RETURN_PREPARATION      0
#define PHASE_RETURN_MOVEMENT         1
#define PHASE_RETURN_COMPLETION       2
#define PHASE_RETURN_WAITING          3

// Static variables for returning state
static bool returnStarted = false;
static unsigned long returnStartTime = 0;
static unsigned long returnStateStartTime = 0;
static int currentPhase = PHASE_RETURN_PREPARATION;

// Forward declarations
void resetReturningVariables();
void handleReturnPreparationPhase();
void handleReturnMovementPhase();
void handleReturnCompletionPhase();
void handleReturnWaitingPhase();

// Main returning state handler
void handleReturningState() {

    // Initialize state start time on first entry
    if (!returnStarted && returnStateStartTime == 0) {
        returnStateStartTime = millis();
    }

    // Check for start button press - interrupt to homing
    if (checkStartButtonForHoming()) {
        resetReturningVariables();
        return; // Exit function, state will be changed to HOMING
    }

    // Phase routing
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

// Reset returning variables
void resetReturningVariables() {
    returnStarted = false;
    returnStartTime = 0;
    returnStateStartTime = 0;
    currentPhase = PHASE_RETURN_PREPARATION;
}

// Return preparation phase handler
void handleReturnPreparationPhase() {
    if (!returnStarted) {
        // Phase 6: return movement - move back toward home switch for safe homing

        // Step 1: retract alignment cylinder (ensure clear path)
        retractAlignmentCylinder();

        // Step 2: move FINAL_POSITION distance toward home switch at full speed
        float returnDistanceSteps = -Motion::FINAL_POSITION * Motion::STEPS_PER_INCH; // Negative = move toward home
        float returnSpeed = fastReturnFlag ? (Motion::RETURN_SPEED * Motion::RETURN_SPEED_FAST_MULT) : Motion::RETURN_SPEED;
        fastReturnFlag = false; // consume flag
        moveMotor(returnDistanceSteps, returnSpeed, Motion::RETURN_ACCEL);

        returnStarted = true;
        returnStartTime = millis();
        currentPhase = PHASE_RETURN_MOVEMENT;
    }
}

// Return movement phase handler
void handleReturnMovementPhase() {
    // Wait for return movement to complete
    if (!isMotorRunning()) {
        currentPhase = PHASE_RETURN_COMPLETION;
    }
}

// Return completion phase handler
void handleReturnCompletionPhase() {
    currentPosition = getCurrentMotorPosition();

    // Step 3: check if timeout has elapsed
    unsigned long elapsedTime = millis() - returnStateStartTime;
    if (elapsedTime >= Timing::RETURN_TIMEOUT) {
        // Timeout reached - wait for start button press
        startButtonWasPressed = false; // Reset button tracking for waiting phase
        currentPhase = PHASE_RETURN_WAITING;
    } else {
        // No timeout yet - transition directly to homing
        homingComplete = false; // Reset homing flag to force homing sequence
        enableMotor(); // Ensure motor is enabled before homing
        cycleEndTime = millis(); // Stamp for inter-cycle cooldown in IDLE
        resetReturningVariables();
        currentState = STATE_HOMING;
    }
}

// Return waiting phase handler
void handleReturnWaitingPhase() {
    // Wait for start button press
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
        cycleEndTime = millis(); // Stamp for inter-cycle cooldown in IDLE
        resetReturningVariables();
        currentState = STATE_HOMING;
    } else if (!startButtonCurrentlyPressed) {
        // Button is not pressed, reset the tracking variable
        startButtonWasPressed = false;
    }
}
