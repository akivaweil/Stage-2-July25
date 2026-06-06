#include "Config/Pins_Definitions.h"

// Pin Configuration - Using ESP32-S3 GPIO pins (Freenove ESP32-S3 board)
namespace Pins {
// Input pins (using input-capable pins)
const int HOME_SWITCH = 1;                  
const int START_BUTTON = 2;                 
const int TRANSFER_ARM_START_SIGNAL = 47;
const int END_POSITION_VERIFICATION_SENSOR = 35;  
const int IS_ROUTER_CLEAR = 39;                   

// Output pins (using safe output pins)
const int STEP = 38;                        
const int DIR = 11;                        
const int ENABLE = 8;                       
const int LEFT_CLAMP = 10;
const int RIGHT_CLAMP = 46;                 
const int ALIGN_CYLINDER = 3;               
const int ROUTER_START_SIGNAL = 17;         // physical HIGH signal sent with ESP-NOW router start
}  // namespace Pins 