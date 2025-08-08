#pragma once

// Board identification
namespace Config {
extern const char *BOARD_ID;
extern const char *BOARD_DESCRIPTION;
}  // namespace Config

// Motion Parameters
namespace Motion {
extern const float STEPS_PER_INCH;   // 200 steps/rev ÷ (60 teeth × 2mm ÷ 25.4mm/inch) = 42.33 steps/inch
extern const float HOME_OFFSET;        // Position offset from home switch
extern const float ALIGNMENT_INITIAL_DISTANCE; // Initial distance for alignment
extern const float ALIGNMENT_BACKWARD_DISTANCE;
extern const float APPROACH_DISTANCE;  // Distance to approach cutting position
extern const float CUTTING_DISTANCE;   // Distance to cut through material
extern const float TOTAL_FORWARD_DISTANCE;  // Total forward distance

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
// Alignment-specific timing (ms) to avoid magic numbers in alignment choreography
extern const int ALIGNMENT_LEFT_CLAMP_EXTEND_MS;    // was 50
extern const int ALIGNMENT_ALIGNMENT_MOVE_MS;       // was 20
extern const int ALIGNMENT_RIGHT_CLAMP_WAIT_MS;     // was 150
extern const int ALIGNMENT_SHORT_SETTLE_MS;         // was 100
extern const int ALIGNMENT_LONG_SETTLE_MS;          // was 200
}  // namespace Timing