#include <Stage2_Machine.h>

//* ************************************************************************
//* ************************ CUTTING STATE *******************************
//* ************************************************************************
// This state performs the complete cutting sequence: approach, cut, finish, temporarily release clamps, then re-extend for return

void handleCuttingState() {
    static unsigned long stepStartTime = 0;
    static int cuttingPhase = 0;
    static bool motionComplete = false;
    static float targetPosition = 0;
    
    // Initialize step timing
    if (stepStartTime == 0) {
        stepStartTime = millis();
    }
    
    switch (cuttingPhase) {
        //* ************************************************************************
        //* ************************ PHASE 2: APPROACH MOVEMENT ******************
        //* ************************************************************************
        
        case 0:
            //! ************************************************************************
            //! APPROACH: SET TARGET POSITION
            //! ************************************************************************
            // Set target position to approach position
            targetPosition = Motion::APPROACH_POSITION * Motion::STEPS_PER_INCH;
            moveMotorToPosition(targetPosition, Motion::APPROACH_SPEED, Motion::FORWARD_ACCEL);
            
            stepStartTime = millis();
            cuttingPhase++;
            break;
            
        case 1:
            //! ************************************************************************
            //! VERIFY APPROACH POSITION AND LOG COMPLETION
            //! ************************************************************************
            // Wait for motion to complete using FastAccelStepper status
            if (!isMotorRunning()) {
                // Update current position from stepper
                currentPosition = getCurrentMotorPosition();
                
                cuttingPhase++;
                stepStartTime = millis();
            }
            break;
            
        //* ************************************************************************
        //* ************************ PHASE 3: CUTTING MOVEMENT *******************
        //* ************************************************************************
        
        case 2:
            //! ************************************************************************
            //! CUTTING: CALCULATE TARGET
            //! ************************************************************************
            targetPosition = Motion::CUTTING_POSITION * Motion::STEPS_PER_INCH;
            
            // Execute slow, controlled cutting
            moveMotorToPosition(targetPosition, Motion::CUTTING_SPEED, Motion::FORWARD_ACCEL / 2);
            
            stepStartTime = millis();
            cuttingPhase++;
            break;
            
        case 3:
            //! ************************************************************************
            //! VERIFY CUTTING COMPLETION
            //! ************************************************************************
            // Wait for cutting motion to complete using FastAccelStepper status
            if (!isMotorRunning()) {
                // Update current position from stepper
                currentPosition = getCurrentMotorPosition();
                
                cuttingPhase++;
                stepStartTime = millis();
            }
            break;
            
        //* ************************************************************************
        //* ************************ PHASE 4: FINISH MOVEMENT ********************
        //* ************************************************************************
        
        case 4:
            //! ************************************************************************
            //! FINISH: SET FINAL TARGET POSITION
            //! ************************************************************************
            targetPosition = Motion::FINAL_POSITION * Motion::STEPS_PER_INCH;
            moveMotorToPosition(targetPosition, Motion::FINISH_SPEED, Motion::FORWARD_ACCEL);
            
            stepStartTime = millis();
            cuttingPhase++;
            break;
            
        case 5:
            //! ************************************************************************
            //! VERIFY FINISH COMPLETION
            //! ************************************************************************
            // Wait for finish motion to complete using FastAccelStepper status
            if (!isMotorRunning()) {
                // Update current position from stepper
                currentPosition = getCurrentMotorPosition();
                
                cuttingPhase++;
                stepStartTime = millis();
            }
            break;
            
        //* ************************************************************************
        //* ************************ PHASE 4.5: POSITION VERIFICATION CHECK *******
        //* ************************************************************************
        
        case 6:
            //! ************************************************************************
            //! CHECK POSITION VERIFICATION SENSOR - IF NOT TRIGGERED, MOVE UNTIL IT IS
            //! ************************************************************************
            // Check if position verification sensor is triggered (active LOW)
            if (!positionVerificationSensor.read()) {
                // Position verification sensor is not triggered, move forward at slow speed
                targetPosition = currentPosition + (0.1 * Motion::STEPS_PER_INCH); // Move 0.1" forward
                moveMotorToPosition(targetPosition, Motion::FINAL_SPEED, Motion::FORWARD_ACCEL);
                
                stepStartTime = millis();
                cuttingPhase++; // Go to next phase to wait for movement
            } else {
                // Position verification sensor is triggered, proceed to settle time
                cuttingPhase = 7; // Skip to settle time phase
                stepStartTime = millis();
            }
            break;
            
        case 7:
            //! ************************************************************************
            //! WAIT FOR POSITION VERIFICATION MOVEMENT COMPLETION
            //! ************************************************************************
            // Wait for position verification movement to complete
            if (!isMotorRunning()) {
                // Update current position from stepper
                currentPosition = getCurrentMotorPosition();
                
                // Check position verification sensor again
                if (!positionVerificationSensor.read()) {
                    // Still not triggered, go back to move again
                    cuttingPhase = 6;
                } else {
                    // Now triggered, proceed to settle time
                    cuttingPhase = 8;
                }
                stepStartTime = millis();
            }
            break;
            
        //* ************************************************************************
        //* ************************ PHASE 5: SETTLE TIME *************************
        //* ************************************************************************
        
        case 8:
            //! ************************************************************************
            //! SETTLE TIME: WAIT 150MS
            //! ************************************************************************
            // Wait for settle time to ensure motion has fully stabilized
            if (millis() - stepStartTime >= Timing::CLAMP_SETTLE_TIME) {
                cuttingPhase++;
                stepStartTime = millis();
            }
            break;
            
        //* ************************************************************************
        //* ************************ PHASE 6: CLAMP RELEASE **********************
        //* ************************************************************************
        
        case 9:
            //! ************************************************************************
            //! CLAMP RELEASE: RETRACT BOTH CLAMPS TEMPORARILY
            //! ************************************************************************
            // Retract both clamps temporarily to release material
            retractBothClamps();
            
            // Send high signal to pin 17 for duration of release
            digitalWrite(Pins::CLAMP_RELEASE_SIGNAL, HIGH);
            
            stepStartTime = millis();
            cuttingPhase++;
            break;
            
        case 10:
            //! ************************************************************************
            //! WAIT FOR CLAMP RELEASE COMPLETION
            //! ************************************************************************
            // Wait for clamp release time (ensure full release)
            if (millis() - stepStartTime >= Timing::CLAMP_RELEASE_TIME) {
                cuttingPhase++;
                stepStartTime = millis();
            }
            break;
            
        //* ************************************************************************
        //* ************************ PHASE 7: RE-EXTEND CLAMPS *******************
        //* ************************************************************************
        
        case 11:
            //! ************************************************************************
            //! RE-EXTEND CLAMPS FOR RETURN JOURNEY
            //! ************************************************************************
            // Extend both clamps again for the return journey
            extendBothClamps();
            
            stepStartTime = millis();
            cuttingPhase++;
            break;
            
        case 12:
            //! ************************************************************************
            //! WAIT FOR CLAMP RE-EXTENSION
            //! ************************************************************************
            // Wait for clamps to fully extend before return
            if (millis() - stepStartTime >= 0) { //skip settle time
                cuttingPhase++;
                stepStartTime = millis();
            }
            break;
            
        //* ************************************************************************
        //* ************************ PHASE 8: PREPARE FOR RETURN *****************
        //* ************************************************************************
        
        case 13:
            //! ************************************************************************
            //! SIGNAL TRANSFER ARM AND TRANSITION TO RETURNING STATE
            //! ************************************************************************
            // Signal transfer arm: Set HIGH (prevent Z-axis interference)
            digitalWrite(Pins::TRANSFER_ARM_SIGNAL, HIGH);
            
            // Bring pin 17 low before transitioning to returning state
            digitalWrite(Pins::CLAMP_RELEASE_SIGNAL, LOW);
            
            // Reset static variables for next cycle
            stepStartTime = 0;
            cuttingPhase = 0;
            motionComplete = false;
            targetPosition = 0;
            
            // Transition to returning state
            currentState = RETURNING;
            break;
    }
} 