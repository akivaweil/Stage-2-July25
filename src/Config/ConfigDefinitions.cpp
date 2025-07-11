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
const float TOTAL_FORWARD_DISTANCE = 24.35;  // Total exact distance from offset position to final position

// Speed Settings (steps/second)
const float HOMING_SPEED = 150;     
const float ALIGNMENT_INITIAL_SPEED = 70; // Initial speed for alignment
const float APPROACH_SPEED = 30000; 
const float CUTTING_SPEED = 70;    
const float FINISH_SPEED = 30000;   
const float RETURN_SPEED = 30000;   

// Acceleration Settings (steps/second²)
const float FORWARD_ACCEL = 14000;   
const float RETURN_ACCEL = 17000;    
}  // namespace Motion

// Timing Settings (milliseconds)
namespace Timing {
const int CLAMP_SETTLE_TIME = 250;
const int CLAMP_RELEASE_TIME = 300;
const int HOME_SETTLE_TIME = 100;
const int MOTION_SETTLE_TIME = 50;
const int ALIGN_SETTLE_TIME = 300;
const int SIGNAL_DURATION = 500;
const int MOTOR_TIMEOUT = 5000;
}  // namespace Timing 