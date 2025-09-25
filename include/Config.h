#pragma once

// Board identification
namespace Config {
extern const char *BOARD_ID;
extern const char *BOARD_DESCRIPTION;
}  // namespace Config

// Motion Parameters - All positions are absolute relative to home (position 0)
namespace Motion {
extern const float STEPS_PER_INCH;   // 200 steps/rev ÷ (60 teeth × 2mm ÷ 25.4mm/inch) = 42.33 steps/inch

// Absolute positions relative to home (position 0)
extern const float HOME_POSITION;                    // Home position (where switch is triggered)
extern const float HOME_OFFSET_POSITION;             // Position after homing (0.5" from home switch)
extern const float ALIGNMENT_INITIAL_POSITION;       // Position after initial alignment move
extern const float ALIGNMENT_BACKWARD_POSITION;      // Position after backward alignment move
extern const float ALIGNMENT_FINAL_POSITION;         // Position after final alignment backward move
extern const float APPROACH_POSITION;                // Position for cutting approach
extern const float CUTTING_POSITION;                 // Position after cutting
extern const float FINAL_POSITION;                   // Final position after finish move

// Speed Settings (steps/second)
extern const float HOMING_SPEED;     
extern const float ALIGNMENT_INITIAL_SPEED; // Initial speed for alignment
extern const float APPROACH_SPEED; 
extern const float CUTTING_SPEED;    
extern const float FINISH_SPEED;   
extern const float RETURN_SPEED;   
extern const float FINAL_SPEED;   

// Acceleration Settings (steps/second²)
extern const float FORWARD_ACCEL;   
extern const float RETURN_ACCEL;    
}  // namespace Motion

// Timing Settings (milliseconds)
//! NOTE: Use integer millisecond timing constants. Avoid floats unless absolutely necessary.
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
extern const int ALIGNMENT_CYLINDER_PRE_EXTEND_MS;  // 150ms for alignment cylinder to position material

// Oscillation Settings for clamp release
extern const float OSCILLATION_SPEED;        // Speed for oscillation movement (steps/second)
extern const float OSCILLATION_DISTANCE;     // Distance from center position (inches)
extern const int OSCILLATION_CYCLES;         // Number of oscillation cycles during release
}  // namespace Timing