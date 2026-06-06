#include "StateMachine/StateMachine.h"
#include "Config/Config.h"

// Alignment state
// Short motor forward nudge, then alignment cylinder extends and dwells, left clamp extends,
// dwell extended, alignment retracts, dwell, right clamp extends; settle before CUTTING

// Alignment step constants
#define STEP_SHORT_FORWARD_MOVE          0
#define STEP_WAIT_SHORT_FORWARD          1
#define STEP_ALIGNMENT_CYLINDER_EXTEND   2
#define STEP_WAIT_ALIGNMENT_PRE          3
#define STEP_LEFT_CLAMP_EXTEND             4
#define STEP_WAIT_ALIGNMENT_STAY_EXTENDED 5
#define STEP_ALIGNMENT_CYLINDER_RETRACT    6
#define STEP_WAIT_AFTER_RETRACT            7
#define STEP_RIGHT_CLAMP_EXTEND            8
#define STEP_WAIT_CLAMP_SETTLE             9

// Static variables for alignment state
static unsigned long stepStartTime = 0;
static int currentStep = 0;

// Forward declarations
void resetAlignmentVariables();

// Main alignment state handler
void handleAlignmentState() {

    // Check for start button press - interrupt to homing
    if (checkStartButtonForHoming()) {
        resetAlignmentVariables();
        return;
    }

    // Step sequence
    switch (currentStep) {
        case STEP_SHORT_FORWARD_MOVE:
            // Step 1: short forward motor nudge
            stepper->setSpeedInHz(Motion::ALIGNMENT_INITIAL_SPEED);
            stepper->moveTo((long)(Motion::ALIGNMENT_SHORT_FORWARD_POSITION * Motion::STEPS_PER_INCH));
            stepStartTime = millis();
            currentStep++;
            break;

        case STEP_WAIT_SHORT_FORWARD:
            // Step 2: wait for short forward move to finish
            if (!stepper->isRunning()) {
                stepStartTime = millis();
                currentStep++;
            }
            break;

        case STEP_ALIGNMENT_CYLINDER_EXTEND:
            // Step 3: extend alignment cylinder
            extendAlignmentCylinder();
            stepStartTime = millis();
            currentStep++;
            break;

        case STEP_WAIT_ALIGNMENT_PRE:
            // Step 4: wait for alignment cylinder to position material
            if (millis() - stepStartTime >= Timing::ALIGNMENT_CYLINDER_PRE_EXTEND_MS) {
                stepStartTime = millis();
                currentStep++;
            }
            break;

        case STEP_LEFT_CLAMP_EXTEND:
            // Step 5: extend left clamp
            extendLeftClamp();
            stepStartTime = millis();
            currentStep++;
            break;

        case STEP_WAIT_ALIGNMENT_STAY_EXTENDED:
            // Step 6: keep alignment cylinder extended before retract
            if (millis() - stepStartTime >= Timing::ALIGNMENT_CYLINDER_EXTENDED_BEFORE_RETRACT_MS) {
                stepStartTime = millis();
                currentStep++;
            }
            break;

        case STEP_ALIGNMENT_CYLINDER_RETRACT:
            // Step 7: retract alignment cylinder
            retractAlignmentCylinder();
            stepStartTime = millis();
            currentStep++;
            break;

        case STEP_WAIT_AFTER_RETRACT:
            // Step 8: dwell while alignment cylinder retracted before right clamp
            if (millis() - stepStartTime >= Timing::ALIGNMENT_AFTER_RETRACT_BEFORE_RIGHT_CLAMP_MS) {
                stepStartTime = millis();
                currentStep++;
            }
            break;

        case STEP_RIGHT_CLAMP_EXTEND:
            // Step 9: extend right clamp
            extendRightClamp();
            stepStartTime = millis();
            currentStep++;
            break;

        case STEP_WAIT_CLAMP_SETTLE:
            // Step 10: settle before cutting cycle
            if (millis() - stepStartTime >= Timing::CLAMP_SETTLE_TIME) {
                resetAlignmentVariables();
                currentState = STATE_CUTTING;
            }
            break;
    }
}

// Reset alignment variables
void resetAlignmentVariables() {
    stepStartTime = 0;
    currentStep = 0;
}
