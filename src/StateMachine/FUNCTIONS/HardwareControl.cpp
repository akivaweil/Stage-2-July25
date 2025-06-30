//* ************************************************************************
//* ************************ HARDWARE CONTROL **************************
//* ************************************************************************
// Functions to control motor, clamps, cylinders, and other hardware using FastAccelStepper

#include <Stage2_Machine.h>

//* ************************************************************************
//* ************************ HARDWARE SETUP ****************************
//* ************************************************************************

void setupHardware() {
    Serial.println("Initializing hardware...");
    
    // Configure output pins
    pinMode(Pins::STEP, OUTPUT);
    pinMode(Pins::DIR, OUTPUT);
    pinMode(Pins::ENABLE, OUTPUT);
    pinMode(Pins::LEFT_CLAMP, OUTPUT);
    pinMode(Pins::RIGHT_CLAMP, OUTPUT);
    pinMode(Pins::ALIGN_CYLINDER, OUTPUT);
    pinMode(Pins::TRANSFER_ARM_SIGNAL, OUTPUT);
    
    // Set initial states - all outputs OFF
    digitalWrite(Pins::STEP, LOW);
    digitalWrite(Pins::DIR, LOW);
    digitalWrite(Pins::ENABLE, HIGH); // Motor disabled (active low)
    digitalWrite(Pins::LEFT_CLAMP, LOW);
    digitalWrite(Pins::RIGHT_CLAMP, LOW);
    digitalWrite(Pins::ALIGN_CYLINDER, LOW);
    digitalWrite(Pins::TRANSFER_ARM_SIGNAL, LOW);
    
    Serial.println("Hardware initialization complete");
}

//* ************************************************************************
//* ************************ MOTOR CONTROL ******************************
//* ************************************************************************

void setupMotor() {
    Serial.println("Initializing FastAccelStepper...");
    
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
        
        Serial.println("FastAccelStepper initialized successfully");
    } else {
        Serial.println("ERROR: Failed to initialize FastAccelStepper!");
    }
}

void enableMotor() {
    if (stepper) {
        stepper->enableOutputs();
        Serial.println("Motor enabled");
    }
}

void disableMotor() {
    if (stepper) {
        stepper->disableOutputs();
        Serial.println("Motor disabled");
    }
}

void setDirection(bool forward) {
    if (stepper) {
        // FastAccelStepper handles direction automatically with move commands
        // This function is kept for compatibility but direction is handled in move functions
    }
}

void moveMotor(float steps, float speed, float acceleration) {
    if (!stepper) {
        Serial.println("ERROR: Stepper not initialized!");
        return;
    }
    
    // Set speed and acceleration
    stepper->setSpeedInHz(speed);
    stepper->setAcceleration(acceleration);
    
    // Execute relative move
    stepper->move((long)steps);
    
    Serial.print("Moving motor: ");
    Serial.print(steps);
    Serial.print(" steps at ");
    Serial.print(speed);
    Serial.print(" Hz with accel ");
    Serial.println(acceleration);
}

void moveMotorToPosition(float targetSteps, float speed, float acceleration) {
    if (!stepper) {
        Serial.println("ERROR: Stepper not initialized!");
        return;
    }
    
    // Set speed and acceleration
    stepper->setSpeedInHz(speed);
    stepper->setAcceleration(acceleration);
    
    // Execute absolute move
    stepper->moveTo((long)targetSteps);
    
    // Update current position tracking
    currentPosition = targetSteps;
    
    Serial.print("Moving motor to position: ");
    Serial.print(targetSteps);
    Serial.print(" steps at ");
    Serial.print(speed);
    Serial.print(" Hz with accel ");
    Serial.println(acceleration);
}

void stopMotor() {
    if (stepper) {
        stepper->stopMove();
        Serial.println("Motor stopped");
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
            delay(1); // Small delay to prevent watchdog issues
        }
    }
}

//* ************************************************************************
//* ************************ ADVANCED MOTOR CONTROL *********************
//* ************************************************************************

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
        Serial.print("Motor position set to: ");
        Serial.println(position);
    }
}

//* ************************************************************************
//* ************************ CLAMP CONTROL FUNCTIONS ********************
//* ************************************************************************
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

//* ************************************************************************
//* ************************ ALIGNMENT CYLINDER FUNCTIONS ***************
//* ************************************************************************
// Controls for alignment cylinder (LOW = retracted, HIGH = extended)

void extendAlignmentCylinder() {
    digitalWrite(Pins::ALIGN_CYLINDER, HIGH);  // HIGH extends the cylinder
}

void retractAlignmentCylinder() {
    digitalWrite(Pins::ALIGN_CYLINDER, LOW);   // LOW retracts the cylinder
}

//* ************************************************************************
//* ************************ COMMUNICATION ******************************
//* ************************************************************************

void signalTransferArm() {
    // Send signal to transfer arm that cycle is complete
    digitalWrite(Pins::TRANSFER_ARM_SIGNAL, HIGH);
    delay(100); // Brief pulse
    digitalWrite(Pins::TRANSFER_ARM_SIGNAL, LOW);
    Serial.println("Transfer arm signaled");
} 