#include <Config.h>

// Board identification
namespace Config {
const char *BOARD_ID = "STAGE2_001";
const char *BOARD_DESCRIPTION = "Stage 2 Cutting Machine - USB Only";
}  // namespace Config

// Motion Parameters
namespace Motion {
const float STEPS_PER_INCH = 42.33;   // 200 steps/rev ÷ (60 teeth × 2mm ÷ 25.4mm/inch) = 42.33 steps/inch
const float HOME_OFFSET = .4;        // Position offset from home switch (sets position 0 after homing)
const float ALIGNMENT_INITIAL_DISTANCE = .5; // Initial distance for alignment
const float ALIGNMENT_BACKWARD_DISTANCE = 0.2;
const float APPROACH_DISTANCE = 4.0;  // Exact distance from offset position to approach cutting position
const float CUTTING_DISTANCE = 6.0;   // Exact distance to cut through material (from approach position)
const float TOTAL_FORWARD_DISTANCE = 24.5;  // Total exact distance from offset position to final position

// Speed Settings (steps/second)
const float HOMING_SPEED = 150;     
const float ALIGNMENT_INITIAL_SPEED = 200; // Initial speed for alignment
const float APPROACH_SPEED = 30000; 
const float CUTTING_SPEED = 75;    
const float FINISH_SPEED = 30000;   
const float RETURN_SPEED = 30000;   

// Acceleration Settings (steps/second²)
const float FORWARD_ACCEL = 16000;   
const float RETURN_ACCEL = 16000;    
}  // namespace Motion

// Timing Settings (milliseconds)
namespace Timing {
const float CLAMP_SETTLE_TIME = 150.0f;
const float CLAMP_RELEASE_TIME = 450.0f;
const float HOME_SETTLE_TIME = 100.0f;
const float MOTION_SETTLE_TIME = 50.0f;
const float ALIGN_SETTLE_TIME = 300.0f;
const float SIGNAL_DURATION = 2000.0f;
const float MOTOR_TIMEOUT = 5000.0f;
// Alignment-specific timing (ms)
const float ALIGNMENT_LEFT_CLAMP_EXTEND_MS = 50.0f;   // allow left clamp to extend
const float ALIGNMENT_ALIGNMENT_MOVE_MS = 20.0f;      // allow alignment movement to complete
const float ALIGNMENT_RIGHT_CLAMP_WAIT_MS = 150.0f;   // right clamp extension time
const float ALIGNMENT_SHORT_SETTLE_MS = 100.0f;       // short settle/pause
const float ALIGNMENT_LONG_SETTLE_MS = 200.0f;        // long settle/pause
}  // namespace Timing 3