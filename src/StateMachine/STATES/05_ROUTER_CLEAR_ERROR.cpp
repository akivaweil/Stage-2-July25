#include "StateMachine/StateMachine.h"

// Router clear error state
// This state handles the error condition when the router is not clear
// during the cutting sequence. Waits for user intervention or automatic recovery.

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
        
        if (holdDuration >= 1000) { // 1 second hold for homing
            // Long press detected - set flag to go to homing after cutting completes
            startButtonWasPressed = false;
            cutToHomingFlag = true;
            Serial.println("Router Error: cutToHomingFlag set to TRUE");
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
        
        if (holdDuration < 1000) { // Short press - return to previous state
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
