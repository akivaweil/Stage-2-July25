//* ************************************************************************
//* ************************ RETURN MOVE *******************************
//* ************************************************************************
// Returns the cutting motor to home position at high speed

#include <Stage2_Machine.h>

void state_RETURN_MOVE() {
    static bool moveStarted = false;
    static bool moveComplete = false;
    
    // First entry - start return move
    if (!moveStarted) {
        Serial.println("Starting return move");
        
        // Calculate return distance to home offset position
        float returnDistance = stepsToInches(currentPosition) - Motion::HOME_OFFSET;
        float returnSteps = inchesToSteps(returnDistance);
        
        // Set direction reverse (back towards home)
        setDirection(false);
        moveMotor(returnSteps, Motion::RETURN_SPEED, Motion::RETURN_ACCEL);
        
        moveStarted = true;
    }
    
    // Check if return move is complete
    if (millis() - stateStartTime >= 3000) { // Estimated time for return move
        if (!moveComplete) {
            // Update position back to home offset
            currentPosition = inchesToSteps(Motion::HOME_OFFSET);
            moveComplete = true;
            
            Serial.println("Return move complete");
            Serial.print("Returned to position: ");
            Serial.print(stepsToInches(currentPosition));
            Serial.println(" inches");
            
            moveStarted = false; // Reset for next cycle
            moveComplete = false; // Reset for next cycle
            
            //! ************************************************************************
            //! STEP 9: CYCLE COMPLETE
            //! ************************************************************************
            changeState(CYCLE_COMPLETE);
        }
    }
} 