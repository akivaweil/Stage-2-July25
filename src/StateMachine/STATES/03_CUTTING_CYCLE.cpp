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
    delay(Timing::CLAMP_SETTLE_TIME);
    
    //! ************************************************************************
    //! STEP 2: APPROACH MOVE - FAST SPEED TO CUTTING START POSITION
    //! ************************************************************************
    moveMotorToPosition(Motion::APPROACH_DISTANCE * Motion::STEPS_PER_INCH, Motion::APPROACH_SPEED, Motion::FORWARD_ACCEL);
    currentPosition = Motion::APPROACH_DISTANCE * Motion::STEPS_PER_INCH;
    
    //! ************************************************************************
    //! STEP 3: CUTTING MOVE - SLOW SPEED THROUGH MATERIAL
    //! ************************************************************************
    float cuttingTarget = currentPosition + (Motion::CUTTING_DISTANCE * Motion::STEPS_PER_INCH);
    moveMotorToPosition(cuttingTarget, Motion::CUTTING_SPEED, Motion::FORWARD_ACCEL);
    currentPosition = cuttingTarget;
    
    //! ************************************************************************
    //! STEP 4: FINISH MOVE - FAST SPEED TO COMPLETE FORWARD TRAVEL
    //! ************************************************************************
    float finishTarget = Motion::TOTAL_FORWARD_DISTANCE * Motion::STEPS_PER_INCH;
    moveMotorToPosition(finishTarget, Motion::FINISH_SPEED, Motion::FORWARD_ACCEL);
    
    //! ************************************************************************
    //! STEP 5: PROCEED TO RETURNING STATE
    //! ************************************************************************
    currentState = RETURNING;
} 