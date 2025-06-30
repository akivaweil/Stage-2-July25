//* ************************************************************************
//* ************************ CLAMP RELEASE *****************************
//* ************************************************************************
// Releases both left and right clamps to free the cut material

#include <Stage2_Machine.h>

void state_CLAMP_RELEASE() {
    static bool clampsReleased = false;
    
    // First entry - release clamps
    if (!clampsReleased) {
        Serial.println("Releasing clamps");
        releaseClamps();
        clampsReleased = true;
    }
    
    // Wait for clamp release time
    if (millis() - stateStartTime >= Timing::CLAMP_RELEASE_TIME) {
        Serial.println("Clamps released - releasing alignment");
        clampsReleased = false; // Reset for next cycle
        
        //! ************************************************************************
        //! STEP 7: RELEASE ALIGNMENT CYLINDER
        //! ************************************************************************
        changeState(ALIGN_RELEASE);
    }
} 