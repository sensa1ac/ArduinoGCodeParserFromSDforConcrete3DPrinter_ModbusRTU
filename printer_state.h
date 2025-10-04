#ifndef PRINTER_STATE_H
#define PRINTER_STATE_H

#include <Arduino.h>

// Перечисление состояний принтера
enum class PrinterState {
  IDLE,
  PRINTING,
  PAUSED,
  EMERGENCY
};

// Установка состояния
void setPrinterState(PrinterState state);

// Получение текущего состояния
PrinterState getPrinterState();

#endif