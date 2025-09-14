//* ************************************************************************
//* ************************ INPUT CONTROL ******************************
//* ************************************************************************
// Functions to setup and read input devices (buttons and sensors)

#include <Stage2_Machine.h>

//* ************************************************************************
//* ************************ INPUT SETUP ********************************
//* ************************************************************************

void setupInputs() {
    Serial.println("Initializing inputs...");
    
    // Configure input pins
    pinMode(Pins::HOME_SWITCH, INPUT_PULLDOWN);      // Home switch is active HIGH
    pinMode(Pins::START_BUTTON, INPUT_PULLDOWN);     // Start button is active HIGH  
    pinMode(Pins::TRANSFER_ARM_START_SIGNAL, INPUT_PULLDOWN); // Transfer signal is active HIGH
    pinMode(Pins::POSITION_VERIFICATION_SENSOR, INPUT_PULLUP);     // Position verification sensor is active LOW
    
    // Initialize Bounce2 objects for debouncing
    // Home switch - ultra-fast debounce for immediate homing response
    homeSwitch.attach(Pins::HOME_SWITCH);
    homeSwitch.interval(1); // 1ms debounce for instant homing response
    homeSwitch.setPressedState(HIGH); // Active HIGH
    
    // Start button - standard debounce for reliable button presses
    startButton.attach(Pins::START_BUTTON);
    startButton.interval(50); // 50ms debounce for reliable button detection
    startButton.setPressedState(HIGH); // Active HIGH
    
    // Transfer arm signal - moderate debounce for sensor reliability
    transferArmSignal.attach(Pins::TRANSFER_ARM_START_SIGNAL);
    transferArmSignal.interval(25); // 25ms debounce time
    transferArmSignal.setPressedState(HIGH); // Active HIGH
    
    // Position verification sensor - fast debounce for immediate detection
    positionVerificationSensor.attach(Pins::POSITION_VERIFICATION_SENSOR);
    positionVerificationSensor.interval(10); // 10ms debounce for reliable sensor detection
    positionVerificationSensor.setPressedState(LOW); // Active LOW
    
    Serial.println("Input initialization complete");
}

//* ************************************************************************
//* ************************ INPUT READING ******************************
//* ************************************************************************

void updateInputs() {
    // Update all input objects - must be called regularly for proper debouncing
    homeSwitch.update();
    startButton.update();
    transferArmSignal.update();
    positionVerificationSensor.update();
}

bool checkInputs() {
    // Check for any active inputs - used for general monitoring
    updateInputs();
    
    bool anyActive = false;
    
    if (homeSwitch.read()) {
        anyActive = true;
    }
    
    if (startButton.read()) {
        anyActive = true;
    }
    
    if (transferArmSignal.read()) {
        anyActive = true;
    }
    
    if (positionVerificationSensor.read()) {
        anyActive = true;
    }
    
    return anyActive;
}

//* ************************************************************************
//* ************************ UTILITY FUNCTIONS *************************
//* ************************************************************************

float inchesToSteps(float inches) {
    return inches * Motion::STEPS_PER_INCH;
}

float stepsToInches(float steps) {
    return steps / Motion::STEPS_PER_INCH;
}

//* ************************************************************************
//* ************************ CUTTING CYCLE INTERRUPTION ******************
//* ************************************************************************

bool checkStartButtonForHoming() {
    // Update inputs for reliable button detection
    updateInputs();
    
    // Check if start button is pressed during cutting cycle
    if (startButton.read()) {
        // Reset cycle flags and transition to homing
        cycleInProgress = false;
        homingComplete = false; // Force homing sequence
        
        // Stop any running motor movement
        stopMotor();
        
        // Keep both clamps extended for safe material handling
        // Only retract alignment cylinder for safe homing
        retractAlignmentCylinder();
        
        // Transition to homing state
        currentState = HOMING;
        
        return true; // Start button was pressed
    }
    
    return false; // No start button press
} 