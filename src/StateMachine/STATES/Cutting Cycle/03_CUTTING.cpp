#include <Stage2_Machine.h>
#include "OTA/OTA_Upload.h"

//* ************************************************************************
//* ************************ CUTTING STATE *******************************
//* ************************************************************************
// This state performs the complete cutting sequence: approach, cut, finish, then a staged drop-off release (right clamp → router start signal → left clamp) before returning home
 
//! ************************************************************************
//! CUTTING CONFIGURATION (TIMING, ETC.)
//! ************************************************************************
namespace CuttingConfig {
    const int ROUTER_SIGNAL_ON_TIME_MS  = 500;  // Router signal ON duration
    const int ROUTER_SIGNAL_OFF_TIME_MS = 100;  // Router signal OFF duration between pulses
}  // namespace CuttingConfig

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
#define PHASE_RELEASE_RIGHT_CLAMP      9   // Drop-off: retract right clamp first
#define PHASE_WAIT_AFTER_RIGHT         10  // Drop-off: 100ms dwell before router start signal
#define PHASE_SEND_ROUTER_START        11  // Drop-off: pulse router start signal
#define PHASE_WAIT_AFTER_ROUTER        12  // Drop-off: 200ms dwell before left clamp release
#define PHASE_RELEASE_LEFT_CLAMP       13  // Drop-off: retract left clamp
#define PHASE_WAIT_AFTER_LEFT          14  // Drop-off: 200ms dwell before continuing
#define PHASE_PREPARE_RETURN           15
#define PHASE_WAIT_ROUTER_CLEAR        16  // Hold at drop-off until start button pressed

//! ************************************************************************
//! STATIC VARIABLES FOR CUTTING STATE
//! ************************************************************************
static unsigned long stepStartTime = 0;
static unsigned long routerSignalSegmentStartTime = 0;
static int cuttingPhase = 0;
static int routerSignalStage = 0;          // 0 = idle, 1-5 = pulse/gap sequence
static bool motionComplete = false;
static float targetPosition = 0;
static float initialFinalPosition = 0;

//! ************************************************************************
//! FORWARD DECLARATIONS
//! ************************************************************************
void resetCuttingVariables();
void startRouterSignalPattern();
void updateRouterSignalPattern();
void handleApproachPhase();
void handleCuttingPhase();
void handleFinishPhase();
void handlePositionVerificationPhase();
void handleSettleTimePhase();
void handleDropoffSequencePhase();
void handlePrepareReturnPhase();
void handleWaitRouterClearPhase();

//! ************************************************************************
//! MAIN CUTTING STATE HANDLER
//! ************************************************************************
void handleCuttingState() {
    
    //! ************************************************************************
    //! CHECK FOR START BUTTON PRESS - INTERRUPT TO HOMING
    //! (skipped during PHASE_WAIT_ROUTER_CLEAR — that phase owns the button)
    //! ************************************************************************
    if (cuttingPhase != PHASE_WAIT_ROUTER_CLEAR) {
        if (checkStartButtonForHoming()) {
            resetCuttingVariables();
            return; // Exit function, state will be changed to HOMING
        }
    }
    
    // Update router signal pattern (non-blocking)
    updateRouterSignalPattern();
    
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

        case PHASE_RELEASE_RIGHT_CLAMP:
        case PHASE_WAIT_AFTER_RIGHT:
        case PHASE_SEND_ROUTER_START:
        case PHASE_WAIT_AFTER_ROUTER:
        case PHASE_RELEASE_LEFT_CLAMP:
        case PHASE_WAIT_AFTER_LEFT:
            handleDropoffSequencePhase();
            break;

        case PHASE_PREPARE_RETURN:
            handlePrepareReturnPhase();
            break;
            
        case PHASE_WAIT_ROUTER_CLEAR:
            handleWaitRouterClearPhase();
            break;
    }
}

//! ************************************************************************
//! RESET CUTTING VARIABLES
//! ************************************************************************
void resetCuttingVariables() {
    stepStartTime = 0;
    routerSignalSegmentStartTime = 0;
    cuttingPhase = 0;
    routerSignalStage = 0;
    motionComplete = false;
    targetPosition = 0;
    initialFinalPosition = 0;
}

//! ************************************************************************
//! ROUTER SIGNAL HELPERS (NON-BLOCKING)
//! ************************************************************************
void startRouterSignalPattern() {
    // Begin pattern: ON for 500ms, OFF for 100ms, repeated 3 times
    routerSignalStage = 1;
    routerSignalSegmentStartTime = millis();
    sendRouterSignal(1);
}

