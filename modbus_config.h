#ifndef MODBUS_CONFIG_H
#define MODBUS_CONFIG_H

#include <ModbusMaster.h>
#include <SoftwareSerial.h>

#define SOFTWARESERIAL_SPEED 115200
// Скорость по шине
#define MODBUS_SPEED 38400
// Таймаут слэйва
#define MODBUS_TIMEOUT 50
// Задержка между опросами слейвов (мс)
#define MODBUS_POLL_DELAY 10 // при <9 пропускает ответы слэйвов на 19200
// Максимальное количество последовательных ошибок ответа от слейва
#define MODBUS_MAX_FAILS 2
///
#define STARTCAP_OFF_TIME 500
///
#define UPPER_Z_LIMIT 1000000000
///
#define DELAY_BEFORE_CONCRETE_SUPPLY 1000
///
#define HOMING_TIMEOUT 100000000000000000
//
const uint32_t LIFTING_Z_BEFORE_HOMING_INCASE_ENDS1 = 70;
//
const uint32_t STEPS_PER_POLL = 330; 
// Коэффициент: 1 срабатывание датчика Холла = 1.5 мм => 1/0.64=1.5625   
const float HALL_COUNTS_PER_MM = 0.5; // 0.7 было круто
/// Ноль - нога 2. X - нога 3. Y - нога 1
static uint32_t initialPositionOffset = 0; // в оборотах вент вентилятора!  
///
static float currentX = 0.0;
static float currentY = 0.0;
static float currentZ = 0.0;
///
const uint32_t X_DELAY = 250;
const uint32_t Y_DELAY = 250;
const uint32_t STEPS_PER_MM = 33;

extern SoftwareSerial rs485;
extern ModbusMaster node;

extern bool posReset[4];
extern uint8_t pollCount[4];
extern uint32_t lastCount[4];
extern uint32_t hallCounts[4];
extern bool isHoming; // Флаг хоминга

void preTransmission();
void postTransmission();
void setupModbus();

#endif