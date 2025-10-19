#pragma once

// Board identification
namespace Config {
extern const char *BOARD_ID;
extern const char *BOARD_DESCRIPTION;
}  // namespace Config

// Motion Parameters
namespace Motion {
extern const float STEPS_PER_INCH;   // 200 steps/rev ÷ (60 teeth × 2mm ÷ 25.4mm/inch) = 42.33 steps/inch
extern const float HOME_OFFSET;        // Position offset from home switch (sets position 0 after homing)
extern const float ALIGNMENT_INITIAL_DISTANCE; // Initial distance for alignment
extern const float ALIGNMENT_BACKWARD_DISTANCE;
extern const float APPROACH_DISTANCE;  // Exact distance from offset position to approach cutting position
extern const float CUTTING_DISTANCE;   // Exact distance to cut through material (from approach position)
extern const float TOTAL_FORWARD_DISTANCE;  // Total exact distance from offset position to final position

// Speed Settings (steps/second)
extern const float HOMING_SPEED;     
extern const float ALIGNMENT_INITIAL_SPEED; // Initial speed for alignment
extern const float APPROACH_SPEED; 
extern const float CUTTING_SPEED;    
extern const float FINISH_SPEED;   
extern const float RETURN_SPEED;   

// Acceleration Settings (steps/second²)
extern const float FORWARD_ACCEL;   
extern const float RETURN_ACCEL;    
}  // namespace Motion

// Timing Settings (milliseconds)
namespace Timing {
extern const int CLAMP_SETTLE_TIME;
extern const int CLAMP_RELEASE_TIME;
extern const int HOME_SETTLE_TIME;
extern const int MOTION_SETTLE_TIME;
extern const int ALIGN_SETTLE_TIME;
extern const int SIGNAL_DURATION;
extern const int MOTOR_TIMEOUT;
// Alignment-specific timing (ms)
extern const int ALIGNMENT_LEFT_CLAMP_EXTEND_MS;   // allow left clamp to extend
extern const int ALIGNMENT_ALIGNMENT_MOVE_MS;      // allow alignment movement to complete
extern const int ALIGNMENT_RIGHT_CLAMP_WAIT_MS;    // right clamp extension time
extern const int ALIGNMENT_SHORT_SETTLE_MS;        // short settle/pause
extern const int ALIGNMENT_LONG_SETTLE_MS;         // long settle/pause
extern const int ALIGNMENT_CYLINDER_PRE_EXTEND_MS; // 150ms for alignment cylinder to position material
}  // namespace Timing