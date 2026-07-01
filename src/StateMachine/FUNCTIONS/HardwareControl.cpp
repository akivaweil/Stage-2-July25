// Hardware control
// Functions to control motor, clamps, cylinders, and other hardware using FastAccelStepper

#include "StateMachine/StateMachine.h"
#include <esp_task_wdt.h>

// Hardware setup

void setupHardware() {
    // Configure output pins
    pinMode(Pins::STEP, OUTPUT);
    pinMode(Pins::DIR, OUTPUT);
    pinMode(Pins::ENABLE, OUTPUT);
    pinMode(Pins::LEFT_CLAMP, OUTPUT);
    pinMode(Pins::RIGHT_CLAMP, OUTPUT);
    pinMode(Pins::ALIGN_CYLINDER, OUTPUT);
    pinMode(Pins::ROUTER_START_SIGNAL, OUTPUT);   // physical signal sent with ESP-NOW router start
    
    // Set initial states - all outputs OFF
    digitalWrite(Pins::STEP, LOW);
    digitalWrite(Pins::DIR, LOW);
    digitalWrite(Pins::ENABLE, HIGH); // Motor disabled (active low)
    digitalWrite(Pins::LEFT_CLAMP, LOW);
    digitalWrite(Pins::RIGHT_CLAMP, LOW);
    digitalWrite(Pins::ALIGN_CYLINDER, LOW);
    digitalWrite(Pins::ROUTER_START_SIGNAL, LOW);   // start LOW
}

// Motor control

void setupMotor() {
    // Validate pin numbers first
    if (Pins::STEP < 0 || Pins::STEP > 48) {
        Serial.println("[Stage2] FAULT: invalid STEP pin");
        return;
    }
    if (Pins::DIR < 0 || Pins::DIR > 48) {
        Serial.println("[Stage2] FAULT: invalid DIR pin");
        return;
    }
    if (Pins::ENABLE < 0 || Pins::ENABLE > 48) {
        Serial.println("[Stage2] FAULT: invalid ENABLE pin");
        return;
    }

    // Initialize the stepper engine
    engine.init();

    // Create stepper instance
    stepper = engine.stepperConnectToPin(Pins::STEP);
    if (stepper) {
        // Set direction and enable pins
        stepper->setDirectionPin(Pins::DIR);
        stepper->setEnablePin(Pins::ENABLE);

        // Set motor parameters
        stepper->setSpeedInHz(Motion::HOMING_SPEED);        // Default speed
        stepper->setAcceleration(Motion::FORWARD_ACCEL);    // Default acceleration

        // Disable motor initially
        stepper->disableOutputs();
    } else {
        Serial.println("[Stage2] FAULT: stepper instance creation failed");
    }
}

void enableMotor() {
    if (stepper) {
        stepper->enableOutputs();
    }
}

void disableMotor() {
    if (stepper) {
        stepper->disableOutputs();
    }
}

void moveMotor(float steps, float speed, float acceleration) {
    if (!stepper) {
        return;
    }
    
    // Set speed and acceleration
    stepper->setSpeedInHz(speed);
    stepper->setAcceleration(acceleration);
    
    // Execute relative move
    stepper->move((long)steps);
}

void moveMotorToPosition(float targetSteps, float speed, float acceleration) {
    if (!stepper) {
        return;
    }
    
    // Set speed and acceleration
    stepper->setSpeedInHz(speed);
    stepper->setAcceleration(acceleration);
    
    // Execute absolute move
    stepper->moveTo((long)targetSteps);
    
    // Update current position tracking
    currentPosition = targetSteps;
}

void stopMotor() {
    if (stepper) {
        stepper->stopMove();
    }
}

bool isMotorRunning() {
    if (stepper) {
        return stepper->isRunning();
    }
    return false;
}

void waitForMotorComplete() {
    if (stepper) {
        while (stepper->isRunning()) {
            esp_task_wdt_reset();  // blocking wait — keep the task watchdog fed
            delay(1); // Small delay to prevent watchdog issues
        }
    }
}

// Advanced motor control

// Get current motor position from FastAccelStepper
long getCurrentMotorPosition() {
    if (stepper) {
        return stepper->getCurrentPosition();
    }
    return 0;
}

// Set current motor position (useful for homing)
void setCurrentMotorPosition(long position) {
    if (stepper) {
        stepper->setCurrentPosition(position);
        currentPosition = (float)position;
    }
}

// Clamp control functions
// Controls for left and right clamps (LOW = extended, HIGH = retracted)

void extendLeftClamp() {
    digitalWrite(Pins::LEFT_CLAMP, LOW);  // LOW extends the clamp
}

void retractLeftClamp() {
    digitalWrite(Pins::LEFT_CLAMP, HIGH);  // HIGH retracts the clamp
}

void extendRightClamp() {
    digitalWrite(Pins::RIGHT_CLAMP, LOW);  // LOW extends the clamp
}

void retractRightClamp() {
    digitalWrite(Pins::RIGHT_CLAMP, HIGH);  // HIGH retracts the clamp
}

void extendBothClamps() {
    digitalWrite(Pins::LEFT_CLAMP, LOW);   // LOW extends the clamp
    digitalWrite(Pins::RIGHT_CLAMP, LOW);  // LOW extends the clamp
}

void retractBothClamps() {
    digitalWrite(Pins::LEFT_CLAMP, HIGH);   // HIGH retracts the clamp
    digitalWrite(Pins::RIGHT_CLAMP, HIGH);  // HIGH retracts the clamp
}

// Alignment cylinder functions
// Controls for alignment cylinder (LOW = retracted, HIGH = extended)

void extendAlignmentCylinder() {
    digitalWrite(Pins::ALIGN_CYLINDER, HIGH);  // HIGH extends the cylinder
}

void retractAlignmentCylinder() {
    digitalWrite(Pins::ALIGN_CYLINDER, LOW);   // LOW retracts the cylinder
}
