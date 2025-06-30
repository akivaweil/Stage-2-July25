//* ************************************************************************
//* ************************ HARDWARE CONTROL **************************
//* ************************************************************************
// Functions to control motor, clamps, cylinders, and other hardware

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
    // Motor is already configured in setupHardware()
    // Additional motor controller initialization could go here
    Serial.println("Motor controller ready");
}

void enableMotor() {
    digitalWrite(Pins::ENABLE, LOW); // Enable motor (active low)
}

void disableMotor() {
    digitalWrite(Pins::ENABLE, HIGH); // Disable motor (active low)
}

void setDirection(bool forward) {
    digitalWrite(Pins::DIR, forward ? HIGH : LOW);
}

void moveMotor(float steps, float speed, float acceleration) {
    // This is a simplified move function
    // In a real implementation, this would interface with a stepper driver
    // that handles acceleration, deceleration, and precise timing
    
    Serial.print("Moving motor: ");
    Serial.print(steps);
    Serial.print(" steps at ");
    Serial.print(speed);
    Serial.println(" steps/sec");
    
    // For now, just update timing for state machine
    // Real implementation would start hardware motion controller
}

void stopMotor() {
    // Stop any ongoing motor motion
    // In real implementation, would send stop command to motor controller
    Serial.println("Motor stopped");
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