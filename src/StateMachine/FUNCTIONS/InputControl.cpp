// Input control
// Functions to setup and read input devices (buttons and sensors)

#include "StateMachine/StateMachine.h"

// Bounce2 debounce intervals (ms) — named to avoid bare magic numbers
namespace InputDebounce {
    const int HOME_SWITCH_DEBOUNCE_MS                = 1;   // Ultra-fast for instant homing response
    const int START_BUTTON_DEBOUNCE_MS               = 5;   // Reliable button detection
    const int TRANSFER_ARM_SIGNAL_DEBOUNCE_MS        = 25;  // Moderate for sensor reliability
    const int END_POSITION_SENSOR_DEBOUNCE_MS        = 2;   // Ultra-fast sensor detection
    const int IS_ROUTER_CLEAR_DEBOUNCE_MS            = 5;   // Short debounce for router-clear physical sensor
}  // namespace InputDebounce

// Input setup

void setupInputs() {
    // Configure input pins
    pinMode(Pins::HOME_SWITCH, INPUT_PULLDOWN);                        // active HIGH
    pinMode(Pins::START_BUTTON, INPUT_PULLDOWN);                       // active HIGH
    pinMode(Pins::TRANSFER_ARM_START_SIGNAL, INPUT_PULLDOWN);          // active HIGH
    pinMode(Pins::END_POSITION_VERIFICATION_SENSOR, INPUT_PULLUP);     // active LOW
    pinMode(Pins::IS_ROUTER_CLEAR, INPUT_PULLUP);                      // active LOW
    
    // Initialize Bounce2 objects for debouncing
    // Home switch - ultra-fast debounce for immediate homing response
    homeSwitch.attach(Pins::HOME_SWITCH);
    homeSwitch.interval(InputDebounce::HOME_SWITCH_DEBOUNCE_MS); // instant homing response
    homeSwitch.setPressedState(HIGH); // Active HIGH

    // Start button - standard debounce for reliable button presses
    startButton.attach(Pins::START_BUTTON);
    startButton.interval(InputDebounce::START_BUTTON_DEBOUNCE_MS); // reliable button detection
    startButton.setPressedState(HIGH); // Active HIGH

    // Transfer arm signal - moderate debounce for sensor reliability
    transferArmSignal.attach(Pins::TRANSFER_ARM_START_SIGNAL);
    transferArmSignal.interval(InputDebounce::TRANSFER_ARM_SIGNAL_DEBOUNCE_MS); // sensor reliability
    transferArmSignal.setPressedState(HIGH); // Active HIGH

    // End position verification sensor - ultra-fast debounce for immediate detection
    endPositionVerificationSensor.attach(Pins::END_POSITION_VERIFICATION_SENSOR);
    endPositionVerificationSensor.interval(InputDebounce::END_POSITION_SENSOR_DEBOUNCE_MS); // ultra-fast sensor detection
    endPositionVerificationSensor.setPressedState(LOW); // Active LOW

    // Router-clear physical sensor - short debounce on the physical read (active LOW)
    isRouterClearSensor.attach(Pins::IS_ROUTER_CLEAR);
    isRouterClearSensor.interval(InputDebounce::IS_ROUTER_CLEAR_DEBOUNCE_MS); // short debounce
    isRouterClearSensor.setPressedState(LOW); // Active LOW
}

// Input reading

void updateInputs() {
    // Update all input objects - must be called regularly for proper debouncing
    homeSwitch.update();
    startButton.update();
    transferArmSignal.update();
    endPositionVerificationSensor.update();
    isRouterClearSensor.update();
}

// Utility functions

float inchesToSteps(float inches) {
    return inches * Motion::STEPS_PER_INCH;
}

float stepsToInches(float steps) {
    return steps / Motion::STEPS_PER_INCH;
}

// Cutting cycle interruption

bool checkStartButtonForHoming() {
    // Update inputs for reliable button detection
    updateInputs();
    
    // Ignore start-to-homing only for CYCLE_START_IGNORE_MS after cycle begins (held
    // start from IDLE trigger); after that window, start press stops the cycle
    if (cycleStartTime > 0 && (millis() - cycleStartTime) < (unsigned long)Timing::CYCLE_START_IGNORE_MS) {
        return false;
    }
    
    // Check if start button rising edge (button press) during cutting cycle
    bool startButtonCurrentlyPressed = startButton.read();
    if (startButtonCurrentlyPressed && !startButtonWasPressed) {
        // Rising edge detected - button was just pressed
        startButtonWasPressed = true;
        
        // Reset cycle flags and transition to homing
        cycleInProgress = false;
        homingComplete = false; // Force homing sequence
        
        // Stop any running motor movement
        stopMotor();
        
        // Disable and re-enable motor to clear any jams
        disableMotor();
        enableMotor();
        
        // Keep both clamps extended for safe material handling
        // Only retract alignment cylinder for safe homing
        retractAlignmentCylinder();
        
        // Transition to homing state
        currentState = STATE_HOMING;
        
        return true; // Start button was pressed
    } else if (!startButtonCurrentlyPressed) {
        // Button is not pressed, reset the tracking variable
        startButtonWasPressed = false;
    }
    
    return false; // No start button press
} 