//* ************************************************************************
//* ************************ FINISH MOVE *******************************
//* ************************************************************************
// Completes the forward cutting motion

#include <Stage2_Machine.h>

void state_FINISH_MOVE() {
    static bool moveStarted = false;
    static bool moveComplete = false;
    
    // First entry - start finish move
    if (!moveStarted) {
        Serial.println("Starting finish move");
        
        // Calculate remaining distance to complete total forward distance
        float currentDistanceInches = stepsToInches(currentPosition);
        float remainingDistance = Motion::FORWARD_DISTANCE - currentDistanceInches + Motion::HOME_OFFSET;
        float finishSteps = inchesToSteps(remainingDistance);
        
        // Continue forward direction at finish speed
        setDirection(true);
        moveMotor(finishSteps, Motion::FINISH_SPEED, Motion::FORWARD_ACCEL);
        
        moveStarted = true;
    }
    
    // Check if finish move is complete
    if (millis() - stateStartTime >= 2000) { // Estimated time for finish move
        if (!moveComplete) {
            // Update position to final forward position
            currentPosition = inchesToSteps(Motion::FORWARD_DISTANCE + Motion::HOME_OFFSET);
            moveComplete = true;
            
            Serial.println("Finish move complete - releasing clamps");
            Serial.print("Final forward position: ");
            Serial.print(stepsToInches(currentPosition));
            Serial.println(" inches");
            
            moveStarted = false; // Reset for next cycle
            moveComplete = false; // Reset for next cycle
            
            //! ************************************************************************
            //! STEP 6: RELEASE CLAMPS
            //! ************************************************************************
            changeState(CLAMP_RELEASE);
        }
    }
} 