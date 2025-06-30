//* ************************************************************************
//* ************************ CUTTING CYCLE *****************************
//* ************************************************************************
// Initializes the cutting cycle and moves to first step

#include <Stage2_Machine.h>

void state_CUTTING_CYCLE() {
    // Set cycle in progress flag
    cycleInProgress = true;
    
    // Enable motor for cutting operations
    enableMotor();
    
    Serial.println("Starting cutting cycle");
    
    //! ************************************************************************
    //! STEP 1: ENGAGE CLAMPS
    //! ************************************************************************
    changeState(CLAMP_ENGAGE);
} 