#include "StateMachine/StateMachine.h"

// Router clear error state
// This state handles the error condition when the router is not clear
// during the cutting sequence. Waits for user intervention or automatic recovery.

// Button-hold threshold separating a short press (return to previous state)
// from a long press (set cutToHomingFlag and go to homing).
namespace RouterClearErrorConfig {
    const unsigned long BUTTON_HOLD_THRESHOLD_MS = 1000;  // 1s hold = go to homing
}  // namespace RouterClearErrorConfig

void handleRouterClearErrorState() {
    // Router error state: handle user input for error recovery

    // Update inputs for button detection
    updateInputs();
    
    // Check start button state
    bool startButtonCurrentlyPressed = startButton.read();
    
    if (startButtonCurrentlyPressed && !startButtonWasPressed) {
        // Rising edge detected - button was just pressed
        startButtonWasPressed = true;
        stateStartTime = millis(); // Start timing the button hold
        
    } else if (startButtonCurrentlyPressed && startButtonWasPressed) {
        // Button is still being held - check for hold duration
        unsigned long holdDuration = millis() - stateStartTime;
        
        if (holdDuration >= RouterClearErrorConfig::BUTTON_HOLD_THRESHOLD_MS) { // long hold for homing
            // Long press detected - set flag to go to homing after cutting completes
            startButtonWasPressed = false;
            cutToHomingFlag = true;
            currentState = stateBeforeError;
            
            // If returning to cutting state, continue from where it left off
            if (stateBeforeError == STATE_CUTTING) {
                setCuttingPhaseToContinue();
            }
            return;
        }
        
    } else if (!startButtonCurrentlyPressed && startButtonWasPressed) {
        // Button was released - check if it was a short press
        unsigned long holdDuration = millis() - stateStartTime;
        
        if (holdDuration < RouterClearErrorConfig::BUTTON_HOLD_THRESHOLD_MS) { // Short press - return to previous state
            // Return to the state before the error occurred
            startButtonWasPressed = false;
            currentState = stateBeforeError;
            
            // If returning to cutting state, continue from where it left off
            if (stateBeforeError == STATE_CUTTING) {
                setCuttingPhaseToContinue();
            }
            return;
        }
        
        startButtonWasPressed = false;
    }
}
