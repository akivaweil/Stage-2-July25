//* ************************************************************************
//* ************************ ALIGN RELEASE *****************************
//* ************************************************************************
// Releases the alignment cylinder to allow material movement

#include <Stage2_Machine.h>

void state_ALIGN_RELEASE() {
    static bool alignReleased = false;
    
    // First entry - release alignment cylinder
    if (!alignReleased) {
        Serial.println("Releasing alignment cylinder");
        releaseAlignCylinder();
        alignReleased = true;
    }
    
    // Wait for alignment release time
    if (millis() - stateStartTime >= Timing::ALIGNMENT_TIME) {
        Serial.println("Alignment released - starting return move");
        alignReleased = false; // Reset for next cycle
        
        //! ************************************************************************
        //! STEP 8: RETURN MOVE
        //! ************************************************************************
        changeState(RETURN_MOVE);
    }
} 