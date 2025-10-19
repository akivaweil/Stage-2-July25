#include <Stage2_Machine.h>
#include <Config.h>

//* ************************************************************************
//* ************************ ALIGNMENT STATE ****************************
//* ************************************************************************
// This state performs the complete clamp sequence with detailed material alignment choreography

void handleAlignmentState() {
    static unsigned long stepStartTime = 0;
    static int currentStep = 0;
    
    // Initialize step timing
    if (stepStartTime == 0) {
        stepStartTime = millis();
    }
    
    switch (currentStep) {
        case 0:
            //! ************************************************************************
            //! STEP 1: INITIAL FORWARD MOVEMENT
            //! ************************************************************************
            // Move motor forward 1 inch at a specific speed for initial alignment
            stepper->setSpeedInHz(Motion::ALIGNMENT_INITIAL_SPEED);
            stepper->move(Motion::ALIGNMENT_INITIAL_DISTANCE * Motion::STEPS_PER_INCH);
            stepStartTime = millis();
            currentStep++;
            break;
            
        case 1:
            // Wait for the initial movement to complete
            if (!stepper->isRunning()) {
                stepStartTime = millis();
                currentStep++;
            }
            break;

        case 2:
            //! ************************************************************************
            //! STEP 2: REVERSE A BIT TO MOVE OUT OF THE WAY
            //! ************************************************************************
            // Move motor backward 0.1 inch to allow material to settle against clamps
            stepper->setSpeedInHz(Motion::ALIGNMENT_INITIAL_SPEED); // Assuming same speed is okay
            stepper->move(-Motion::ALIGNMENT_BACKWARD_DISTANCE * Motion::STEPS_PER_INCH);
            stepStartTime = millis();
            currentStep++;
            break;
            
        case 3:
            // Wait for the reverse movement to complete
            if (!stepper->isRunning()) {
                stepStartTime = millis();
                currentStep++;
            }
            break;

        //* ************************************************************************
        //* ************************ PHASE 1: CLAMP SEQUENCE (MATERIAL ALIGNMENT) *
        //* ************************************************************************
        
        case 4:
            //! ************************************************************************
            //! STEP 3: ALIGNMENT CYLINDER FIRST, THEN LEFT CLAMP SEQUENCE
            //! ************************************************************************
            // Step 3.1: Extend alignment cylinder first (position material precisely)
            extendAlignmentCylinder();
            stepStartTime = millis();
            currentStep++;
            break;
            
        case 5:
            // Step 3.2: Wait for alignment cylinder to position material
            if (millis() - stepStartTime >= Timing::ALIGNMENT_CYLINDER_PRE_EXTEND_MS) {
                // Step 3.3: Extend left clamp (secure material after alignment)
                extendLeftClamp();
                stepStartTime = millis();
                currentStep++;
            }
            break;
            
        case 6:
            // Step 3.4: Wait (allow left clamp to fully extend)
            if (millis() - stepStartTime >= Timing::ALIGNMENT_LEFT_CLAMP_EXTEND_MS) {
                // Step 3.5: Retract left clamp (release to allow fine adjustment)
                retractLeftClamp();
                stepStartTime = millis();
                currentStep++;
            }
            break;
            
        case 7:
            // Step 3.6: Wait (settle time)
            if (millis() - stepStartTime >= Timing::ALIGNMENT_LONG_SETTLE_MS) {
                currentStep++;
                stepStartTime = millis();
            }
            break;
            
        case 8:
            //! ************************************************************************
            //! STEP 4: RIGHT CLAMP SEQUENCE WITH ALIGNMENT
            //! ************************************************************************
            // Step 4.1: Extend right clamp first
            extendRightClamp();
            stepStartTime = millis();
            currentStep++;
            break;
            
        case 9:
            // Step 4.2: Wait, then retract alignment cylinder
            if (millis() - stepStartTime >= Timing::ALIGNMENT_SHORT_SETTLE_MS) {
                retractAlignmentCylinder();
                stepStartTime = millis();
                currentStep++;
            }
            break;
            
        case 10:
            // Step 4.3: Wait (right clamp extension time)
            if (millis() - stepStartTime >= Timing::ALIGNMENT_RIGHT_CLAMP_WAIT_MS) {
                // Step 4.4: Retract right clamp (release for repositioning)
                retractRightClamp();
                stepStartTime = millis();
                currentStep++;
            }
            break;
            
        case 11:
            // Step 4.5: Extend alignment cylinder and right clamp (prep for final positioning)
            extendAlignmentCylinder();
            extendRightClamp();
            stepStartTime = millis();
            currentStep++;
            break;
            
        case 12:
            // Step 4.6: Wait, then retract alignment cylinder (clear for cutting)
            if (millis() - stepStartTime >= Timing::ALIGNMENT_SHORT_SETTLE_MS) {
                retractAlignmentCylinder();
                stepStartTime = millis();
                currentStep++;
            }
            break;
            
        case 13:
            // Step 4.7: Wait (settle time after alignment cylinder retraction)
            if (millis() - stepStartTime >= Timing::ALIGNMENT_LONG_SETTLE_MS) {
                // Step 4.8: Final backward movement of 0.3 inches after all clamp movements
                stepper->setSpeedInHz(Motion::ALIGNMENT_INITIAL_SPEED);
                stepper->move(-0.3 * Motion::STEPS_PER_INCH);
                stepStartTime = millis();
                currentStep++;
            }
            break;
            
        case 14:
            // Step 4.9: Wait for final backward movement to complete
            if (!stepper->isRunning()) {
                currentStep++;
                stepStartTime = millis();
            }
            break;
            
        case 15:
            //! ************************************************************************
            //! STEP 5: FINAL CLAMP ENGAGEMENT FOR CUTTING
            //! ************************************************************************
            // Step 5.1: Extend left clamp (secure for cutting)
            extendLeftClamp();
            stepStartTime = millis();
            currentStep++;
            break;
            
        case 16:
            // Step 5.2: Extend right clamp (dual-clamp secure hold)
            extendRightClamp();
            stepStartTime = millis();
            currentStep++;
            break;
            
        case 17:
            // Step 5.3: Wait for final clamp engagement before proceeding to cutting cycle
            if (millis() - stepStartTime >= Timing::CLAMP_SETTLE_TIME) {
                // ADDED: Set current position to account for relative alignment moves
                float netAlignmentSteps = (Motion::ALIGNMENT_INITIAL_DISTANCE - Motion::ALIGNMENT_BACKWARD_DISTANCE - 0.3) * Motion::STEPS_PER_INCH;
                setCurrentMotorPosition((long)netAlignmentSteps);

                // Reset static variables for next cycle
                stepStartTime = 0;
                currentStep = 0;
                
                // Transition to cutting state
                currentState = CUTTING;
            }
            break;
    }
} 