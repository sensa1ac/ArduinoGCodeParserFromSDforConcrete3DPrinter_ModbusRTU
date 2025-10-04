#ifndef MOTION_CONTROL_H
#define MOTION_CONTROL_H

#include "gcode_parser.h"
#include "motor_control.h"
#include "hall_sensor.h"
#include "pins_arduino.h"


// Выполнение команды G-кода
void executeGCodeCommand(GCodeCommand cmd);

// Перемещение к заданной позиции
void moveTo(float x, float y, float z, float feedrate);

// Возврат в нулевую позицию (G28)
void homeAxes();

// Управление подачей бетона
void startConcreteSupply();
void stopConcreteSupply();

#endif