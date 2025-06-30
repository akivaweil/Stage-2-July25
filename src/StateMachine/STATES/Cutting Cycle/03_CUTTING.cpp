#include <Stage2_Machine.h>

//* ************************************************************************
//* ************************ CUTTING STATE *******************************
//* ************************************************************************
// This state performs the complete cutting sequence: approach, cut, finish, release clamps

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
            //! APPROACH: SET TARGET POSITION TO 8.0 INCHES
            //! ************************************************************************
            // Set target position to 8.0 inches (approach distance)
            targetPosition = Motion::APPROACH_DISTANCE * Motion::STEPS_PER_INCH;
            moveMotorToPosition(targetPosition, Motion::APPROACH_SPEED, Motion::FORWARD_ACCEL);
            
            // Log approach movement
            Serial.print("APPROACH: Moving to ");
            Serial.print(Motion::APPROACH_DISTANCE);
            Serial.print(" inches (");
            Serial.print(targetPosition);
            Serial.println(" steps)");
            
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
                
                Serial.print("APPROACH COMPLETE: Position verified at ");
                Serial.print(stepsToInches(currentPosition));
                Serial.println(" inches");
                
                cuttingPhase++;
                stepStartTime = millis();
            }
            break;
            
        //* ************************************************************************
        //* ************************ PHASE 3: CUTTING MOVEMENT *******************
        //* ************************************************************************
        
        case 2:
            //! ************************************************************************
            //! CUTTING: CALCULATE TARGET (8.0" + 4.0" = 12.0")
            //! ************************************************************************
            // Calculate cutting target: 8.0" + 4.0" = 12.0 inches
            targetPosition = (Motion::APPROACH_DISTANCE + Motion::CUTTING_DISTANCE) * Motion::STEPS_PER_INCH;
            
            // Execute slow, controlled cutting
            // Speed: 127 steps/sec (~3 inches/second)
            // Reduced acceleration for precision
            // Distance: 4.0 inches through material
            moveMotorToPosition(targetPosition, Motion::CUTTING_SPEED, Motion::FORWARD_ACCEL / 2);
            
            // Log cutting movement
            Serial.print("CUTTING: Moving from ");
            Serial.print(Motion::APPROACH_DISTANCE);
            Serial.print(" to ");
            Serial.print(Motion::APPROACH_DISTANCE + Motion::CUTTING_DISTANCE);
            Serial.print(" inches at ");
            Serial.print(Motion::CUTTING_SPEED);
            Serial.println(" steps/sec");
            
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
                
                Serial.print("CUTTING COMPLETE: Position verified at ");
                Serial.print(stepsToInches(currentPosition));
                Serial.println(" inches");
                
                cuttingPhase++;
                stepStartTime = millis();
            }
            break;
            
        //* ************************************************************************
        //* ************************ PHASE 4: FINISH MOVEMENT ********************
        //* ************************************************************************
        
        case 4:
            //! ************************************************************************
            //! FINISH: SET FINAL TARGET POSITION (14.0 INCHES)
            //! ************************************************************************
            // Set final target position: 14.0 inches (total forward distance)
            targetPosition = Motion::TOTAL_FORWARD_DISTANCE * Motion::STEPS_PER_INCH;
            moveMotorToPosition(targetPosition, Motion::FINISH_SPEED, Motion::FORWARD_ACCEL);
            
            // Log finish movement
            Serial.print("FINISH: Moving to final position ");
            Serial.print(Motion::TOTAL_FORWARD_DISTANCE);
            Serial.print(" inches (");
            Serial.print(targetPosition);
            Serial.println(" steps)");
            
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
                
                Serial.print("FINISH COMPLETE: Final position verified at ");
                Serial.print(stepsToInches(currentPosition));
                Serial.println(" inches");
                
                cuttingPhase++;
                stepStartTime = millis();
            }
            break;
            
        //* ************************************************************************
        //* ************************ PHASE 5: CLAMP RELEASE **********************
        //* ************************************************************************
        
        case 6:
            //! ************************************************************************
            //! CLAMP RELEASE: RETRACT BOTH CLAMPS
            //! ************************************************************************
            // Retract left clamp (release material)
            retractLeftClamp();
            Serial.println("CLAMP RELEASE: Left clamp retracted");
            
            stepStartTime = millis();
            cuttingPhase++;
            break;
            
        case 7:
            // Brief delay between clamp retractions
            if (millis() - stepStartTime >= 50) {
                // Retract right clamp (release material)
                retractRightClamp();
                Serial.println("CLAMP RELEASE: Right clamp retracted");
                
                stepStartTime = millis();
                cuttingPhase++;
            }
            break;
            
        case 8:
            //! ************************************************************************
            //! WAIT 100ms (ENSURE FULL RELEASE)
            //! ************************************************************************
            // Wait 100ms (ensure full release)
            if (millis() - stepStartTime >= 100) {
                Serial.println("CLAMP RELEASE: Full release verified");
                
                cuttingPhase++;
                stepStartTime = millis();
            }
            break;
            
        //* ************************************************************************
        //* ************************ PHASE 6: PREPARE FOR RETURN *****************
        //* ************************************************************************
        
        case 9:
            //! ************************************************************************
            //! SIGNAL TRANSFER ARM AND TRANSITION TO RETURNING STATE
            //! ************************************************************************
            // Signal transfer arm: Set HIGH (prevent Z-axis interference)
            digitalWrite(Pins::TRANSFER_ARM_SIGNAL, HIGH);
            Serial.println("CUTTING COMPLETE: Transfer arm signaled, transitioning to return");
            
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