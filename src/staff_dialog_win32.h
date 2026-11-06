#ifndef ROAD_OF_LIFE_STAFF_DIALOG_WIN32_H
#define ROAD_OF_LIFE_STAFF_DIALOG_WIN32_H

#include <windows.h>
#include <vector>
#include "game.h"

bool showHireForm(HWND owner, int dpi, HFONT font, HFONT heading,
                  int limit, std::vector<Staff>& employees);
bool showDismissForm(HWND owner, int dpi, HFONT font, HFONT heading,
                     const std::vector<Staff>& employees, int& index);
bool showSalaryForm(HWND owner, int dpi, HFONT font, HFONT heading,
                    const std::vector<Staff>& employees, int& index, int& change);

#endif
