#pragma once

// Pin Configuration - Using ESP32-S3 GPIO pins (Freenove ESP32-S3 board)
namespace Pins {
// Input pins
extern const int HOME_SWITCH;                  
extern const int START_BUTTON;                 
extern const int TRANSFER_ARM_START_SIGNAL;
extern const int END_POSITION_VERIFICATION_SENSOR;  
extern const int IS_ROUTER_CLEAR;                   

// Output pins
extern const int STEP;                        
extern const int DIR;                        
extern const int ENABLE;                       
extern const int LEFT_CLAMP;                  
extern const int RIGHT_CLAMP;                 
extern const int ALIGN_CYLINDER;               
extern const int ROUTER_START_SIGNAL;         // physical HIGH signal sent alongside ESP-NOW router start (pin 17)
}  // namespace Pins 