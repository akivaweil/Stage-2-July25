#include <Stage2_Machine.h>

//* ************************************************************************
//* ************************ ROUTER CLEAR ERROR STATE ********************
//* ************************************************************************
// This state handles the error condition when the router is not clear
// during the cutting sequence. Waits for user intervention or automatic recovery.

void handleRouterClearErrorState() {
    //! ************************************************************************
    //! ROUTER ERROR STATE: HANDLE USER INPUT FOR ERROR RECOVERY
    //! ************************************************************************
    
    // Update inputs for button detection
    updateInputs();
    
    // Check start button and transfer arm signal states
    bool startButtonCurrentlyPressed = startButton.read();
    bool transferArmCurrentlyActive = transferArmSignal.read();
    
    // Handle start button or transfer arm signal input
    if ((startButtonCurrentlyPressed && !startButtonWasPressed) || 
        (transferArmCurrentlyActive && !transferArmSignalWasActive)) {
        // Rising edge detected - button was just pressed or signal activated
        startButtonWasPressed = startButtonCurrentlyPressed;
        transferArmSignalWasActive = transferArmCurrentlyActive;
        stateStartTime = millis(); // Start timing the input hold
        
    } else if ((startButtonCurrentlyPressed && startButtonWasPressed) || 
               (transferArmCurrentlyActive && transferArmSignalWasActive)) {
        // Input is still being held - check for hold duration
        unsigned long holdDuration = millis() - stateStartTime;
        
        if (holdDuration >= 1000) { // 1 second hold for homing
            // Long press detected - set flag to go to homing after cutting completes
            startButtonWasPressed = false;
            transferArmSignalWasActive = false;
            cutToHomingFlag = true;
            Serial.println("Router Error: cutToHomingFlag set to TRUE");
            currentState = stateBeforeError;
            
            // If returning to cutting state, continue from where it left off
            if (stateBeforeError == CUTTING) {
                setCuttingPhaseToContinue();
            }
            return;
        }
        
    } else if ((!startButtonCurrentlyPressed && startButtonWasPressed) || 
               (!transferArmCurrentlyActive && transferArmSignalWasActive)) {
        // Input was released - check if it was a short press
        unsigned long holdDuration = millis() - stateStartTime;
        
        if (holdDuration < 1000) { // Short press - return to previous state
            // Return to the state before the error occurred
            startButtonWasPressed = false;
            transferArmSignalWasActive = false;
            currentState = stateBeforeError;
            
            // If returning to cutting state, continue from where it left off
            if (stateBeforeError == CUTTING) {
                setCuttingPhaseToContinue();
            }
            return;
        }
        
        startButtonWasPressed = false;
        transferArmSignalWasActive = false;
    }
}
