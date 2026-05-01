#include <Stage2_Machine.h>
#include <Config.h>

//* ************************************************************************
//* ************************ ALIGNMENT STATE ****************************
//* ************************************************************************
// Alignment cylinder extends and dwells, then left and right clamps extend; no stepper choreography

//! ************************************************************************
//! ALIGNMENT STEP CONSTANTS
//! ************************************************************************
#define STEP_ALIGNMENT_CYLINDER_EXTEND 0
#define STEP_WAIT_ALIGNMENT_PRE        1
#define STEP_BOTH_CLAMPS_EXTEND        2
#define STEP_WAIT_CLAMP_SETTLE         3

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
        case STEP_ALIGNMENT_CYLINDER_EXTEND:
            //! ************************************************************************
            //! STEP 1: EXTEND ALIGNMENT CYLINDER
            //! ************************************************************************
            extendAlignmentCylinder();
            stepStartTime = millis();
            currentStep++;
            break;

        case STEP_WAIT_ALIGNMENT_PRE:
            //! ************************************************************************
            //! STEP 2: WAIT FOR ALIGNMENT CYLINDER TO POSITION MATERIAL
            //! ************************************************************************
            if (millis() - stepStartTime >= Timing::ALIGNMENT_CYLINDER_PRE_EXTEND_MS) {
                stepStartTime = millis();
                currentStep++;
            }
            break;

        case STEP_BOTH_CLAMPS_EXTEND:
            //! ************************************************************************
            //! STEP 3: EXTEND LEFT AND RIGHT CLAMPS
            //! ************************************************************************
            extendBothClamps();
            stepStartTime = millis();
            currentStep++;
            break;

        case STEP_WAIT_CLAMP_SETTLE:
            //! ************************************************************************
            //! STEP 4: SETTLE BEFORE CUTTING CYCLE
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
