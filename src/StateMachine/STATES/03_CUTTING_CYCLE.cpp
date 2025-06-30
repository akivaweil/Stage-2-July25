#include <Stage2_Machine.h>

//* ************************************************************************
//* ************************ CUTTING CYCLE STATE ************************
//* ************************************************************************
// This state performs the motor cutting sequence (clamps already engaged from alignment)

void handleCuttingCycleState() {
    static float currentPosition = 0;
    static int cutStep = 0;
    
    switch (cutStep) {
        case 0:
            //! ************************************************************************
            //! STEP 1: APPROACH MOVE - FAST SPEED TO CUTTING START POSITION
            //! ************************************************************************
            moveMotor(Motion::APPROACH_DISTANCE * Motion::STEPS_PER_INCH, Motion::APPROACH_SPEED, Motion::FORWARD_ACCEL);
            currentPosition = Motion::APPROACH_DISTANCE;
            cutStep++;
            break;
            
        case 1:
            //! ************************************************************************
            //! STEP 2: CUTTING MOVE - SLOW SPEED THROUGH MATERIAL
            //! ************************************************************************
            moveMotor(Motion::CUTTING_DISTANCE * Motion::STEPS_PER_INCH, Motion::CUTTING_SPEED, Motion::FORWARD_ACCEL);
            currentPosition += Motion::CUTTING_DISTANCE;
            cutStep++;
            break;
            
        case 2:
            //! ************************************************************************
            //! STEP 3: FINISH MOVE - FAST SPEED TO COMPLETE FORWARD TRAVEL
            //! ************************************************************************
            {
                float remainingDistance = Motion::TOTAL_FORWARD_DISTANCE - currentPosition;
                moveMotor(remainingDistance * Motion::STEPS_PER_INCH, Motion::FINISH_SPEED, Motion::FORWARD_ACCEL);
                cutStep++;
            }
            break;
            
        case 3:
            //! ************************************************************************
            //! STEP 4: PROCEED TO RETURNING STATE
            //! ************************************************************************
            // Reset static variables for next cycle
            currentPosition = 0;
            cutStep = 0;
            
            // Transition to returning state
            currentState = RETURNING;
            break;
    }
} 