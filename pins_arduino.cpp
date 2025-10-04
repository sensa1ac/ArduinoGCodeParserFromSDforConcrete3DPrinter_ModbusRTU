#include "pins_arduino.h"
#include "modbus_config.h"

void setupArduinoPins() {
  pinMode(SSR11, OUTPUT);
  digitalWrite(SSR11, LOW);
  pinMode(SSR12, OUTPUT);
  digitalWrite(SSR12, LOW);
  pinMode(SSR21, OUTPUT);
  digitalWrite(SSR21, LOW);
  pinMode(SSR22, OUTPUT);
  digitalWrite(SSR22, LOW);
  pinMode(SSR31, OUTPUT);
  digitalWrite(SSR31, LOW);
  pinMode(SSR32, OUTPUT);
  digitalWrite(SSR32, LOW);
  pinMode(SSR41, OUTPUT);
  digitalWrite(SSR41, LOW);
  pinMode(SSR42, OUTPUT);
  digitalWrite(SSR42, LOW);
  pinMode(SSR8_1, OUTPUT);
  digitalWrite(SSR8_1, HIGH);
  pinMode(SSR7_2, OUTPUT);
  digitalWrite(SSR7_2, HIGH);
  pinMode(SSR5_3, OUTPUT);
  digitalWrite(SSR5_3, HIGH);
  pinMode(SSR6_4, OUTPUT);
  digitalWrite(SSR6_4, HIGH);
  pinMode(DIR_Y, OUTPUT);
  pinMode(STEP_Y, OUTPUT);
  pinMode(DIR_X, OUTPUT);
  pinMode(STEP_X, OUTPUT);
  pinMode(CONCRETE_SUPPLY, OUTPUT);
  digitalWrite(CONCRETE_SUPPLY, HIGH);
  pinMode(DE_RE_PIN, OUTPUT);
  pinMode(SD_CS, OUTPUT); // Настраиваем SD_CS (пин 53)
  digitalWrite(SD_CS, HIGH); // Устанавливаем в HIGH (неактивное состояние)
}