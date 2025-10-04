#include "hall_sensor.h"
#include "printer_state.h"

uint32_t hallCounts[4] = {0, 0, 0, 0};
static uint32_t lastHallCounts[4] = {0, 0, 0, 0};
static uint32_t hallPosition[4] = {0, 0, 0, 0};
static uint32_t endSensorCounts[4] = {0, 0, 0, 0};
static uint8_t slaveFailCount[4] = {0, 0, 0, 0};
bool motorStopped[4] = {false, false, false, false};

void pollHallSensors(bool isMovingUp) {
  unsigned long startCycle = millis();

  for (uint8_t slaveID = 1; slaveID <= 4; slaveID++) {
    Serial.print("Slave ");
    Serial.print(slaveID);
    Serial.print(": ");

    node.begin(slaveID, rs485);
    uint8_t result = node.readHoldingRegisters(0x00, 4);

    uint8_t motorIndex = slaveID - 1;
    if (result == node.ku8MBSuccess) {
      slaveFailCount[motorIndex] = 0;

      endSensorCounts[motorIndex] = ((uint32_t)node.getResponseBuffer(0) << 16) | node.getResponseBuffer(1);
      uint32_t count = ((uint32_t)node.getResponseBuffer(2) << 16) | node.getResponseBuffer(3);
      hallCounts[motorIndex] = count;

      if (!motorStopped[motorIndex]) {
        int32_t delta = count - lastHallCounts[motorIndex];
        if (delta != 0) {
          if (isMovingUp) {
            if (hallPosition[motorIndex] <= UPPER_Z_LIMIT - delta) {
              hallPosition[motorIndex] += delta;
            } else {
              hallPosition[motorIndex] = UPPER_Z_LIMIT;
            }
          } else {
            hallPosition[motorIndex] = (hallPosition[motorIndex] > delta) ? hallPosition[motorIndex] - delta : 0;
          }
        }
        lastHallCounts[motorIndex] = count;
      }

      Serial.print("EndS=");
      Serial.print(endSensorCounts[motorIndex]);
      Serial.print(", Count=");
      Serial.print(hallCounts[motorIndex]);
      Serial.print(", Pos=");
      Serial.println(getHallPosition(motorIndex));
    } else {
      slaveFailCount[motorIndex]++;
      Serial.println("N/A");
      if (slaveFailCount[motorIndex] >= MODBUS_MAX_FAILS) {
        enterEmergencyState(slaveID, "не ответил 2 раза");
        return;
      }
    }
    delay(MODBUS_POLL_DELAY);
  }

  unsigned long cycleTime = millis() - startCycle;
  Serial.print("Cycle time: ");
  Serial.print(cycleTime);
  Serial.println(" ms");
}

uint32_t getHallCount(uint8_t motorID) {
  return hallCounts[motorID];
}

float getHallPosition(uint8_t motorID) {
  return (float)hallPosition[motorID] / HALL_COUNTS_PER_MM;
}

uint32_t getEndSensorCount(uint8_t motorID) {
  return endSensorCounts[motorID];
}

void resetHallCounts(uint32_t initialPosition) {
  for (uint8_t i = 0; i < 4; i++) {
    hallCounts[i] = initialPosition * HALL_COUNTS_PER_MM;
    lastHallCounts[i] = hallCounts[i];
    hallPosition[i] = initialPosition * HALL_COUNTS_PER_MM;
    endSensorCounts[i] = 0;
    slaveFailCount[i] = 0;
    motorStopped[i] = false;
  }
}

void enterEmergencyState(uint8_t slaveID, const char* reason) {
  setPrinterState(PrinterState::EMERGENCY);
  Serial.print("ЧС: слейв ");
  Serial.print(slaveID);
  Serial.print(": ");
  Serial.println(reason);
}