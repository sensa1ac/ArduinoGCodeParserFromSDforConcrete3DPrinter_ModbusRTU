#include "modbus_config.h"
#include "pins_arduino.h"

SoftwareSerial rs485(RS485_RX, RS485_TX);
ModbusMaster node;

void preTransmission() {
  digitalWrite(DE_RE_PIN, HIGH);
}

void postTransmission() {
  digitalWrite(DE_RE_PIN, LOW);
}

void setupModbus() {
  Serial.begin(SOFTWARESERIAL_SPEED);
  while (!Serial);

  digitalWrite(DE_RE_PIN, LOW);
  rs485.begin(MODBUS_SPEED);

  node.begin(1, rs485);
  node.setTimeout(MODBUS_TIMEOUT);
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);
}