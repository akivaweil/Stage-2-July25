#include <Config.h>

namespace Config {
const char *BOARD_ID = "STAGE2_001";
const char *BOARD_DESCRIPTION = "Stage 2 Cutting Machine - USB Only";
}  // namespace Config

// ─── MOTION ──────────────────────────────────────────────────────────────────
// All positions are absolute inches from home (0.0).
// All speeds are in steps/sec; all accelerations in steps/sec².
// Motor: 3200 steps/rev, 60-tooth pulley, 2mm pitch belt → 677.33 steps/inch
namespace Motion {
const float STEPS_PER_INCH = 677.33;

// ── Positions (inches) ───────────────────────────────────────────────────────
const float HOME_POSITION              = 0.0;   // Where the home switch triggers
const float HOME_OFFSET_POSITION       = 0.01;  // Tiny move off the switch after homing
const float ALIGNMENT_SHORT_FORWARD_POSITION = 0.15f;  // Short forward nudge before pneumatic alignment sequence
const float ALIGNMENT_INITIAL_POSITION = 0.8;   // Motor moves forward to here to push material against fence
const float ALIGNMENT_BACKWARD_POSITION= 0.3;   // Motor pulls back to here to release pressure on fence
const float ALIGNMENT_FINAL_POSITION   = 0.4;   // Motor settles here after alignment is complete
const float APPROACH_POSITION          = 5.1;   // Motor fast-travels to here before the blade enters material
const float CUTTING_POSITION           = 11.0;  // Motor slow-cuts to here (blade fully through material)
const float FINAL_POSITION             = 24.4;  // Motor fast-finishes to here to clear the cut piece

// ── Speeds (steps/sec) ───────────────────────────────────────────────────────
const float HOMING_SPEED            = 2520;    // Slow crawl used during homing
const float ALIGNMENT_INITIAL_SPEED = 2520;    // Slow speed for alignment nudge moves
const float APPROACH_SPEED          = 105000;  // Full speed approach before blade touches material
const float CUTTING_SPEED           = 1379;    // Slow feed rate while blade is cutting
const float FINISH_SPEED            = 105000;  // Full speed move after cut is complete
const float RETURN_SPEED            = 105000;  // Full speed return to home
const float RETURN_SPEED_FAST_MULT  = 1.5;     // Multiplier applied when fastReturnFlag is set (IS_ROUTER_CLEAR wait path)
const float FINAL_SPEED             = 840;     // Creep speed for end-of-travel position verification

// ── Accelerations (steps/sec²) ───────────────────────────────────────────────
const float FORWARD_ACCEL = 100000;  // Ramp-up used for all forward moves
const float RETURN_ACCEL  = 160000;  // Slightly higher ramp for return (no load)
}  // namespace Motion

// ─── TIMING ──────────────────────────────────────────────────────────────────
// All values in milliseconds unless noted.
namespace Timing {
const int CLAMP_SETTLE_TIME  = 100;   // Wait after extending clamps before moving
const int CLAMP_RELEASE_TIME = 200;   // Dwell with clamps retracted during board release
const int HOME_SETTLE_TIME   = 100;   // Wait after homing switch triggers before moving off
const int MOTION_SETTLE_TIME = 50;    // Short pause after any general motion completes
const int ALIGN_SETTLE_TIME  = 200;   // Pause after alignment sequence finishes
const int SIGNAL_DURATION    = 2000;  // Duration of output signals (e.g. done signal)
const int MOTOR_TIMEOUT      = 5000;  // Max time to wait for motor to finish before giving up
const int RETURN_TIMEOUT           = 5000;  // Max time in RETURNING state before re-checking start button
const int CYCLE_START_IGNORE_MS    = 1500;  // Ignore start-button cancel for this long after a cycle begins

// ── Alignment choreography timing ────────────────────────────────────────────
const int ALIGNMENT_LEFT_CLAMP_EXTEND_MS      = 50;   // Time for left clamp to extend before nudge move
const int ALIGNMENT_ALIGNMENT_MOVE_MS         = 270;  // Time allowed for the alignment nudge move
const int ALIGNMENT_RIGHT_CLAMP_WAIT_MS       = 150;  // Time for right clamp to extend after nudge
const int ALIGNMENT_SHORT_SETTLE_MS           = 100;  // Brief pause between alignment steps
const int ALIGNMENT_LONG_SETTLE_MS            = 200;  // Longer pause for hardware to settle
const int ALIGNMENT_CYLINDER_PRE_EXTEND_MS    = 250;  // Dwell after alignment cylinder extends before left clamp
const int ALIGNMENT_CYLINDER_EXTENDED_BEFORE_RETRACT_MS = 150;  // Keep alignment cylinder extended after left clamp before retract
const int ALIGNMENT_AFTER_RETRACT_BEFORE_RIGHT_CLAMP_MS = 300;  // Wait after retracting alignment cylinder before right clamp (includes retract settle)

// ── Clamp-release oscillation ────────────────────────────────────────────────
const float OSCILLATION_SPEED    = 33600;  // Speed for back-and-forth oscillation moves (steps/sec)
const float OSCILLATION_ACCEL    = 80000;  // Acceleration for oscillation moves (steps/sec²)
const float OSCILLATION_DISTANCE = 0.1;   // Half-width of oscillation (inches each direction from center)
}  // namespace Timing