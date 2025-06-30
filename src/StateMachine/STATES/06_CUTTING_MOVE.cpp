//* ************************************************************************
//* ************************ CUTTING MOVE ******************************
//* ************************************************************************
// Performs the actual cutting operation at slow, precise speed

#include <Stage2_Machine.h>

void state_CUTTING_MOVE() {
    static bool moveStarted = false;
    static bool moveComplete = false;
    
    // First entry - start cutting move
    if (!moveStarted) {
        Serial.println("Starting cutting move");
        
        // Calculate cutting distance in steps
        float cuttingSteps = inchesToSteps(Motion::CUTTING_DISTANCE);
        
        // Continue forward direction at cutting speed
        setDirection(true);
        moveMotor(cuttingSteps, Motion::CUTTING_SPEED, Motion::FORWARD_ACCEL);
        
        moveStarted = true;
    }
    
    // Check if cutting move is complete (estimated time for slow cutting)
    if (millis() - stateStartTime >= 8000) { // Estimated time for cutting move
        if (!moveComplete) {
            // Update position
            currentPosition += inchesToSteps(Motion::CUTTING_DISTANCE);
            moveComplete = true;
            
            Serial.println("Cutting move complete - starting finish move");
            Serial.print("Current position: ");
            Serial.print(stepsToInches(currentPosition));
            Serial.println(" inches");
            
            moveStarted = false; // Reset for next cycle
            moveComplete = false; // Reset for next cycle
            
            //! ************************************************************************
            //! STEP 5: FINISH MOVE
            //! ************************************************************************
            changeState(FINISH_MOVE);
        }
    }
} 