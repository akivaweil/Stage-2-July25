#pragma once

// Pin Configuration - Using ESP32-S3 GPIO pins (Freenove ESP32-S3 board)
namespace Pins {
// Input pins
extern const int HOME_SWITCH;                  
extern const int START_BUTTON;                 
extern const int TRANSFER_ARM_START_SIGNAL;  

// Output pins
extern const int STEP;                        
extern const int DIR;                        
extern const int ENABLE;                       
extern const int LEFT_CLAMP;                  
extern const int RIGHT_CLAMP;                 
extern const int ALIGN_CYLINDER;               
extern const int TRANSFER_ARM_SIGNAL;         
extern const int CLAMP_RELEASE_SIGNAL;        // dedicated output for clamp release signal (formerly pin 17)
}  // namespace Pins 