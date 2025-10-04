#ifndef GCODE_PARSER_H
#define GCODE_PARSER_H

#include <Arduino.h>

struct GCodeCommand {
  char type;
  int number;
  float x, y, z;
  float f;
  bool concrete;
  bool homing;
};

void initGCodeParser();
bool getNextGCodeCommand(GCodeCommand* cmd);
bool parseGCodeLine(String line, GCodeCommand* cmd);

#endif