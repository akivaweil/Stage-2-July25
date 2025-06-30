//* ************************************************************************
//* ************************ CLAMP ENGAGE ******************************
//* ************************************************************************
// Engages both left and right clamps to secure the material

#include <Stage2_Machine.h>

void state_CLAMP_ENGAGE() {
    static bool clampsEngaged = false;
    
    // First entry - engage clamps
    if (!clampsEngaged) {
        Serial.println("Engaging clamps");
        engageClamps();
        clampsEngaged = true;
    }
    
    // Wait for clamp engagement time
    if (millis() - stateStartTime >= Timing::CLAMP_ENGAGE_TIME) {
        Serial.println("Clamps engaged - proceeding to alignment");
        clampsEngaged = false; // Reset for next cycle
        
        //! ************************************************************************
        //! STEP 2: ENGAGE ALIGNMENT CYLINDER
        //! ************************************************************************
        changeState(ALIGN_ENGAGE);
    }
} 