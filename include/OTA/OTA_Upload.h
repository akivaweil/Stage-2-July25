#pragma once
#include <Arduino.h>

void setupOTA();
void handleOTA();
void sendRouterSignal(uint8_t value);

extern volatile bool isRouterClear;
