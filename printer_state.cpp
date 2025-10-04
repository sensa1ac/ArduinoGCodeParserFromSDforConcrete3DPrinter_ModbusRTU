#include "printer_state.h"
#include "motor_control.h"

static PrinterState currentState = PrinterState::IDLE;

void setPrinterState(PrinterState state) {
  if (state == PrinterState::EMERGENCY) {
    // Остановка всех двигателей
    for (uint8_t i = 1; i <= 4; i++) {
      stopMotor(i);
    }
    Serial.println("Emergency state activated");
  }
  currentState = state;
}

PrinterState getPrinterState() {
  return currentState;
}