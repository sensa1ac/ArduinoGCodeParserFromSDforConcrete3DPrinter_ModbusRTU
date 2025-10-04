#ifndef HALL_SENSOR_H
#define HALL_SENSOR_H

#include "modbus_config.h"

void pollHallSensors(bool isMovingUp);
uint32_t getHallCount(uint8_t motorID);
float getHallPosition(uint8_t motorID);
uint32_t getEndSensorCount(uint8_t motorID);
void resetHallCounts(uint32_t initialPosition = 0);
void enterEmergencyState(uint8_t slaveID, const char* reason);

extern bool motorStopped[4];
extern uint32_t hallPosition[4]; // Добавляем extern для hallPosition
extern uint32_t lastHallCounts[4]; // Добавляем extern для lastHallCounts

#endif