#include <Stage2_Machine.h>

//* ************************************************************************
//* ************************ CUTTING STATE *******************************
//* ************************************************************************
// This state performs the complete cutting sequence: approach, cut, finish, temporarily release clamps, then re-extend for return

//! ************************************************************************
//! CUTTING PHASE CONSTANTS
//! ************************************************************************
#define PHASE_APPROACH_SET_TARGET      0
#define PHASE_APPROACH_VERIFY          1
#define PHASE_CUTTING_SET_TARGET       2
#define PHASE_CUTTING_VERIFY           3
#define PHASE_FINISH_SET_TARGET        4
#define PHASE_FINISH_VERIFY            5
#define PHASE_POSITION_CHECK           6
#define PHASE_POSITION_MONITOR         7
#define PHASE_SETTLE_TIME              8
#define PHASE_CLAMP_RELEASE            9
#define PHASE_OSCILLATION              10
#define PHASE_RE_EXTEND_CLAMPS         11
#define PHASE_WAIT_RE_EXTENSION        12
#define PHASE_PREPARE_RETURN           13

//! ************************************************************************
//! STATIC VARIABLES FOR CUTTING STATE
//! ************************************************************************
static unsigned long stepStartTime = 0;
static int cuttingPhase = 0;
static bool motionComplete = false;
static float targetPosition = 0;
static float initialFinalPosition = 0;
static float centerPosition = 0;          // Center position for oscillation
static bool oscillatingForward = true;    // Direction of oscillation

//! ************************************************************************
//! FORWARD DECLARATIONS
//! ************************************************************************
void resetCuttingVariables();
void handleApproachPhase();
void handleCuttingPhase();
void handleFinishPhase();
void handlePositionVerificationPhase();
void handleSettleTimePhase();
void handleClampReleasePhase();
void handleOscillationPhase();
void handleReExtendPhase();
void handlePrepareReturnPhase();

//! ************************************************************************
//! MAIN CUTTING STATE HANDLER
//! ************************************************************************
void handleCuttingState() {
    
    //! ************************************************************************
    //! CHECK FOR START BUTTON PRESS - INTERRUPT TO HOMING
    //! ************************************************************************
    if (checkStartButtonForHoming()) {
        resetCuttingVariables();
        return; // Exit function, state will be changed to HOMING
    }
    
    // Initialize step timing
    if (stepStartTime == 0) {
        stepStartTime = millis();
    }
    
    //! ************************************************************************
    //! PHASE ROUTING
    //! ************************************************************************
    switch (cuttingPhase) {
        case PHASE_APPROACH_SET_TARGET:
        case PHASE_APPROACH_VERIFY:
            handleApproachPhase();
            break;
            
        case PHASE_CUTTING_SET_TARGET:
        case PHASE_CUTTING_VERIFY:
            handleCuttingPhase();
            break;
            
        case PHASE_FINISH_SET_TARGET:
        case PHASE_FINISH_VERIFY:
            handleFinishPhase();
            break;
            
        case PHASE_POSITION_CHECK:
        case PHASE_POSITION_MONITOR:
            handlePositionVerificationPhase();
            break;
            
        case PHASE_SETTLE_TIME:
            handleSettleTimePhase();
            break;
            
        case PHASE_CLAMP_RELEASE:
        case PHASE_OSCILLATION:
            handleClampReleasePhase();
            break;
            
        case PHASE_RE_EXTEND_CLAMPS:
        case PHASE_WAIT_RE_EXTENSION:
            handleReExtendPhase();
            break;
            
        case PHASE_PREPARE_RETURN:
            handlePrepareReturnPhase();
            break;
    }
}

//! ************************************************************************
//! RESET CUTTING VARIABLES
//! ************************************************************************
void resetCuttingVariables() {
    stepStartTime = 0;
    cuttingPhase = 0;
    motionComplete = false;
    targetPosition = 0;
    initialFinalPosition = 0;
    centerPosition = 0;
    oscillatingForward = true;
}

