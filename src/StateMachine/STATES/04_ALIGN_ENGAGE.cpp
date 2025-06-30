//* ************************************************************************
//* ************************ ALIGN ENGAGE ******************************
//* ************************************************************************
// Engages the alignment cylinder to position material correctly

#include <Stage2_Machine.h>

void state_ALIGN_ENGAGE() {
    static bool alignEngaged = false;
    
    // First entry - engage alignment cylinder
    if (!alignEngaged) {
        Serial.println("Engaging alignment cylinder");
        engageAlignCylinder();
        alignEngaged = true;
    }
    
    // Wait for alignment time
    if (millis() - stateStartTime >= Timing::ALIGNMENT_TIME) {
        Serial.println("Alignment complete - starting approach move");
        alignEngaged = false; // Reset for next cycle
        
        //! ************************************************************************
        //! STEP 3: APPROACH MOVE
        //! ************************************************************************
        changeState(APPROACH_MOVE);
    }
} 