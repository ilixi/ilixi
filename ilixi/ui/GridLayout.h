/*
 Copyright 2010-2012 Tarik Sekmen.

 All Rights Reserved.

 Written by Tarik Sekmen <tarik@ilixi.org>.

 This file is part of ilixi.

 ilixi is free software: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 ilixi is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with ilixi.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ILIXI_GRIDLAYOUT_H_
#define ILIXI_GRIDLAYOUT_H_

#include <ui/LayoutBase.h>
#include <vector>
#include <climits>

namespace ilixi
{

class GridLayout : public LayoutBase
{
public:
    struct CellData
    {
        CellData(Widget* w, int r, int c, int re, int ce)
                : widget(w),
                  row(r),
                  col(c),
                  lastRow(re),
                  lastCol(ce),
                  width(-2),
                  height(-2),
                  h4w(-2),
                  ignored(false)
        {
        }

        Widget* widget;
        int row;
        int col;
        int lastRow;
        int lastCol;
        int width;
        int height;
        int h4w;bool ignored;
    };

    struct LineData
    {
        LineData()
                : constraint(FixedConstraint),
                  value(0),
                  min(0),
                  max(INT_MAX),
                  pos(0),
                  active(false)
        {
        }

        char constraint;
        int value;
        int min;
        int max;
        int pos;bool active;
    };

    GridLayout(unsigned int rows, unsigned int columns, Widget* parent = 0);

    virtual
    ~GridLayout();

    virtual int
    heightForWidth(int width) const;

    virtual Size
    preferredSize() const;

    unsigned int
    columns() const;

    unsigned int
    rows() const;

    unsigned int
    columnWidth(int column) const;

    unsigned int
    rowHeight(int row) const;

    void
    setColumnWidth(unsigned int column, unsigned int colWidth);

    void
    setRowHeight(unsigned int row, unsigned int rowHeight);

    virtual bool
    addWidget(Widget* widget);

    bool
    addWidget(Widget* widget, int row, int col, int rowSpan = 0,
              int colSpan = 0);

    void
    tile();

private:
    int _index;
    int _rows;
    int _cols;



    typedef std::vector<CellData*> CellDataVector;
    CellDataVector _cells;

    typedef std::vector<LineData> LineDataVector;
    typedef LineDataVector::iterator LineDataVectorIterator;

    std::vector<unsigned int> _colWidths;
    std::vector<unsigned int> _rowHeights;

    void
    arrangeLine(LineDataVector& ld, int availableSpace, int nActive,
                int expanding);
};

}

#endif /* ILIXI_GRIDLAYOUT_H_ */
