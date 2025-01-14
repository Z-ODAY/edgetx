/*
 * Copyright (C) EdgeTX
 *
 * Based on code named
 *   opentx - https://github.com/opentx/opentx
 *   th9x - http://code.google.com/p/th9x
 *   er9x - http://code.google.com/p/er9x
 *   gruvin9x - http://code.google.com/p/gruvin9x
 *
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#include "file_carosell.h"

extern inline tmr10ms_t getTicks()
{
  return g_tmr10ms;
}

FileCarosell::FileCarosell(Window *parent, const rect_t &rect,
                           std::vector<std::string> fileNames) :
    FormGroup(parent, rect, NO_FOCUS | FORM_NO_BORDER),
    _fileNames(fileNames),
    fp(new FilePreview(this, {0, 0, rect.w, rect.h}, false))
{
  timer = getTicks();
  message = new StaticText(this, {0, rect.h/2, rect.w, PAGE_LINE_HEIGHT}, "", 0, CENTERED | FONT(L) | COLOR_THEME_PRIMARY1);
  setSelected(0);
}

void FileCarosell::setFileNames(std::vector<std::string> fileNames)
{
  _fileNames = fileNames;
  setSelected(-1);
  timer = getTicks();
  pageInterval = SHORT_PAGE_INTERVAL;
}

void FileCarosell::setSelected(int n)
{
  if (n != selected) {
    selected = n;

    if (n >= 0 && n < (int)_fileNames.size()) {
      fp->setFile(_fileNames[selected].c_str());
    } else
      fp->setFile("");
  }
  
  if (selected == -1) {
    lv_obj_clear_flag(message->getLvObj(), LV_OBJ_FLAG_HIDDEN);
    message->setText(_fileNames.size() > 0 ? STR_LOADING : STR_NO_THEME_IMAGE);
  } else {
    lv_obj_add_flag(message->getLvObj(), LV_OBJ_FLAG_HIDDEN);
  }
}

void FileCarosell::checkEvents()
{
  FormGroup::checkEvents();

  uint32_t newTicks = getTicks();

  // if we are paused then just update time.  we will begin the carosell after
  // timeout period once unpaused
  if (_paused) {
    timer = newTicks;
  } else if (newTicks - timer > pageInterval && _fileNames.size()) {
    int newSelected = (selected + 1) % _fileNames.size();
    setSelected(newSelected);
    timer = newTicks;
    pageInterval = PAGE_INTERVAL;
  }
}
