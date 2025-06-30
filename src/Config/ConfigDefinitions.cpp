#include "Config.h"

// WiFi credentials and board identification
namespace Config {
const char *WIFI_SSID = "Everwood";
const char *WIFI_PASSWORD = "Everwood-Staff";
const char *BOARD_ID = "STAGE2_001";
const char *BOARD_DESCRIPTION = "Stage 2 Cutting Machine";
}  // namespace Config

// Motion Parameters
namespace Motion {
const float STEPS_PER_INCH = 42.33;   // 200 steps/rev ÷ (60 teeth × 2mm ÷ 25.4mm/inch) = 42.33 steps/inch
const float HOME_OFFSET = 1.0;        // Position offset from home switch
const float APPROACH_DISTANCE = 8.0;  // Distance to approach cutting position
const float CUTTING_DISTANCE = 4.0;   // Distance to cut through material
const float TOTAL_FORWARD_DISTANCE = 14.0;  // Total forward distance

// Speed Settings (steps/second)
const float HOMING_SPEED = 423;     
const float APPROACH_SPEED = 12000; 
const float CUTTING_SPEED = 100;    
const float FINISH_SPEED = 14000;   
const float RETURN_SPEED = 14000;   

// Acceleration Settings (steps/second²)
const float FORWARD_ACCEL = 2000;   
const float RETURN_ACCEL = 2000;    
}  // namespace Motion

// Timing Settings (milliseconds)
namespace Timing {
const int CLAMP_SETTLE_TIME = 200;
const int CLAMP_RELEASE_TIME = 200;
const int HOME_SETTLE_TIME = 100;
const int MOTION_SETTLE_TIME = 50;
const int ALIGN_SETTLE_TIME = 300;
const int SIGNAL_DURATION = 100;
}  // namespace Timing 