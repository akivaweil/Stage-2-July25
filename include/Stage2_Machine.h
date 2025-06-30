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
    CUTTING_CYCLE,
    CLAMP_ENGAGE,
    ALIGN_ENGAGE,
    APPROACH_MOVE,
    CUTTING_MOVE,
    FINISH_MOVE,
    CLAMP_RELEASE,
    ALIGN_RELEASE,
    RETURN_MOVE,
    CYCLE_COMPLETE,
    ERROR_STATE
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
void executeStateMachine();
void changeState(MachineState newState);

// State functions
void state_IDLE();
void state_HOMING();
void state_CUTTING_CYCLE();
void state_CLAMP_ENGAGE();
void state_ALIGN_ENGAGE();
void state_APPROACH_MOVE();
void state_CUTTING_MOVE();
void state_FINISH_MOVE();
void state_CLAMP_RELEASE();
void state_ALIGN_RELEASE();
void state_RETURN_MOVE();
void state_CYCLE_COMPLETE();
void state_ERROR_STATE();

// Motor control functions
void setupMotor();
void moveMotor(float steps, float speed, float acceleration);
void stopMotor();
void enableMotor();
void disableMotor();
void setDirection(bool forward);

// Hardware control functions
void setupHardware();
void engageClamps();
void releaseClamps();
void engageAlignCylinder();
void releaseAlignCylinder();
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