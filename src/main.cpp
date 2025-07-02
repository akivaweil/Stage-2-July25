//* ************************************************************************
//* ************************ STAGE 2 SAW MACHINE **********************
//* ************************************************************************
// This code controls a Stage 2 cutting machine with pneumatic clamps,
// alignment cylinder, and stepper motor for precise cutting operations.

#include <Stage2_Machine.h>
#include <esp_task_wdt.h>

// OTA function declarations
void setupOTA();
void handleOTA();

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

// FastAccelStepper objects
FastAccelStepperEngine engine = FastAccelStepperEngine();
FastAccelStepper *stepper = NULL;

// Timing variables
unsigned long stateStartTime = 0;
unsigned long lastMotorUpdate = 0;

//* ************************************************************************
//* ************************ MAIN SETUP *******************************
//* ************************************************************************

void setup() {
    // Initialize serial communication
    Serial.begin(115200);
    delay(100); // Brief delay for serial initialization (reduced from 2000ms)
    Serial.println("Stage 2 Cutting Machine Starting...");
    
    // Initialize OTA
    Serial.println("Initializing OTA...");
    setupOTA();
    Serial.println("OTA initialization complete");
    
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
    // Feed the watchdog timer to prevent resets
    esp_task_wdt_reset();
    
    // Handle OTA requests (temporarily disabled to isolate issue)
    // handleOTA();
    
    // Update all input readings
    updateInputs();
    
    // Execute state machine
    runStateMachine();
    
    // Small delay for stability - reduced for better input responsiveness
    delay(5);
}