//* ************************************************************************
//* ************************ STATE MACHINE FUNCTIONS ******************
//* ************************************************************************
// Core state machine execution and state transition management

#include <Stage2_Machine.h>

//* ************************************************************************
//* ************************ STATE MACHINE HANDLER **********************
//* ************************************************************************
// Main state machine execution function

void runStateMachine() {
    // Handle OTA updates - pause state machine if OTA is in progress
    if (isOTAInProgress()) {
        return; // Skip state machine execution during OTA
    }
    
    switch (currentState) {
        case IDLE:
            handleIdleState();
            break;
            
        case HOMING:
            handleHomingState();
            break;
            
        case ALIGNMENT:
            handleAlignmentState();
            break;
            
        case CUTTING:
            handleCuttingState();
            break;
            
        case RETURNING:
            handleReturningState();
            break;
    }
}

//* ************************************************************************
//* ************************ STATE TRANSITION ***************************
//* ************************************************************************

void changeState(MachineState newState) {
    // Only change if different state
    if (newState != currentState) {
        previousState = currentState;
        currentState = newState;
        stateStartTime = millis();
    }
} 