#pragma once

#include <Arduino.h>
#include <Bounce2.h>
#include <WiFi.h>

// Config includes
#include "Config/Config.cpp"
#include "Config/Pins_Definitions.cpp"

// State Machine enum
enum MachineState {
    IDLE,
    HOMING,
    ALIGNMENT,
    CUTTING_CYCLE,
    RETURNING
};

// Global variables
extern MachineState currentState;
extern MachineState previousState;
extern float currentPosition;
extern bool homingComplete;
extern bool cycleInProgress;
extern bool emergencyStop;

// Timing variables
extern unsigned long stateStartTime;
extern unsigned long lastMotorUpdate;

// Input objects
extern Bounce2::Button homeSwitch;
extern Bounce2::Button startButton;
extern Bounce2::Button transferArmSignal;

// Function declarations
void setup();
void loop();

// State machine functions
void runStateMachine();
void changeState(MachineState newState);

// Motor control functions
void setupMotor();
void moveMotor(float steps, float speed, float acceleration);
void stopMotor();
void enableMotor();
void disableMotor();
void setDirection(bool forward);

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
void signalTransferArm();

// Utility functions
void setupInputs();
void updateInputs();
bool checkInputs();
float inchesToSteps(float inches);
float stepsToInches(float steps);

// Communication functions
void setupWiFi();
void sendStatus();

// OTA functions
void setupOTA();
void handleOTA();
bool isOTAInProgress();

// State handler function declarations
void handleIdleState();
void handleHomingState();
void handleAlignmentState();
void handleCuttingCycleState();
void handleReturningState(); 