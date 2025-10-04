#include "gcode_parser.h"
#include "sd_card.h"

static bool parserInitialized = false;

void initGCodeParser() {
  if (!parserInitialized) {
    initSDCard(); // Инициализируем SD-карту
    parserInitialized = true;
  }
}

bool getNextGCodeCommand(GCodeCommand* cmd) {
  String line = getNextGCodeLine();
  if (line == "") {
    // Перематываем файл в начало для следующей печати
    if (gcodeFile) {
      gcodeFile.seek(0);
    }
    return false;
  }
  return parseGCodeLine(line, cmd);
}

bool parseGCodeLine(String line, GCodeCommand* cmd) {
  // Сбрасываем поля структуры cmd
  cmd->type = 0;
  cmd->number = -1;
  cmd->x = NAN;
  cmd->y = NAN;
  cmd->z = NAN;
  cmd->f = NAN;
  cmd->concrete = false;
  cmd->homing = false;

  // Удаляем пробелы
  line.trim();
  if (line.length() == 0 || line.startsWith(";")) {
    return false;
  }

  // Проверяем начало строки ('G' или 'M')
  if (line[0] == 'G' || line[0] == 'M') {
    cmd->type = line[0];
    int spaceIdx = line.indexOf(' ');
    String numStr = (spaceIdx == -1) ? line.substring(1) : line.substring(1, spaceIdx);
    cmd->number = numStr.toInt();
  } else {
    return false;
  }

  // Обрабатываем G28
  if (cmd->type == 'G' && cmd->number == 28) {
    cmd->homing = true;
    cmd->x = 0.0;
    cmd->y = 0.0;
    cmd->z = NAN;
  }

  // Парсим параметры
  int pos = line.indexOf(' ') + 1;
  while (pos > 0 && pos < line.length()) {
    char param = line[pos];
    String valueStr = "";
    pos++;
    while (pos < line.length() && line[pos] != ' ') {
      valueStr += line[pos];
      pos++;
    }
    float value = valueStr.toFloat();

    switch (param) {
      case 'X': cmd->x = value; break;
      case 'Y': cmd->y = value; break;
      case 'Z': cmd->z = value; break;
      case 'F': cmd->f = value; break;
    }
    pos = line.indexOf(' ', pos) + 1;
  }

  // Обрабатываем M-команды
  if (cmd->type == 'M') {
    if (cmd->number == 3) {
      cmd->concrete = true;
    } else if (cmd->number == 5) {
      cmd->concrete = false;
    }
  }

  return true;
}