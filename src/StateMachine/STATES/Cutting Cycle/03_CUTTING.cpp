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
    static float initialFinalPosition = 0;
    static float centerPosition = 0;          // Center position for oscillation
    static bool oscillatingForward = true;    // Direction of oscillation
    
    //! ************************************************************************
    //! CHECK FOR START BUTTON PRESS - INTERRUPT TO HOMING
    //! ************************************************************************
    // Check if start button is pressed during cutting - return to home
    if (checkStartButtonForHoming()) {
        // Reset static variables for next cycle
        stepStartTime = 0;
        cuttingPhase = 0;
        motionComplete = false;
        targetPosition = 0;
        initialFinalPosition = 0;
        centerPosition = 0;
        oscillatingForward = true;
        return; // Exit function, state will be changed to HOMING
    }
    
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
                
                // Store initial final position for verification distance calculation
                initialFinalPosition = currentPosition;
                
                cuttingPhase++;
                stepStartTime = millis();
            }
            break;
            
        //* ************************************************************************
        //* ************************ PHASE 4.5: POSITION VERIFICATION CHECK *******
        //* ************************************************************************
        
        case 6:
            //! ************************************************************************
            //! POSITION VERIFICATION: MOVE CONTINUOUSLY UNTIL SENSOR TRIGGERS
            //! ************************************************************************
            // Check if end position verification sensor is already triggered (active LOW)
            if (endPositionVerificationSensor.read()) {
                // End position verification sensor is already triggered, no movement needed
                positionVerificationDistance = 0.0; // No verification distance
                cuttingPhase = 8; // Skip to settle time phase
                stepStartTime = millis();
            } else {
                // Position verification sensor is not triggered, start continuous forward movement
                // Use a large target position to ensure continuous movement until sensor triggers
                targetPosition = currentPosition + (10.0 * Motion::STEPS_PER_INCH); // Move 10" forward (will be stopped by sensor)
                moveMotorToPosition(targetPosition, Motion::FINAL_SPEED, Motion::FORWARD_ACCEL);
                
                stepStartTime = millis();
                cuttingPhase++; // Go to next phase to monitor sensor
            }
            break;
            
        case 7:
            //! ************************************************************************
            //! MONITOR POSITION VERIFICATION SENSOR - STOP WHEN TRIGGERED
            //! ************************************************************************
            // Continuously check if end position verification sensor is triggered
            if (endPositionVerificationSensor.read()) {
                // Sensor triggered! Stop motor immediately and calculate verification distance
                stopMotor();
                currentPosition = getCurrentMotorPosition();
                positionVerificationDistance = currentPosition - initialFinalPosition;
                
                cuttingPhase = 8; // Proceed to settle time phase
                stepStartTime = millis();
            }
            // If sensor not triggered, keep moving (motor continues at set speed)
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
            //! ROUTER JAMMED SENSOR CHECK BEFORE CLAMP RELEASE
            //! ************************************************************************
            // Check router jammed sensor only when machine is at final position and ready to release clamps
            if (routerJammedSensor.read()) {
                // Router jammed detected - transition to error state
                currentState = ERROR;
                return; // Exit function immediately
            }
            
            //! ************************************************************************
            //! CLAMP RELEASE: RETRACT BOTH CLAMPS TEMPORARILY
            //! ************************************************************************
            // Retract both clamps temporarily to release material
            retractBothClamps();
            
            // Send high signal to pin 17 for duration of release
            digitalWrite(Pins::CLAMP_RELEASE_SIGNAL, HIGH);
            
            // Store current position as center for oscillation
            centerPosition = getCurrentMotorPosition();
            oscillatingForward = true;
            
            stepStartTime = millis();
            cuttingPhase++;
            break;
            
        case 10:
            //! ************************************************************************
            //! OSCILLATION DURING CLAMP RELEASE (WITHIN 400MS TOTAL TIME)
            //! ************************************************************************
            // Wait 100ms for clamps to fully retract, then start oscillation
            {
                unsigned long elapsedTime = millis() - stepStartTime;
            
            if (elapsedTime >= 100) { // Start oscillation after 100ms delay
                // Perform continuous oscillation movement until time expires
                if (!isMotorRunning()) {
                    // Calculate next oscillation position
                    float oscillationDistance = Timing::OSCILLATION_DISTANCE * Motion::STEPS_PER_INCH;
                    float targetOscPos;
                    
                    if (oscillatingForward) {
                        targetOscPos = centerPosition + oscillationDistance;
                    } else {
                        targetOscPos = centerPosition - oscillationDistance;
                    }
                    
                    // Move to oscillation position
                    moveMotorToPosition(targetOscPos, Timing::OSCILLATION_SPEED, Timing::OSCILLATION_ACCEL);
                    
                    // Toggle direction for next movement
                    oscillatingForward = !oscillatingForward;
                }
            }
            
            // Check if total clamp release time has elapsed
            if (elapsedTime >= Timing::CLAMP_RELEASE_TIME) {
                // Time's up - stop any oscillation and proceed
                stopMotor();
                cuttingPhase++;
                stepStartTime = millis();
            }
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
            centerPosition = 0;
            oscillatingForward = true;
            
            // Transition to returning state
            currentState = RETURNING;
            break;
    }
} 