void updateRouterSignalPattern() {
    if (routerSignalStage == 0) {
        return; // No active pattern
    }
    
    unsigned long now = millis();
    unsigned long segmentElapsed = now - routerSignalSegmentStartTime;
    
    switch (routerSignalStage) {
        case 1: // ON segment 1
            if (segmentElapsed >= CuttingConfig::ROUTER_SIGNAL_ON_TIME_MS) {
                sendRouterSignal(0);
                routerSignalStage = 2;
                routerSignalSegmentStartTime = now;
            }
            break;
            
        case 2: // OFF segment 1
            if (segmentElapsed >= CuttingConfig::ROUTER_SIGNAL_OFF_TIME_MS) {
                sendRouterSignal(1);
                routerSignalStage = 3;
                routerSignalSegmentStartTime = now;
            }
            break;
            
        case 3: // ON segment 2
            if (segmentElapsed >= CuttingConfig::ROUTER_SIGNAL_ON_TIME_MS) {
                sendRouterSignal(0);
                routerSignalStage = 4;
                routerSignalSegmentStartTime = now;
            }
            break;
            
        case 4: // OFF segment 2
            if (segmentElapsed >= CuttingConfig::ROUTER_SIGNAL_OFF_TIME_MS) {
                sendRouterSignal(1);
                routerSignalStage = 5;
                routerSignalSegmentStartTime = now;
            }
            break;
            
        case 5: // ON segment 3 (final)
            if (segmentElapsed >= CuttingConfig::ROUTER_SIGNAL_ON_TIME_MS) {
                sendRouterSignal(0);
                routerSignalStage = 0;
            }
            break;
            
        default:
            // Safety fallback
            sendRouterSignal(0);
            routerSignalStage = 0;
            break;
    }
}

void setCuttingPhaseToContinue() {
    // Resume drop-off sequence (right after router clear check)
    cuttingPhase = PHASE_RELEASE_RIGHT_CLAMP;
    stepStartTime = millis();
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
    //! SETTLE TIME: WAIT FOR CLAMP_SETTLE_TIME
    //! ************************************************************************
    if (millis() - stepStartTime >= Timing::CLAMP_SETTLE_TIME) {
        //! ************************************************************************
        //! CHECK IS_ROUTER_CLEAR PHYSICAL SENSOR (ACTIVE LOW)
        //! If triggered: hold at drop-off and wait for start button
        //! ************************************************************************
        if (digitalRead(Pins::IS_ROUTER_CLEAR) == LOW) {
            startButtonWasPressed = false; // reset so wait phase sees a clean edge
            cuttingPhase = PHASE_WAIT_ROUTER_CLEAR;
            return;
        }
        
        cuttingPhase++;
        stepStartTime = millis();
    }
}

// DROP-OFF RELEASE SEQUENCE
// Right clamp release → dwell → router start signal → dwell → left clamp
// release → dwell → continue to return-home.
void handleDropoffSequencePhase() {
    switch (cuttingPhase) {
        case PHASE_RELEASE_RIGHT_CLAMP:
            retractRightClamp();
            stepStartTime = millis();
            cuttingPhase++;
            break;

        case PHASE_WAIT_AFTER_RIGHT:
            if (millis() - stepStartTime >= Timing::DROPOFF_AFTER_RIGHT_RELEASE_MS) {
                cuttingPhase++;
                stepStartTime = millis();
            }
            break;

        case PHASE_SEND_ROUTER_START:
            // Single rising edge — router treats each rising edge as one cycle start.
            // Driven back LOW by handlePrepareReturnPhase() at end of drop-off sequence.
            sendRouterSignal(1);
            stepStartTime = millis();
            cuttingPhase++;
            break;

        case PHASE_WAIT_AFTER_ROUTER:
            if (millis() - stepStartTime >= Timing::DROPOFF_AFTER_ROUTER_SIGNAL_MS) {
                cuttingPhase++;
                stepStartTime = millis();
            }
            break;

        case PHASE_RELEASE_LEFT_CLAMP:
            retractLeftClamp();
            stepStartTime = millis();
            cuttingPhase++;
            break;

        case PHASE_WAIT_AFTER_LEFT:
            if (millis() - stepStartTime >= Timing::DROPOFF_AFTER_LEFT_RELEASE_MS) {
                cuttingPhase = PHASE_PREPARE_RETURN;
                stepStartTime = millis();
            }
            break;
    }
}

//! ************************************************************************
//! WAIT ROUTER CLEAR PHASE HANDLER
//! ************************************************************************
// IS_ROUTER_CLEAR sensor was triggered at drop-off point.
// Pause in place until start button is pressed, then resume the normal
// cutting sequence exactly where it left off.
void handleWaitRouterClearPhase() {
    updateInputs();

    bool startButtonCurrentlyPressed = startButton.read();
    if (startButtonCurrentlyPressed && !startButtonWasPressed) {
        startButtonWasPressed = true;

        //! Resume normal sequence from drop-off release
        cuttingPhase = PHASE_RELEASE_RIGHT_CLAMP;
        stepStartTime = millis();
    } else if (!startButtonCurrentlyPressed) {
        startButtonWasPressed = false;
    }
}

//! ************************************************************************
//! PREPARE RETURN PHASE HANDLER
//! ************************************************************************
void handlePrepareReturnPhase() {
    //! ************************************************************************
    //! CHECK FOR HOMING FLAG
    //! ************************************************************************
    sendRouterSignal(0);
    
    resetCuttingVariables();
    
    // Check if flag is set to go to homing instead of returning
    if (cutToHomingFlag) {
        Serial.println("Cutting: cutToHomingFlag is TRUE - going to HOMING");
        cutToHomingFlag = false; // Reset flag
        enableMotor(); // Ensure motor is enabled before homing
        currentState = HOMING;
    } else {
        Serial.println("Cutting: cutToHomingFlag is FALSE - going to RETURNING");
        currentState = RETURNING;
    }
} 