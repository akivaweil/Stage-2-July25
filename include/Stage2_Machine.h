#pragma once

#include <Arduino.h>
#include <Bounce2.h>
#include <FastAccelStepper.h>

// Config includes
#include "Config.h"
#include "MotorConversion.h"
#include "Pins_Definitions.h"

// State Machine enum
enum MachineState {
    IDLE,
    HOMING,
    ALIGNMENT,
    CUTTING,
    RETURNING,
    ROUTER_CLEAR_ERROR
};

// Global variables
extern MachineState currentState;
extern MachineState previousState;
extern MachineState stateBeforeError;
extern float currentPosition;
extern bool homingComplete;
extern bool cycleInProgress;
extern bool emergencyStop;
extern float positionVerificationDistance;
extern bool cutToHomingFlag;

// Timing variables
extern unsigned long stateStartTime;
extern unsigned long lastMotorUpdate;

// Button state tracking for rising edge detection
extern bool startButtonWasPressed;
extern bool transferArmSignalWasActive;

// Input objects
extern Bounce2::Button homeSwitch;
extern Bounce2::Button startButton;
extern Bounce2::Button transferArmSignal;
extern Bounce2::Button endPositionVerificationSensor;

// FastAccelStepper objects
extern FastAccelStepperEngine engine;
extern FastAccelStepper *stepper;

// Function declarations
void setup();
void loop();

// State machine functions
void runStateMachine();
void changeState(MachineState newState);

// Motor control functions
void setupMotor();
void moveMotor(float steps, float speed, float acceleration);
void moveMotorToPosition(float targetSteps, float speed, float acceleration);
void stopMotor();
void enableMotor();
void disableMotor();
bool isMotorRunning();
void waitForMotorComplete();
long getCurrentMotorPosition();
void setCurrentMotorPosition(long position);

// Hardware control functions
void setupHardware();
void extendLeftClamp();
void retractLeftClamp();
void extendRightClamp();
void retractRightClamp();
void extendBothClamps();
void retractBothClamps();
void extendAlignmentCylinder();
void retractAlignmentCylinder();

// Utility functions
void setupInputs();
void updateInputs();
bool checkInputs();
bool checkStartButtonForHoming();
float inchesToSteps(float inches);
float stepsToInches(float steps);

// State handler function declarations
void handleIdleState();
void handleHomingState();
void handleAlignmentState();
void handleCuttingState();
void handleReturningState();
void handleRouterClearErrorState();

// Cutting state utility functions
void setCuttingPhaseToContinue();

// OTA function declarations are in OTA/OTA_Upload.h 