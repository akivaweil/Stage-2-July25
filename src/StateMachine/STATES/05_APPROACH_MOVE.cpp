//* ************************************************************************
//* ************************ APPROACH MOVE *****************************
//* ************************************************************************
// Moves the cutting motor at high speed to approach the cutting position

#include <Stage2_Machine.h>

void state_APPROACH_MOVE() {
    static bool moveStarted = false;
    static bool moveComplete = false;
    
    // First entry - start approach move
    if (!moveStarted) {
        Serial.println("Starting approach move");
        
        // Calculate approach distance in steps
        float approachSteps = inchesToSteps(Motion::APPROACH_DISTANCE);
        
        // Set direction forward and start move
        setDirection(true);
        moveMotor(approachSteps, Motion::APPROACH_SPEED, Motion::FORWARD_ACCEL);
        
        moveStarted = true;
    }
    
    // Check if move is complete (simplified - in real implementation would check motor controller)
    if (millis() - stateStartTime >= 2000) { // Estimated time for approach move
        if (!moveComplete) {
            // Update position
            currentPosition += inchesToSteps(Motion::APPROACH_DISTANCE);
            moveComplete = true;
            
            Serial.println("Approach move complete - starting cutting move");
            Serial.print("Current position: ");
            Serial.print(stepsToInches(currentPosition));
            Serial.println(" inches");
            
            moveStarted = false; // Reset for next cycle
            moveComplete = false; // Reset for next cycle
            
            //! ************************************************************************
            //! STEP 4: CUTTING MOVE
            //! ************************************************************************
            changeState(CUTTING_MOVE);
        }
    }
} 