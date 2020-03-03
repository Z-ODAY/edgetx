/*
 * Copyright (C) OpenTX
 *
 * Source:
 *  https://github.com/opentx/libopenui
 *
 * This file is a part of libopenui library.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 */

#ifndef _TABLE_H_
#define _TABLE_H_

#include "window.h"
#include <vector>
#include "libopenui_config.h"

class Table: public Window {
    class Line {
      public:
        explicit Line(uint8_t columnsCount):
          values(columnsCount)
        {
        }
        std::vector<std::string> values;
        std::function<void()> onPress;
    };

    class Header: public Window {
      public:
        Header(Table * parent, const rect_t & rect, uint8_t columnsCount):
          Window(parent, rect, OPAQUE),
          header(columnsCount),
          columnsWidth(parent->columnsWidth)
        {
        }

        void setLine(const Line & line)
        {
          header = line;
        }

        void paint(BitmapBuffer * dc) override;

      protected:
        Line header;
        std::vector<coord_t> & columnsWidth;
    };

    class Body: public Window {
      friend class Table;

      public:
        Body(Table * parent, const rect_t & rect):
          Window(parent, rect, OPAQUE),
          columnsWidth(parent->columnsWidth)
        {
        }

        void addLine(const Line & line)
        {
          lines.push_back(line);
          setInnerHeight(lines.size() * TABLE_LINE_HEIGHT - 2);
        }

        void clear()
        {
          lines.clear();
        }

        void paint(BitmapBuffer * dc) override;

#if defined(HARDWARE_TOUCH)
        bool onTouchEnd(coord_t x, coord_t y) override
        {
          unsigned index = y / TABLE_LINE_HEIGHT;
          if (index < lines.size()) {
            auto onPress = lines[index].onPress;
            if (onPress)
              onPress();
          }
          return true;
        }
#endif

      protected:
        std::vector<coord_t> & columnsWidth;
        std::vector<Line> lines;
        int selection = -1;
    };

  public:
    Table(Window * parent, const rect_t & rect, uint8_t columnsCount):
      Window(parent, rect),
      columnsCount(columnsCount),
      columnsWidth(columnsCount, width() / columnsCount),
      header(this, {0, 0, width(), 0}, columnsCount),
      body(this, {0, 0, width(), height()})
    {
    }

    ~Table() override
    {
      header.detach();
      body.detach();
    }

    void setColumnsWidth(const coord_t width[])
    {
      for (uint8_t i = 0; i < columnsCount; i++) {
        columnsWidth[i] = width[i];
      }
    }

    int getSelection() const
    {
      return body.selection;
    }

    void clearSelection()
    {
      body.selection = -1;
      body.invalidate();
    }

    void setSelection(int index, bool scroll = false)
    {
      body.selection = index;
      if (scroll) {
        body.setScrollPositionY(index * TABLE_LINE_HEIGHT);
      }
      body.invalidate();
    }

    void setHeader(const char * const values[])
    {
      header.setHeight(TABLE_HEADER_HEIGHT);
      body.setTop(TABLE_HEADER_HEIGHT);
      body.setHeight(height() - TABLE_HEADER_HEIGHT);
      Line line(columnsCount);
      for (uint8_t i = 0; i < columnsCount; i++) {
        line.values[i] = values[i];
      }
      header.setLine(line);
    }

    void addLine(const char * const values[], std::function<void()> onPress = nullptr)
    {
      Line line(columnsCount);
      for (uint8_t i = 0; i < columnsCount; i++) {
        line.values[i] = values[i];
      }
      line.onPress = onPress;
      body.addLine(line);
    }

    void clear()
    {
      body.clear();
      clearSelection();
    }

  protected:
    uint8_t columnsCount;
    std::vector<coord_t> columnsWidth;
    Header header;
    Body body;
};

#endif // _TABLE_H_
