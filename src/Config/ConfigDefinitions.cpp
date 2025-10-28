#include <Config.h>
#include <MotorConversion.h>

//* ************************************************************************
//* ************************ CONFIGURATION SUMMARY ***********************
//* ************************************************************************
// Motor: 3200 steps/rev (converted from 200 steps/rev base values)
// Base values in this file are the "comfortable" values you're familiar with
// Automatic conversion factor of 16.0 applied to all speeds and accelerations
// See MotorConversion.h for the conversion factor
//* ************************************************************************

// Board identification
namespace Config {
const char *BOARD_ID = "STAGE2_001";
const char *BOARD_DESCRIPTION = "Stage 2 Cutting Machine - USB Only";
}  // namespace Config

// Motion Parameters - All positions are absolute relative to home (position 0)
namespace Motion {
const float STEPS_PER_INCH = 677.33;   // 3200 steps/rev ÷ (60 teeth × 2mm ÷ 25.4mm/inch) = 677.33 steps/inch

// Absolute positions relative to home (position 0)
const float HOME_POSITION = 0.0;                    // Home position (where switch is triggered)
const float HOME_OFFSET_POSITION = 0.01;             // Position after homing (0.5" from home switch)
const float ALIGNMENT_INITIAL_POSITION =0.8;       // Position after initial alignment move (0.5" + 0.5")
const float ALIGNMENT_BACKWARD_POSITION = 0.3;      // Position after backward alignment move (1.0" - 0.3")
const float ALIGNMENT_FINAL_POSITION = 0.4;         // Position after final alignment backward move (0.7" - 0.3")
const float APPROACH_POSITION = 5.1;               // Position for cutting approach (0.4" + 4.15" + 0.1" net alignment)
const float CUTTING_POSITION = 12.0;               // Position after cutting (4.65" + 6.4")
const float FINAL_POSITION = 24.3;                 // Final position after finish move (0.4" + 24.15")

// Speed Settings (steps/second) - Base values for 200 steps/rev, auto-converted for 3200 steps/rev
const float HOMING_SPEED_BASE = 150;
const float ALIGNMENT_INITIAL_SPEED_BASE = 150;
const float APPROACH_SPEED_BASE = 30000;
const float CUTTING_SPEED_BASE = 65;
const float FINISH_SPEED_BASE = 30000;
const float RETURN_SPEED_BASE = 25000;
const float FINAL_SPEED_BASE = 50;

// Converted speeds for 3200 steps/rev motor
const float HOMING_SPEED = HOMING_SPEED_BASE * MotorConversion::CONVERSION_FACTOR;
const float ALIGNMENT_INITIAL_SPEED = ALIGNMENT_INITIAL_SPEED_BASE * MotorConversion::CONVERSION_FACTOR;
const float APPROACH_SPEED = APPROACH_SPEED_BASE * MotorConversion::CONVERSION_FACTOR;
const float CUTTING_SPEED = CUTTING_SPEED_BASE * MotorConversion::CONVERSION_FACTOR;
const float FINISH_SPEED = FINISH_SPEED_BASE * MotorConversion::CONVERSION_FACTOR;
const float RETURN_SPEED = RETURN_SPEED_BASE * MotorConversion::CONVERSION_FACTOR;
const float FINAL_SPEED = FINAL_SPEED_BASE * MotorConversion::CONVERSION_FACTOR;   

// Acceleration Settings (steps/second²) - Base values for 200 steps/rev, auto-converted for 3200 steps/rev
const float FORWARD_ACCEL_BASE = 10000;
const float RETURN_ACCEL_BASE = 16000;

// Converted accelerations for 3200 steps/rev motor
const float FORWARD_ACCEL = FORWARD_ACCEL_BASE * MotorConversion::CONVERSION_FACTOR;
const float RETURN_ACCEL = RETURN_ACCEL_BASE * MotorConversion::CONVERSION_FACTOR;    
}  // namespace Motion

// Timing Settings (milliseconds)
namespace Timing {
const int CLAMP_SETTLE_TIME = 100;
const int CLAMP_RELEASE_TIME = 400;
const int HOME_SETTLE_TIME = 100;
const int MOTION_SETTLE_TIME = 50;
const int ALIGN_SETTLE_TIME = 200;
const int SIGNAL_DURATION = 2000;
const int MOTOR_TIMEOUT = 5000;
const int RETURN_TIMEOUT = 5000;             // Timeout for returning state before waiting for start button
// Alignment-specific timing (ms)
const int ALIGNMENT_LEFT_CLAMP_EXTEND_MS = 50;   // allow left clamp to extend
const int ALIGNMENT_ALIGNMENT_MOVE_MS = 270;      // allow alignment movement to complete
const int ALIGNMENT_RIGHT_CLAMP_WAIT_MS = 150;   // right clamp extension time
const int ALIGNMENT_SHORT_SETTLE_MS = 100;       // short settle/pause
const int ALIGNMENT_LONG_SETTLE_MS = 200;        // long settle/pause
const int ALIGNMENT_CYLINDER_PRE_EXTEND_MS = 150; // 150ms for alignment cylinder to position material

// Oscillation Settings for clamp release - Base values for 200 steps/rev, auto-converted for 3200 steps/rev
const float OSCILLATION_SPEED_BASE = 2000;
const float OSCILLATION_ACCEL_BASE = 5000;
const float OSCILLATION_DISTANCE = 0.1;      // Distance from center position (inches)

// Converted oscillation settings for 3200 steps/rev motor
const float OSCILLATION_SPEED = OSCILLATION_SPEED_BASE * MotorConversion::CONVERSION_FACTOR;
const float OSCILLATION_ACCEL = OSCILLATION_ACCEL_BASE * MotorConversion::CONVERSION_FACTOR;
}  // namespace Timing