#pragma once

// WiFi credentials and board identification
namespace Config {
extern const char *WIFI_SSID;
extern const char *WIFI_PASSWORD;
extern const char *BOARD_ID;
extern const char *BOARD_DESCRIPTION;
}  // namespace Config

// Motion Parameters
namespace Motion {
extern const float STEPS_PER_INCH;   // 200 steps/rev ÷ (60 teeth × 2mm ÷ 25.4mm/inch) = 42.33 steps/inch
extern const float HOME_OFFSET;        // Position offset from home switch
extern const float APPROACH_DISTANCE;  // Distance to approach cutting position
extern const float CUTTING_DISTANCE;   // Distance to cut through material
extern const float TOTAL_FORWARD_DISTANCE;  // Total forward distance

// Speed Settings (steps/second)
extern const float HOMING_SPEED;     
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
}  // namespace Timing