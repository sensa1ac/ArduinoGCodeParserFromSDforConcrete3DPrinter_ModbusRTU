#include "motor_control.h"
#include "pins_arduino.h"

// Таймеры для отключения конденсаторов
static unsigned long capacitorTimer = 0;
static bool capacitorDisablePending = false;

void startMotorsUp() {
  // Включение двигателей вверх
  digitalWrite(SSR12, LOW);
  digitalWrite(SSR22, LOW);
  digitalWrite(SSR32, LOW);
  digitalWrite(SSR42, LOW);
  enableStartCapacitors();
  digitalWrite(SSR11, HIGH);
  digitalWrite(SSR21, HIGH);
  digitalWrite(SSR31, HIGH);
  digitalWrite(SSR41, HIGH);

  // Запуск таймера для отключения конденсаторов
  capacitorTimer = millis();
  capacitorDisablePending = true;
  Serial.println("startMotorsUp called, capacitor timer started");
}

void startMotorsDown() {
  // Включение двигателей вниз
  digitalWrite(SSR11, LOW);
  digitalWrite(SSR21, LOW);
  digitalWrite(SSR31, LOW);
  digitalWrite(SSR41, LOW);
  enableStartCapacitors();
  digitalWrite(SSR12, HIGH);
  digitalWrite(SSR22, HIGH);
  digitalWrite(SSR32, HIGH);
  digitalWrite(SSR42, HIGH);

  // Запуск таймера для отключения конденсаторов
  capacitorTimer = millis();
  capacitorDisablePending = true;
  Serial.println("startMotorsDown called, capacitor timer started");
}

void stopMotor(uint8_t motorID) {
  switch (motorID) {
    case 1:
      digitalWrite(SSR11, LOW);
      digitalWrite(SSR12, LOW);
      break;
    case 2:
      digitalWrite(SSR21, LOW);
      digitalWrite(SSR22, LOW);
      break;
    case 3:
      digitalWrite(SSR31, LOW);
      digitalWrite(SSR32, LOW);
      break;
    case 4:
      digitalWrite(SSR41, LOW);
      digitalWrite(SSR42, LOW);
      break;
  }
}

void disableStartCapacitors() {
  digitalWrite(SSR8_1, LOW);
  digitalWrite(SSR7_2, LOW);
  digitalWrite(SSR5_3, LOW);
  digitalWrite(SSR6_4, LOW);
  capacitorDisablePending = false; // Сбрасываем флаг
  Serial.println("disableStartCapacitors called");
}

void enableStartCapacitors() {
  digitalWrite(SSR8_1, HIGH);
  digitalWrite(SSR7_2, HIGH);
  digitalWrite(SSR5_3, HIGH);
  digitalWrite(SSR6_4, HIGH);
  Serial.println("enableStartCapacitors called");
}

void checkCapacitorTimers() {
  // Проверяем, активен ли таймер и прошло ли STARTCAP_OFF_TIME мс
  if (capacitorDisablePending && millis() - capacitorTimer >= STARTCAP_OFF_TIME) {
    disableStartCapacitors();
    Serial.println("Пусковые конденсаторы отключены");
  }
}