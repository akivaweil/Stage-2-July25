// Stage 2 saw machine
// This code controls a Stage 2 cutting machine with pneumatic clamps,
// alignment cylinder, and stepper motor for precise cutting operations.

#include "StateMachine/StateMachine.h"
#include <WiFi.h>
#include "OTA/OTA_Upload.h"
#include <esp_task_wdt.h>

// Task watchdog timeout (seconds): resets the chip if the loop task stalls longer
// than this. The loop, the blocking motor-complete wait, and the OTA progress
// callback all feed it.
static const uint32_t WATCHDOG_TIMEOUT_S = 15;

// Global variables

// State machine variables
SystemState currentState = STATE_IDLE;
SystemState previousState = STATE_IDLE;
SystemState stateBeforeError = STATE_IDLE;

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
Bounce2::Button isRouterClearSensor;

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

// Main setup

void setup() {
    Serial.begin(115200);
    delay(100); // Brief delay for serial initialization
    Serial.println("[Stage2] booting");

    // Initialize OTA
    setupOTA();

    // Setup hardware components
    setupHardware();

    setupInputs();

    setupMotor();

    // Set initial state
    changeState(STATE_HOMING);

    // Subscribe the loop task to the task watchdog (after boot-time WiFi connect).
    // esp_task_wdt_init() reconfigures the core's TWDT to our timeout; add(NULL)
    // watches this (loop) task.
    esp_task_wdt_init(WATCHDOG_TIMEOUT_S, true);
    esp_task_wdt_add(NULL);

    Serial.println("[Stage2] ready");
}

// Main loop

void loop() {
    // Feed the task watchdog each iteration.
    esp_task_wdt_reset();

    // Handle OTA updates first
    handleOTA();
    
    // Update all input readings
    updateInputs();
    
    // Execute state machine
    runStateMachine();
    
    // Small delay for stability - reduced for better input responsiveness
    delay(5);
}