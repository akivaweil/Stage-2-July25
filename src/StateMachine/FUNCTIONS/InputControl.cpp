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
    pinMode(Pins::HOME_SWITCH, INPUT_PULLDOWN);                        // active HIGH
    pinMode(Pins::START_BUTTON, INPUT_PULLDOWN);                       // active HIGH
    pinMode(Pins::TRANSFER_ARM_START_SIGNAL, INPUT_PULLDOWN);          // active HIGH
    pinMode(Pins::END_POSITION_VERIFICATION_SENSOR, INPUT_PULLUP);     // active LOW
    pinMode(Pins::IS_ROUTER_CLEAR, INPUT_PULLUP);                      // active LOW
    
    // Initialize Bounce2 objects for debouncing
    // Home switch - ultra-fast debounce for immediate homing response
    homeSwitch.attach(Pins::HOME_SWITCH);
    homeSwitch.interval(1); // 1ms debounce for instant homing response
    homeSwitch.setPressedState(HIGH); // Active HIGH
    
    // Start button - standard debounce for reliable button presses
    startButton.attach(Pins::START_BUTTON);
    startButton.interval(5); // 5ms debounce for reliable button detection
    startButton.setPressedState(HIGH); // Active HIGH
    
    // Transfer arm signal - moderate debounce for sensor reliability
    transferArmSignal.attach(Pins::TRANSFER_ARM_START_SIGNAL);
    transferArmSignal.interval(25); // 25ms debounce time
    transferArmSignal.setPressedState(HIGH); // Active HIGH
    
    // End position verification sensor - ultra-fast debounce for immediate detection
    endPositionVerificationSensor.attach(Pins::END_POSITION_VERIFICATION_SENSOR);
    endPositionVerificationSensor.interval(2); // 2ms debounce for ultra-fast sensor detection
    endPositionVerificationSensor.setPressedState(LOW); // Active LOW
    
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
    endPositionVerificationSensor.update();
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
    
    if (endPositionVerificationSensor.read()) {
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
    
    // Ignore cancel button for CYCLE_START_IGNORE_MS after cycle begins to prevent
    // a held start button (from IDLE trigger) from immediately cancelling the cycle
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
        currentState = HOMING;
        
        return true; // Start button was pressed
    } else if (!startButtonCurrentlyPressed) {
        // Button is not pressed, reset the tracking variable
        startButtonWasPressed = false;
    }
    
    return false; // No start button press
} 