//* ************************************************************************
//* ************************ STATE MACHINE FUNCTIONS ******************
//* ************************************************************************
// Core state machine execution and state transition management

#include <Stage2_Machine.h>

//* ************************************************************************
//* ************************ STATE MACHINE EXECUTION *******************
//* ************************************************************************

void executeStateMachine() {
    switch (currentState) {
        case IDLE:
            state_IDLE();
            break;
            
        case HOMING:
            state_HOMING();
            break;
            
        case CUTTING_CYCLE:
            state_CUTTING_CYCLE();
            break;
            
        case CLAMP_ENGAGE:
            state_CLAMP_ENGAGE();
            break;
            
        case ALIGN_ENGAGE:
            state_ALIGN_ENGAGE();
            break;
            
        case APPROACH_MOVE:
            state_APPROACH_MOVE();
            break;
            
        case CUTTING_MOVE:
            state_CUTTING_MOVE();
            break;
            
        case FINISH_MOVE:
            state_FINISH_MOVE();
            break;
            
        case CLAMP_RELEASE:
            state_CLAMP_RELEASE();
            break;
            
        case ALIGN_RELEASE:
            state_ALIGN_RELEASE();
            break;
            
        case RETURN_MOVE:
            state_RETURN_MOVE();
            break;
            
        case CYCLE_COMPLETE:
            state_CYCLE_COMPLETE();
            break;
            
        case ERROR_STATE:
            state_ERROR_STATE();
            break;
            
        default:
            changeState(ERROR_STATE);
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
        
        // Log state changes
        Serial.print("State change: ");
        Serial.print(previousState);
        Serial.print(" -> ");
        Serial.println(currentState);
    }
} 