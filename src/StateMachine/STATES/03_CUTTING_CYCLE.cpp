#include <Stage2_Machine.h>

//* ************************************************************************
//* ************************ CUTTING CYCLE STATE ************************
//* ************************************************************************
// This state performs the complete cutting sequence

void handleCuttingCycleState() {
    static float currentPosition = 0;
    
    //! ************************************************************************
    //! STEP 1: EXTEND CLAMPS TO SECURE WORKPIECE
    //! ************************************************************************
    extendBothClamps();
    delay(Motion::CLAMP_SETTLE_TIME);
    
    //! ************************************************************************
    //! STEP 2: APPROACH MOVE - FAST SPEED TO CUTTING START POSITION
    //! ************************************************************************
    moveMotorToPosition(Motion::APPROACH_DISTANCE, Motion::APPROACH_SPEED, Motion::FORWARD_ACCEL);
    currentPosition = Motion::APPROACH_DISTANCE;
    
    //! ************************************************************************
    //! STEP 3: CUTTING MOVE - SLOW SPEED THROUGH MATERIAL
    //! ************************************************************************
    moveMotorToPosition(currentPosition + Motion::CUTTING_DISTANCE, Motion::CUTTING_SPEED, Motion::FORWARD_ACCEL);
    currentPosition += Motion::CUTTING_DISTANCE;
    
    //! ************************************************************************
    //! STEP 4: FINISH MOVE - FAST SPEED TO COMPLETE FORWARD TRAVEL
    //! ************************************************************************
    float remainingDistance = Motion::TOTAL_FORWARD_DISTANCE - currentPosition;
    moveMotorToPosition(Motion::TOTAL_FORWARD_DISTANCE, Motion::FINISH_SPEED, Motion::FORWARD_ACCEL);
    
    //! ************************************************************************
    //! STEP 5: PROCEED TO RETURNING STATE
    //! ************************************************************************
    currentState = RETURNING;
} 