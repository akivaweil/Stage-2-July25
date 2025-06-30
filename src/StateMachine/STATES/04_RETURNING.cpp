#include <Stage2_Machine.h>

//* ************************************************************************
//* ************************ RETURNING STATE *****************************
//* ************************************************************************
// This state returns the motor to home and releases all cylinders

void handleReturningState() {
    //! ************************************************************************
    //! STEP 1: RETRACT CLAMPS TO RELEASE WORKPIECE
    //! ************************************************************************
    retractBothClamps();
    
    //! ************************************************************************
    //! STEP 2: RETRACT ALIGNMENT CYLINDER
    //! ************************************************************************
    retractAlignmentCylinder();
    
    //! ************************************************************************
    //! STEP 3: RETURN MOTOR TO HOME POSITION AT HIGH SPEED
    //! ************************************************************************
    moveMotor(Motion::TOTAL_FORWARD_DISTANCE * Motion::STEPS_PER_INCH * -1, Motion::RETURN_SPEED, Motion::RETURN_ACCEL);
    
    //! ************************************************************************
    //! STEP 4: SIGNAL TRANSFER ARM THAT CYCLE IS COMPLETE
    //! ************************************************************************
    digitalWrite(Pins::TRANSFER_ARM_SIGNAL, HIGH);
    delay(100); // Brief signal pulse
    digitalWrite(Pins::TRANSFER_ARM_SIGNAL, LOW);
    
    //! ************************************************************************
    //! STEP 5: RETURN TO IDLE STATE
    //! ************************************************************************
    currentState = IDLE;
} 