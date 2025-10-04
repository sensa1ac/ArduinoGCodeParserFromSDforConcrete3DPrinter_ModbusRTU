#include "motion_control.h"
#include "printer_state.h"
#include "hall_sensor.h"

void executeGCodeCommand(GCodeCommand cmd) {
  Serial.print("Received command: ");
  Serial.print(cmd.type);
  Serial.print(cmd.number);
  Serial.print(" X=");
  Serial.print(cmd.x);
  Serial.print(" Y=");
  Serial.print(cmd.y);
  Serial.print(" Z=");
  Serial.print(cmd.z);
  Serial.print(" F=");
  Serial.println(cmd.f);

  if (cmd.type == 'G') {
    if (cmd.number == 0 || cmd.number == 1) {
      float targetX = isnan(cmd.x) ? currentX : cmd.x;
      float targetY = isnan(cmd.y) ? currentY : cmd.y;
      float targetZ = isnan(cmd.z) ? currentZ : cmd.z;
      float feedrate = isnan(cmd.f) ? 100.0 : cmd.f;
      moveTo(targetX, targetY, targetZ, feedrate);
    } else if (cmd.number == 28) {
      Serial.println("Calling homeAxes");
      homeAxes();
      Serial.println("homeAxes completed");
    }
  } else if (cmd.type == 'M') {
    if (cmd.number == 3) {
      startConcreteSupply();
      delay(DELAY_BEFORE_CONCRETE_SUPPLY);
      Serial.println("Delay after M3 completed");
    } else if (cmd.number == 5) {
      stopConcreteSupply();
    }
  }
}

void homeAxes() {
  Serial.println("Хоминг ...");
  Serial.println("Движение к X=0, Y=0");
  moveTo(0.0, 0.0, currentZ, 100.0);

  Serial.println("Подготовка хоминга по Z...");
  unsigned long homingStart = millis();

  pollHallSensors(false);
  bool allEndstopsZero = true;
  uint32_t totalHallCounts = 0;
  for (uint8_t i = 0; i < 4; i++) {
    totalHallCounts += getHallCount(i);
    if (getEndSensorCount(i) >= 1) {
      allEndstopsZero = false;
      Serial.print("Слейв ");
      Serial.print(i + 1);
      Serial.println(" концевик сработал (EndS>=1)");
    }
  }

  if (!allEndstopsZero && totalHallCounts == 0) {
    Serial.println("Подъем Z на 80 мм");
    moveTo(currentX, currentY, currentZ + LIFTING_Z_BEFORE_HOMING_INCASE_ENDS1, 100.0);
    Serial.println("Ожидание 2 секунд после подъема");
    delay(2000);
  }

  Serial.println("Хоминг по Z...");
  startMotorsDown();
  delay(500);
  uint32_t initialEndCounts[4];
  pollHallSensors(false);
  for (uint8_t i = 0; i < 4; i++) {
    initialEndCounts[i] = getEndSensorCount(i);
    Serial.print("Слейв ");
    Serial.print(i + 1);
    Serial.print(" начальный EndS=");
    Serial.println(initialEndCounts[i]);
  }

  bool homed[4] = {false, false, false, false};
  while (!(homed[0] && homed[1] && homed[2] && homed[3])) {
    pollHallSensors(false);
    checkCapacitorTimers();

    for (uint8_t i = 0; i < 4; i++) {
      if (!homed[i] && getEndSensorCount(i) >= initialEndCounts[i] + 1) {
        stopMotor(i + 1);
        motorStopped[i] = true;
        homed[i] = true;
        Serial.print("Слейв ");
        Serial.print(i + 1);
        Serial.println(" хоминг по Z завершен (концевик сработал)");
      }
    }

    if (millis() - homingStart > 30000) {
      Serial.println("Таймаут хоминга");
      for (uint8_t i = 0; i < 4; i++) {
        if (!homed[i]) {
          stopMotor(i + 1);
          motorStopped[i] = true;
        }
      }
      enterEmergencyState(0, "Таймаут хоминга по Z");
      return;
    }
  }

  Serial.println("Ожидание 2 секунд для стабилизации");
  delay(2000);

  resetHallCounts(0);
  currentX = 0.0;
  currentY = 0.0;
  currentZ = 0.0;
  Serial.println("Хоминг завершен: X=0, Y=0, Z=0");
}

