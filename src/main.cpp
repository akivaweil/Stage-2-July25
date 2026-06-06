//* ************************************************************************
//* ************************ STAGE 2 SAW MACHINE **********************
//* ************************************************************************
// This code controls a Stage 2 cutting machine with pneumatic clamps,
// alignment cylinder, and stepper motor for precise cutting operations.

#include <Stage2_Machine.h>
#include <WiFi.h>
#include "OTA/OTA_Upload.h"

//* ************************************************************************
//* ************************ GLOBAL VARIABLES **************************
//* ************************************************************************

// State machine variables
MachineState currentState = IDLE;
MachineState previousState = IDLE;
MachineState stateBeforeError = IDLE;

// Motor position tracking
float currentPosition = 0.0;

// System status flags
bool homingComplete = false;
bool cycleInProgress = false;
bool emergencyStop = false;
bool cutToHomingFlag = false;
bool fastReturnFlag = false;

// Position verification tracking
float positionVerificationDistance = 0.0;

// Input objects with Bounce2 library
Bounce2::Button homeSwitch;
Bounce2::Button startButton;
Bounce2::Button transferArmSignal;
Bounce2::Button endPositionVerificationSensor;

// FastAccelStepper objects
FastAccelStepperEngine engine = FastAccelStepperEngine();
FastAccelStepper *stepper = NULL;

// Timing variables
unsigned long stateStartTime = 0;
unsigned long lastMotorUpdate = 0;

// Button state tracking for rising edge detection
bool startButtonWasPressed = false;
bool transferArmSignalWasActive = false;

// Tracks when the current cutting cycle started (for start-button ignore window)
unsigned long cycleStartTime = 0;

// Tracks when the last cycle finished (for inter-cycle cooldown enforced in IDLE)
unsigned long cycleEndTime = 0;

//* ************************************************************************
//* ************************ MAIN SETUP *******************************
//* ************************************************************************

void setup() {
    Serial.begin(115200);
    delay(100); // Brief delay for serial initialization
    Serial.println("Stage 2 Cutting Machine Starting...");
    
    // Initialize OTA
    Serial.println("Initializing OTA...");
    setupOTA();
    
    // Setup hardware components
    Serial.println("Initializing hardware...");
    setupHardware();
    Serial.println("Hardware initialization complete");
    
    Serial.println("Initializing inputs...");
    setupInputs();
    Serial.println("Input initialization complete");
    
    Serial.println("Initializing motor...");
    setupMotor();
    Serial.println("Motor initialization complete");
    
    // Set initial state
    changeState(HOMING);
    
    Serial.println("Setup complete - machine ready for operation");
}

//* ************************************************************************
//* ************************ MAIN LOOP ********************************
//* ************************************************************************

void loop() {
    // Handle OTA updates first
    handleOTA();
    
    // Update all input readings
    updateInputs();
    
    // Execute state machine
    runStateMachine();
    
    // Small delay for stability - reduced for better input responsiveness
    delay(5);
}