#include "sd_card.h"
#include "pins_arduino.h"
#include <SPI.h>

File gcodeFile;

void initSDCard() {
  Serial.print("Инициализация SD-карты... Пин CS=");
  Serial.println(SD_CS);
  if (!SD.begin(SD_CS)) {
    Serial.println("Ошибка инициализации SD-карты!");
    while (1);
  }
  Serial.println("SD-карта инициализирована.");

  gcodeFile = SD.open("gcode.txt");
  if (!gcodeFile) {
    Serial.println("Ошибка открытия gcode.txt!");
    while (1);
  }
  Serial.println("Файл gcode.txt открыт");
}

String getNextGCodeLine() {
  String line = "";
  if (gcodeFile.available()) {
    line = gcodeFile.readStringUntil('\n');
    line.trim();
  } else {
    gcodeFile.close();
  }
  return line;
}