//! ************************************************************************
//! APPROACH PHASE HANDLER
//! ************************************************************************
void handleApproachPhase() {
    switch (cuttingPhase) {
        case PHASE_APPROACH_SET_TARGET:
            //! ************************************************************************
            //! APPROACH: SET TARGET POSITION
            //! ************************************************************************
            targetPosition = Motion::APPROACH_POSITION * Motion::STEPS_PER_INCH;
            moveMotorToPosition(targetPosition, Motion::APPROACH_SPEED, Motion::FORWARD_ACCEL);
            
            stepStartTime = millis();
            cuttingPhase++;
            break;
            
        case PHASE_APPROACH_VERIFY:
            //! ************************************************************************
            //! VERIFY APPROACH POSITION
            //! ************************************************************************
            if (!isMotorRunning()) {
                currentPosition = getCurrentMotorPosition();
                cuttingPhase++;
                stepStartTime = millis();
            }
            break;
    }
}

//! ************************************************************************
//! CUTTING PHASE HANDLER
//! ************************************************************************
void handleCuttingPhase() {
    switch (cuttingPhase) {
        case PHASE_CUTTING_SET_TARGET:
            //! ************************************************************************
            //! CUTTING: CALCULATE TARGET
            //! ************************************************************************
            targetPosition = Motion::CUTTING_POSITION * Motion::STEPS_PER_INCH;
            moveMotorToPosition(targetPosition, Motion::CUTTING_SPEED, Motion::FORWARD_ACCEL / 2);
            
            stepStartTime = millis();
            cuttingPhase++;
            break;
            
        case PHASE_CUTTING_VERIFY:
            //! ************************************************************************
            //! VERIFY CUTTING COMPLETION
            //! ************************************************************************
            if (!isMotorRunning()) {
                currentPosition = getCurrentMotorPosition();
                cuttingPhase++;
                stepStartTime = millis();
            }
            break;
    }
}

//! ************************************************************************
//! FINISH PHASE HANDLER
//! ************************************************************************
void handleFinishPhase() {
    switch (cuttingPhase) {
        case PHASE_FINISH_SET_TARGET:
            //! ************************************************************************
            //! FINISH: SET FINAL TARGET POSITION
            //! ************************************************************************
            targetPosition = Motion::FINAL_POSITION * Motion::STEPS_PER_INCH;
            moveMotorToPosition(targetPosition, Motion::FINISH_SPEED, Motion::FORWARD_ACCEL);
            
            stepStartTime = millis();
            cuttingPhase++;
            break;
            
        case PHASE_FINISH_VERIFY:
            //! ************************************************************************
            //! VERIFY FINISH COMPLETION
            //! ************************************************************************
            if (!isMotorRunning()) {
                currentPosition = getCurrentMotorPosition();
                initialFinalPosition = currentPosition;
                cuttingPhase++;
                stepStartTime = millis();
            }
            break;
    }
}

//! ************************************************************************
//! POSITION VERIFICATION PHASE HANDLER
//! ************************************************************************
void handlePositionVerificationPhase() {
    switch (cuttingPhase) {
        case PHASE_POSITION_CHECK:
            //! ************************************************************************
            //! POSITION VERIFICATION: MOVE CONTINUOUSLY UNTIL SENSOR TRIGGERS
            //! ************************************************************************
            if (endPositionVerificationSensor.read()) {
                positionVerificationDistance = 0.0;
                cuttingPhase = PHASE_SETTLE_TIME;
                stepStartTime = millis();
            } else {
                targetPosition = currentPosition + (10.0 * Motion::STEPS_PER_INCH);
                moveMotorToPosition(targetPosition, Motion::FINAL_SPEED, Motion::FORWARD_ACCEL);
                stepStartTime = millis();
                cuttingPhase++;
            }
            break;
            
        case PHASE_POSITION_MONITOR:
            //! ************************************************************************
            //! MONITOR POSITION VERIFICATION SENSOR - STOP WHEN TRIGGERED
            //! ************************************************************************
            if (endPositionVerificationSensor.read()) {
                stopMotor();
                currentPosition = getCurrentMotorPosition();
                positionVerificationDistance = currentPosition - initialFinalPosition;
                cuttingPhase = PHASE_SETTLE_TIME;
                stepStartTime = millis();
            }
            break;
    }
}

