#include <Stage2_Machine.h>
#include <Config.h>

//* ************************************************************************
//* ************************ ALIGNMENT STATE ****************************
//* ************************************************************************
// Short motor forward nudge, then alignment cylinder extends and dwells, left clamp extends,
// dwell extended, alignment retracts, dwell, right clamp extends; settle before CUTTING

//! ************************************************************************
//! ALIGNMENT STEP CONSTANTS
//! ************************************************************************
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

//! ************************************************************************
//! STATIC VARIABLES FOR ALIGNMENT STATE
//! ************************************************************************
static unsigned long stepStartTime = 0;
static int currentStep = 0;

//! ************************************************************************
//! FORWARD DECLARATIONS
//! ************************************************************************
void resetAlignmentVariables();

//! ************************************************************************
//! MAIN ALIGNMENT STATE HANDLER
//! ************************************************************************
void handleAlignmentState() {

    //! ************************************************************************
    //! CHECK FOR START BUTTON PRESS - INTERRUPT TO HOMING
    //! ************************************************************************
    if (checkStartButtonForHoming()) {
        resetAlignmentVariables();
        return;
    }

    //! ************************************************************************
    //! STEP SEQUENCE
    //! ************************************************************************
    switch (currentStep) {
        case STEP_SHORT_FORWARD_MOVE:
            //! ************************************************************************
            //! STEP 1: SHORT FORWARD MOTOR NUDGE
            //! ************************************************************************
            stepper->setSpeedInHz(Motion::ALIGNMENT_INITIAL_SPEED);
            stepper->moveTo((long)(Motion::ALIGNMENT_SHORT_FORWARD_POSITION * Motion::STEPS_PER_INCH));
            stepStartTime = millis();
            currentStep++;
            break;

        case STEP_WAIT_SHORT_FORWARD:
            //! ************************************************************************
            //! STEP 2: WAIT FOR SHORT FORWARD MOVE TO FINISH
            //! ************************************************************************
            if (!stepper->isRunning()) {
                stepStartTime = millis();
                currentStep++;
            }
            break;

        case STEP_ALIGNMENT_CYLINDER_EXTEND:
            //! ************************************************************************
            //! STEP 3: EXTEND ALIGNMENT CYLINDER
            //! ************************************************************************
            extendAlignmentCylinder();
            stepStartTime = millis();
            currentStep++;
            break;

        case STEP_WAIT_ALIGNMENT_PRE:
            //! ************************************************************************
            //! STEP 4: WAIT FOR ALIGNMENT CYLINDER TO POSITION MATERIAL
            //! ************************************************************************
            if (millis() - stepStartTime >= Timing::ALIGNMENT_CYLINDER_PRE_EXTEND_MS) {
                stepStartTime = millis();
                currentStep++;
            }
            break;

        case STEP_LEFT_CLAMP_EXTEND:
            //! ************************************************************************
            //! STEP 5: EXTEND LEFT CLAMP
            //! ************************************************************************
            extendLeftClamp();
            stepStartTime = millis();
            currentStep++;
            break;

        case STEP_WAIT_ALIGNMENT_STAY_EXTENDED:
            //! ************************************************************************
            //! STEP 6: KEEP ALIGNMENT CYLINDER EXTENDED BEFORE RETRACT
            //! ************************************************************************
            if (millis() - stepStartTime >= Timing::ALIGNMENT_CYLINDER_EXTENDED_BEFORE_RETRACT_MS) {
                stepStartTime = millis();
                currentStep++;
            }
            break;

        case STEP_ALIGNMENT_CYLINDER_RETRACT:
            //! ************************************************************************
            //! STEP 7: RETRACT ALIGNMENT CYLINDER
            //! ************************************************************************
            retractAlignmentCylinder();
            stepStartTime = millis();
            currentStep++;
            break;

        case STEP_WAIT_AFTER_RETRACT:
            //! ************************************************************************
            //! STEP 8: DWELL WHILE ALIGNMENT CYLINDER RETRACTED BEFORE RIGHT CLAMP
            //! ************************************************************************
            if (millis() - stepStartTime >= Timing::ALIGNMENT_AFTER_RETRACT_BEFORE_RIGHT_CLAMP_MS) {
                stepStartTime = millis();
                currentStep++;
            }
            break;

        case STEP_RIGHT_CLAMP_EXTEND:
            //! ************************************************************************
            //! STEP 9: EXTEND RIGHT CLAMP
            //! ************************************************************************
            extendRightClamp();
            stepStartTime = millis();
            currentStep++;
            break;

        case STEP_WAIT_CLAMP_SETTLE:
            //! ************************************************************************
            //! STEP 10: SETTLE BEFORE CUTTING CYCLE
            //! ************************************************************************
            if (millis() - stepStartTime >= Timing::CLAMP_SETTLE_TIME) {
                resetAlignmentVariables();
                currentState = CUTTING;
            }
            break;
    }
}

//! ************************************************************************
//! RESET ALIGNMENT VARIABLES
//! ************************************************************************
void resetAlignmentVariables() {
    stepStartTime = 0;
    currentStep = 0;
}
