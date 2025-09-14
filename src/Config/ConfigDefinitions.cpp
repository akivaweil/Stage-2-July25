#include <Config.h>

// Board identification
namespace Config {
const char *BOARD_ID = "STAGE2_001";
const char *BOARD_DESCRIPTION = "Stage 2 Cutting Machine - USB Only";
}  // namespace Config

// Motion Parameters - All positions are absolute relative to home (position 0)
namespace Motion {
const float STEPS_PER_INCH = 42.33;   // 200 steps/rev ÷ (60 teeth × 2mm ÷ 25.4mm/inch) = 42.33 steps/inch

// Absolute positions relative to home (position 0)
const float HOME_POSITION = 0.0;                    // Home position (where switch is triggered)
const float HOME_OFFSET_POSITION = 0.01;             // Position after homing (0.5" from home switch)
const float ALIGNMENT_INITIAL_POSITION =0.5;       // Position after initial alignment move (0.5" + 0.5")
const float ALIGNMENT_BACKWARD_POSITION = 0.3;      // Position after backward alignment move (1.0" - 0.3")
const float ALIGNMENT_FINAL_POSITION = 0.4;         // Position after final alignment backward move (0.7" - 0.3")
const float APPROACH_POSITION = 5.0;               // Position for cutting approach (0.4" + 4.15" + 0.1" net alignment)
const float CUTTING_POSITION = 12.0;               // Position after cutting (4.65" + 6.4")
const float FINAL_POSITION = 24.0;                 // Final position after finish move (0.4" + 24.15")

// Speed Settings (steps/second)
const float HOMING_SPEED = 150;     
const float ALIGNMENT_INITIAL_SPEED = 150; // Initial speed for alignment
const float APPROACH_SPEED = 30000; 
const float CUTTING_SPEED = 75;    
const float FINISH_SPEED = 30000;   
const float RETURN_SPEED = 30000;   
const float FINAL_SPEED = 50;   

// Acceleration Settings (steps/second²)
const float FORWARD_ACCEL = 10000;   
const float RETURN_ACCEL = 16000;    
}  // namespace Motion

// Timing Settings (milliseconds)
namespace Timing {
const int CLAMP_SETTLE_TIME = 50;
const int CLAMP_RELEASE_TIME = 350;
const int HOME_SETTLE_TIME = 100;
const int MOTION_SETTLE_TIME = 50;
const int ALIGN_SETTLE_TIME = 200;
const int SIGNAL_DURATION = 2000;
const int MOTOR_TIMEOUT = 5000;
const int START_BUTTON_COOLDOWN = 50;  // 50ms cooldown to prevent immediate re-triggering
// Alignment-specific timing (ms)
const int ALIGNMENT_LEFT_CLAMP_EXTEND_MS = 50;   // allow left clamp to extend
const int ALIGNMENT_ALIGNMENT_MOVE_MS = 270;      // allow alignment movement to complete
const int ALIGNMENT_RIGHT_CLAMP_WAIT_MS = 150;   // right clamp extension time
const int ALIGNMENT_SHORT_SETTLE_MS = 100;       // short settle/pause
const int ALIGNMENT_LONG_SETTLE_MS = 200;        // long settle/pause
const int ALIGNMENT_CYLINDER_PRE_EXTEND_MS = 150; // 150ms for alignment cylinder to position material
}  // namespace Timing 3