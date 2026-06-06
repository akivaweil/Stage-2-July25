// State machine functions
// Core state machine execution and state transition management

#include "StateMachine/StateMachine.h"

// State machine handler
// Main state machine execution function

void runStateMachine() {
    // Execute state machine
    switch (currentState) {
        case STATE_IDLE:
            handleIdleState();
            break;

        case STATE_HOMING:
            handleHomingState();
            break;

        case STATE_ALIGNMENT:
            handleAlignmentState();
            break;

        case STATE_CUTTING:
            handleCuttingState();
            break;

        case STATE_RETURNING:
            handleReturningState();
            break;

        case STATE_ROUTER_CLEAR_ERROR:
            handleRouterClearErrorState();
            break;
    }
}

// State transition

void changeState(SystemState newState) {
    // Only change if different state
    if (newState != currentState) {
        previousState = currentState;
        currentState = newState;
        stateStartTime = millis();
    }
} 