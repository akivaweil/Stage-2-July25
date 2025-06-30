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
    
    // Initialize Bounce2 objects for debouncing
    homeSwitch.attach(Pins::HOME_SWITCH);
    homeSwitch.interval(10); // 10ms
    homeSwitch.setPressedState(HIGH); // Active HIGH
    
    startButton.attach(Pins::START_BUTTON);
    startButton.interval(25); // 25ms debounce time
    startButton.setPressedState(HIGH); // Active HIGH
    
    transferArmSignal.attach(Pins::TRANSFER_ARM_START_SIGNAL);
    transferArmSignal.interval(25); // 25ms debounce time
    transferArmSignal.setPressedState(HIGH); // Active HIGH
    
    Serial.println("Input initialization complete");
}

//* ************************************************************************
//* ************************ INPUT READING ******************************
//* ************************************************************************

void updateInputs() {
    // Update all input objects
    homeSwitch.update();
    startButton.update();
    transferArmSignal.update();
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