#include <Stage2_Machine.h>

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
        //* ************************************************************************
        //* ************************ PHASE 1: CLAMP SEQUENCE (MATERIAL ALIGNMENT) *
        //* ************************************************************************
        
        case 0:
            //! ************************************************************************
            //! STEP 1: INITIAL LEFT CLAMP PULSE AND ALIGNMENT
            //! ************************************************************************
            // Step 1.1: Extend left clamp (secure material initially)
            extendLeftClamp();
            stepStartTime = millis();
            currentStep++;
            break;
            
        case 1:
            // Step 1.2: Wait 150ms (allow clamp to engage)
            if (millis() - stepStartTime >= 50) {
                // Step 1.3: Extend alignment cylinder (position material precisely)
                extendAlignmentCylinder();
                stepStartTime = millis();
                currentStep++;
            }
            break;
            
        case 2:
            // Step 1.4: Wait 150ms (allow alignment to complete)
            if (millis() - stepStartTime >= 20) {
                // Step 1.5: Retract left clamp (release to allow fine adjustment)
                retractLeftClamp();
                stepStartTime = millis();
                currentStep++;
            }
            break;
            
        case 3:
            // Step 1.6: Wait 150ms (settle time)
            if (millis() - stepStartTime >= 150) {
                currentStep++;
                stepStartTime = millis();
            }
            break;
            
        case 4:
            //! ************************************************************************
            //! STEP 2: RIGHT CLAMP SEQUENCE WITH ALIGNMENT
            //! ************************************************************************
            // Step 2.1: Retract alignment cylinder (clear the way)
            retractAlignmentCylinder();
            stepStartTime = millis();
            currentStep++;
            break;
            
        case 5:
            // Step 2.2: Extend right clamp (secure other end of material)
            extendRightClamp();
            stepStartTime = millis();
            currentStep++;
            break;
            
        case 6:
            // Step 2.3: Wait 150ms (clamp engagement time)
            if (millis() - stepStartTime >= 150) {
                // Step 2.4: Retract right clamp (release for repositioning)
                retractRightClamp();
                stepStartTime = millis();
                currentStep++;
            }
            break;
            
        case 7:
            // Step 2.5: Extend alignment cylinder (final positioning adjustment)
            extendAlignmentCylinder();
            stepStartTime = millis();
            currentStep++;
            break;
            
        case 8:
            // Step 2.6: Wait 150ms (alignment time)
            if (millis() - stepStartTime >= 150) {
                // Step 2.7: Retract alignment cylinder (clear for cutting)
                retractAlignmentCylinder();
                stepStartTime = millis();
                currentStep++;
            }
            break;
            
        case 9:
            // Step 2.8: Wait 200ms (settle time)
            if (millis() - stepStartTime >= 200) {
                currentStep++;
                stepStartTime = millis();
            }
            break;
            
        case 10:
            //! ************************************************************************
            //! STEP 3: FINAL CLAMP ENGAGEMENT FOR CUTTING
            //! ************************************************************************
            // Step 3.1: Extend left clamp (secure for cutting)
            extendLeftClamp();
            stepStartTime = millis();
            currentStep++;
            break;
            
        case 11:
            // Step 3.2: Extend right clamp (dual-clamp secure hold)
            extendRightClamp();
            stepStartTime = millis();
            currentStep++;
            break;
            
        case 12:
            // Wait for final clamp engagement before proceeding to cutting cycle
            if (millis() - stepStartTime >= 150) {
                // Reset static variables for next cycle
                stepStartTime = 0;
                currentStep = 0;
                
                // Transition to cutting state
                currentState = CUTTING;
            }
            break;
    }
} 