#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include "modbus_config.h"

// Запуск всех двигателей вверх
void startMotorsUp();

// Запуск всех двигателей вниз
void startMotorsDown();

// Запуск двигателя вниз для конкретного слейва
void startMotorDown(uint8_t motorID);

// Остановка конкретного двигателя
void stopMotor(uint8_t motorID);

// Отключение всех пусковых конденсаторов
void disableStartCapacitors();

// Включение всех пусковых конденсаторов
void enableStartCapacitors();

// Проверка таймеров для отключения конденсаторов
void checkCapacitorTimers();

#endif