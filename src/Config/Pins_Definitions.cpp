#pragma once

// Pin Configuration - Using ESP32-S3 GPIO pins (Freenove ESP32-S3 board)
namespace Pins {
// Input pins
inline const int HOME_SWITCH = 1;                  
inline const int START_BUTTON = 2;                 
inline const int TRANSFER_ARM_START_SIGNAL = 47;  

// Output pins
inline const int STEP = 38;                        
inline const int DIR = 37;                        
inline const int ENABLE = 8;                       
inline const int LEFT_CLAMP = 46;                  
inline const int RIGHT_CLAMP = 10;                 
inline const int ALIGN_CYLINDER = 3;               
inline const int TRANSFER_ARM_SIGNAL = 48;         
}  // namespace Pins 