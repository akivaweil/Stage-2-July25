//* ************************************************************************
//* ************************ STAGE 2 SAW MACHINE **********************
//* ************************************************************************
// This code controls a Stage 2 cutting machine with pneumatic clamps,
// alignment cylinder, and stepper motor for precise cutting operations.

#include <Stage2_Machine.h>

//* ************************************************************************
//* ************************ GLOBAL VARIABLES **************************
//* ************************************************************************

// State machine variables
MachineState currentState = IDLE;
MachineState previousState = IDLE;

// Motor position tracking
float currentPosition = 0.0;

// System status flags
bool homingComplete = false;
bool cycleInProgress = false;
bool emergencyStop = false;

// Input objects with Bounce2 library
Bounce2::Button homeSwitch;
Bounce2::Button startButton;
Bounce2::Button transferArmSignal;

// Timing variables
unsigned long stateStartTime = 0;
unsigned long lastMotorUpdate = 0;

//* ************************************************************************
//* ************************ MAIN SETUP *******************************
//* ************************************************************************

void setup() {
    // Initialize serial communication
    Serial.begin(SERIAL_BAUDRATE);
    Serial.println("Stage 2 Cutting Machine Starting...");
    
    // Setup all hardware components
    setupHardware();
    setupInputs();
    setupMotor();
    
    // Initialize WiFi connection
    setupWiFi();
    
    // Initialize OTA
    setupOTA();
    
    // Set initial state
    changeState(HOMING);
    
    Serial.println("Setup complete - Starting homing sequence");
}

//* ************************************************************************
//* ************************ MAIN LOOP ********************************
//* ************************************************************************

void loop() {
    // Handle OTA updates
    handleOTA();
    
    // Update all input readings
    updateInputs();
    
    // Execute state machine
    executeStateMachine();
    
    // Small delay for stability
    delay(10);
}