void moveTo(float x, float y, float z, float feedrate) {
  if (z != currentZ) {
    bool allMotorsStopped = false;
    bool motorStoppedLocal[4] = {false, false, false, false};
    unsigned long moveStart = millis();

    for (uint8_t i = 0; i < 4; i++) {
      motorStopped[i] = false;
    }

    if (z > currentZ) {
      startMotorsUp();
      delay(500);
      while (!allMotorsStopped && (millis() - moveStart < 30000)) {
        pollHallSensors(true);
        checkCapacitorTimers();
        allMotorsStopped = true;
        for (uint8_t i = 0; i < 4; i++) {
          if (!motorStoppedLocal[i]) {
            float currentPos = getHallPosition(i);
            if (currentPos >= z) {
              stopMotor(i + 1);
              motorStoppedLocal[i] = true;
              motorStopped[i] = true;
              Serial.print("Мотор ");
              Serial.print(i + 1);
              Serial.println(" остановлен (вверх)");
            } else {
              allMotorsStopped = false;
            }
          }
        }
      }
    } else {
      startMotorsDown();
      delay(500);
      while (!allMotorsStopped && (millis() - moveStart < 30000)) {
        pollHallSensors(false);
        checkCapacitorTimers();
        allMotorsStopped = true;
        for (uint8_t i = 0; i < 4; i++) {
          if (!motorStoppedLocal[i]) {
            float currentPos = getHallPosition(i);
            if (currentPos <= z) {
              stopMotor(i + 1);
              motorStoppedLocal[i] = true;
              motorStopped[i] = true;
              Serial.print("Мотор ");
              Serial.print(i + 1);
              Serial.println(" остановлен (вниз)");
            } else {
              allMotorsStopped = false;
            }
          }
        }
      }
    }

    if (!allMotorsStopped) {
      for (uint8_t i = 0; i < 4; i++) {
        if (!motorStoppedLocal[i]) {
          stopMotor(i + 1);
          motorStopped[i] = true;
        }
      }
      enterEmergencyState(0, "Таймаут движения по Z");
      return;
    }

    // Сбрасываем hallPosition для соответствия целевой Z
    for (uint8_t i = 0; i < 4; i++) {
      hallPosition[i] = z * HALL_COUNTS_PER_MM;
      lastHallCounts[i] = hallCounts[i];
      Serial.print("Слейв ");
      Serial.print(i + 1);
      Serial.print(" позиция установлена на Z=");
      Serial.println(z);
    }
    currentZ = z;
  }

  int32_t stepsX = (x - currentX) * STEPS_PER_MM;
  int32_t stepsY = (y - currentY) * STEPS_PER_MM;
  int32_t maxSteps = max(abs(stepsX), abs(stepsY));

  digitalWrite(DIR_X, stepsX >= 0 ? LOW : HIGH);
  digitalWrite(DIR_Y, stepsY >= 0 ? LOW : HIGH);

  for (int32_t i = 0; i < maxSteps; i++) {
    if (i < abs(stepsX)) {
      digitalWrite(STEP_X, HIGH);
      delayMicroseconds(X_DELAY);
      digitalWrite(STEP_X, LOW);
      delayMicroseconds(X_DELAY);
    }
    if (i < abs(stepsY)) {
      digitalWrite(STEP_Y, HIGH);
      delayMicroseconds(Y_DELAY);
      digitalWrite(STEP_Y, LOW);
      delayMicroseconds(Y_DELAY);
    }
  }

  currentX = x;
  currentY = y;
}

void startConcreteSupply() {
  digitalWrite(CONCRETE_SUPPLY, LOW);
  Serial.println("Подача бетона ВКЛ");
}

void stopConcreteSupply() {
  digitalWrite(CONCRETE_SUPPLY, HIGH);
  Serial.println("Подача бетона ВЫКЛ");
}