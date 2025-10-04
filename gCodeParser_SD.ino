#include "gcode_parser.h"
#include "motion_control.h"
#include "printer_state.h"
#include "motor_control.h"
#include "hall_sensor.h"
#include "modbus_config.h"
#include "pins_arduino.h"
#include "sd_card.h"

void setup() {
  Serial.begin(115200);
  while (!Serial);

  setupArduinoPins();
  setupModbus();
  resetHallCounts(0);
  initGCodeParser();
  setPrinterState(PrinterState::IDLE);

  Serial.println("Принтер инициализирован");
}

void loop() {
  if (getPrinterState() != PrinterState::EMERGENCY) {
    pollHallSensors(getPrinterState() == PrinterState::PRINTING); // Исправлен вызов
  }

  checkCapacitorTimers();

  switch (getPrinterState()) {
    case PrinterState::IDLE:
      setPrinterState(PrinterState::PRINTING);
      Serial.println("Начало печати");
      break;

    case PrinterState::PRINTING:
      GCodeCommand cmd;
      if (getNextGCodeCommand(&cmd)) {
        executeGCodeCommand(cmd);
        Serial.print("Выполняется: ");
        Serial.print(cmd.type);
        Serial.print(cmd.number);
        Serial.print(" X=");
        Serial.print(cmd.x);
        Serial.print(" Y=");
        Serial.print(cmd.y);
        Serial.print(" Z=");
        Serial.print(cmd.z);
        Serial.print(" F=");
        Serial.print(cmd.f);
        Serial.print(" Concrete=");
        Serial.println(cmd.concrete);
      } else {
        setPrinterState(PrinterState::EMERGENCY);
        stopConcreteSupply();
        Serial.println("Печать завершена, переход в EMERGENCY");
      }
      break;

    case PrinterState::PAUSED:
      break;

    case PrinterState::EMERGENCY:
      break;
  }
}