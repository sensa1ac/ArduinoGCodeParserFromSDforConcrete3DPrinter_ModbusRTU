#ifndef SD_CARD_H
#define SD_CARD_H

#include <Arduino.h>
#include <SD.h>

extern File gcodeFile;

void initSDCard();
String getNextGCodeLine();

#endif