//! ************************************************************************
//! SETTLE TIME PHASE HANDLER
//! ************************************************************************
void handleSettleTimePhase() {
    //! ************************************************************************
    //! SETTLE TIME: WAIT 150MS
    //! ************************************************************************
    if (millis() - stepStartTime >= Timing::CLAMP_SETTLE_TIME) {
        //! ************************************************************************
        //! CHECK IS_ROUTER_CLEAR SENSOR (ACTIVE LOW) - IF ACTIVE, TRIGGER ERROR STATE
        //! ************************************************************************
        if (digitalRead(Pins::IS_ROUTER_CLEAR) == LOW) {
            stateBeforeError = CUTTING;
            resetCuttingVariables();
            currentState = ROUTER_CLEAR_ERROR;
            return;
        }
        
        cuttingPhase++;
        stepStartTime = millis();
    }
}

//! ************************************************************************
//! CLAMP RELEASE PHASE HANDLER
//! ************************************************************************
void handleClampReleasePhase() {
    switch (cuttingPhase) {
        case PHASE_CLAMP_RELEASE:
            //! ************************************************************************
            //! CLAMP RELEASE: RETRACT BOTH CLAMPS TEMPORARILY
            //! ************************************************************************
            retractBothClamps();
            digitalWrite(Pins::CLAMP_RELEASE_SIGNAL, HIGH);
            centerPosition = getCurrentMotorPosition();
            oscillatingForward = true;
            
            stepStartTime = millis();
            cuttingPhase++;
            break;
            
        case PHASE_OSCILLATION:
            //! ************************************************************************
            //! OSCILLATION DURING CLAMP RELEASE (WITHIN 400MS TOTAL TIME)
            //! ************************************************************************
            {
                unsigned long elapsedTime = millis() - stepStartTime;
                
                if (elapsedTime >= 100) { // Start oscillation after 100ms delay
                    if (!isMotorRunning()) {
                        float oscillationDistance = Timing::OSCILLATION_DISTANCE * Motion::STEPS_PER_INCH;
                        float targetOscPos = oscillatingForward ? 
                            centerPosition + oscillationDistance : 
                            centerPosition - oscillationDistance;
                        
                        moveMotorToPosition(targetOscPos, Timing::OSCILLATION_SPEED, Timing::OSCILLATION_ACCEL);
                        oscillatingForward = !oscillatingForward;
                    }
                }
                
                if (elapsedTime >= Timing::CLAMP_RELEASE_TIME) {
                    stopMotor();
                    cuttingPhase++;
                    stepStartTime = millis();
                }
            }
            break;
    }
}

//! ************************************************************************
//! RE-EXTEND PHASE HANDLER
//! ************************************************************************
void handleReExtendPhase() {
    switch (cuttingPhase) {
        case PHASE_RE_EXTEND_CLAMPS:
            //! ************************************************************************
            //! RE-EXTEND CLAMPS FOR RETURN JOURNEY
            //! ************************************************************************
            extendBothClamps();
            stepStartTime = millis();
            cuttingPhase++;
            break;
            
        case PHASE_WAIT_RE_EXTENSION:
            //! ************************************************************************
            //! WAIT FOR CLAMP RE-EXTENSION
            //! ************************************************************************
            if (millis() - stepStartTime >= 0) { // Skip settle time
                cuttingPhase++;
                stepStartTime = millis();
            }
            break;
    }
}

//! ************************************************************************
//! PREPARE RETURN PHASE HANDLER
//! ************************************************************************
void handlePrepareReturnPhase() {
    //! ************************************************************************
    //! SIGNAL TRANSFER ARM AND TRANSITION TO RETURNING STATE
    //! ************************************************************************
    digitalWrite(Pins::TRANSFER_ARM_SIGNAL, HIGH);
    digitalWrite(Pins::CLAMP_RELEASE_SIGNAL, LOW);
    
    resetCuttingVariables();
    currentState = RETURNING;
} 