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
            //! STEP 3: INITIAL LEFT CLAMP PULSE AND ALIGNMENT
            //! ************************************************************************
            // Step 3.1: Extend left clamp (secure material initially)
            extendLeftClamp();
            stepStartTime = millis();
            currentStep++;
            break;
            
        case 5:
            // Step 3.2: Wait 50ms (allow left clamp to fully extend)
            if (millis() - stepStartTime >= 50) {
                // Step 3.3: Extend alignment cylinder (position material precisely)
                extendAlignmentCylinder();
                stepStartTime = millis();
                currentStep++;
            }
            break;
            
        case 6:
            // Step 3.4: Wait 20ms (allow alignment movement to complete)
            if (millis() - stepStartTime >= 20) {
                // Step 3.5: Retract left clamp (release to allow fine adjustment)
                retractLeftClamp();
                stepStartTime = millis();
                currentStep++;
            }
            break;
            
        case 7:
            // Step 3.6: Wait 200ms (settle time)
            if (millis() - stepStartTime >= 200) {
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
            // Step 4.2: Wait 100ms, then retract alignment cylinder
            if (millis() - stepStartTime >= 100) {
                retractAlignmentCylinder();
                stepStartTime = millis();
                currentStep++;
            }
            break;
            
        case 10:
            // Step 4.3: Wait 150ms (right clamp extension time)
            if (millis() - stepStartTime >= 150) {
                // Step 4.4: Retract right clamp (release for repositioning)
                retractRightClamp();
                stepStartTime = millis();
                currentStep++;
            }
            break;
            
        case 11:
            // Step 4.5: Extend alignment cylinder (final positioning adjustment)
            extendAlignmentCylinder();
            stepStartTime = millis();
            currentStep++;
            break;
            
        case 12:
            // Step 4.6: Wait 150ms (alignment time)
            if (millis() - stepStartTime >= 150) {
                // Step 4.7: Retract alignment cylinder (clear for cutting)
                retractAlignmentCylinder();
                stepStartTime = millis();
                currentStep++;
            }
            break;
            
        case 13:
            // Step 4.8: Wait 200ms (settle time)
            if (millis() - stepStartTime >= 200) {
                currentStep++;
                stepStartTime = millis();
            }
            break;
            
        case 14:
            //! ************************************************************************
            //! STEP 5: FINAL CLAMP ENGAGEMENT FOR CUTTING
            //! ************************************************************************
            // Step 5.1: Extend left clamp (secure for cutting)
            extendLeftClamp();
            stepStartTime = millis();
            currentStep++;
            break;
            
        case 15:
            // Step 5.2: Extend right clamp (dual-clamp secure hold)
            extendRightClamp();
            stepStartTime = millis();
            currentStep++;
            break;
            
        case 16:
            // Wait for final clamp engagement before proceeding to cutting cycle
            if (millis() - stepStartTime >= 150) {
                // ADDED: Set current position to account for relative alignment moves
                float netAlignmentSteps = (Motion::ALIGNMENT_INITIAL_DISTANCE - Motion::ALIGNMENT_BACKWARD_DISTANCE) * Motion::STEPS_PER_INCH;
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