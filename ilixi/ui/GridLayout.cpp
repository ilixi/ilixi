/*
 Copyright 2010-2013 Tarik Sekmen.

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

#include <GridLayout.h>
#include <ui/RadioButton.h>
#include <core/Logger.h>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_GRIDLAYOUT, "ilixi/ui/GridLayout", "GridLayout");

GridLayout::GridLayout(unsigned int rows, unsigned int column, Widget* parent)
        : LayoutBase(parent),
          _rows(rows),
          _cols(column)
{
    ILOG_TRACE_W(ILX_GRIDLAYOUT);
    _cells.assign(_rows * _cols, (CellData*) NULL);
    _colWidths.assign(_cols, 0);
    _rowHeights.assign(_rows, 0);
}

GridLayout::~GridLayout()
{
    ILOG_TRACE_W(ILX_GRIDLAYOUT);
    for (unsigned int c = 0; c < _cols; c++)
    {
        for (unsigned int r = 0; r < _rows; r++)
        {
            int index = r * _cols + c;

            if (_cells[index] == NULL)
                continue;

            else if (_cells[index]->lastRow == r && _cells[index]->lastCol == c)
            {
                delete _cells[index];
                _cells[index] = NULL;
            }
        }
    }
}

int
GridLayout::heightForWidth(int width) const
{
    // FIXME implement h4w
    ILOG_TRACE_W(ILX_GRIDLAYOUT);
    return -1;
}

Size
GridLayout::preferredSize() const
{
    ILOG_TRACE_W(ILX_GRIDLAYOUT);

    int index = 0;
    std::vector<int> cols;
    std::vector<int> rows;
    cols.assign(_cols, 0);
    rows.assign(_rows, 0);

    // Clear Cell Data
    for (int i = 0; i < _rows * _cols; i++)
    {
        if (!_cells[i])
            continue;

        _cells[i]->ignored = false;
        _cells[i]->width = -2;
        _cells[i]->height = -2;
        _cells[i]->h4w = -2;
    }

    for (int c = 0; c < _cols; ++c)
    {
        for (int r = 0; r < _rows; ++r)
        {
            index = r * _cols + c;

            if (!_cells[index])
                continue;

            // request preferred size from the widget only once.
            if (_cells[index]->width == -2)
            {
                Size s = _cells[index]->widget->preferredSize();
                _cells[index]->width = s.width();
                _cells[index]->height = s.height();
                ILOG_DEBUG(ILX_GRIDLAYOUT, "   -> cells[%d] -> %d, %d\n", index, _cells[index]->width, _cells[index]->height);
//                if (_cells[index]->widget->minWidth() && _cells[index]->width < _cells[index]->widget->minWidth())
//                    _cells[index]->width = _cells[index]->widget->minWidth();
//                else if (_cells[index]->widget->maxWidth() && _cells[index]->width > _cells[index]->widget->maxWidth())
//                    _cells[index]->width = _cells[index]->widget->maxWidth();
//
//                if (_cells[index]->widget->minHeight() && _cells[index]->height < _cells[index]->widget->minHeight())
//                    _cells[index]->height = _cells[index]->widget->minHeight();
//                else if (_cells[index]->widget->maxHeight() && _cells[index]->height > _cells[index]->widget->maxHeight())
//                    _cells[index]->height = _cells[index]->widget->maxHeight();

                ILOG_DEBUG(ILX_GRIDLAYOUT, "   -> cells[%d] -> %d, %d\n", index, _cells[index]->width, _cells[index]->height);
            }
        }
    }

    // single cell
    ILOG_DEBUG(ILX_GRIDLAYOUT, " -> Single\n");
    for (int c = 0; c < _cols; ++c)
    {
        for (int r = 0; r < _rows; ++r)
        {
            index = r * _cols + c;

            if (!_cells[index])
                continue;

            if (_cells[index]->lastRow == _cells[index]->row && _cells[index]->lastCol == _cells[index]->col)
            {
                if (cols[c] < _cells[index]->width)
                    cols[c] = _cells[index]->width;

                if (rows[r] < _cells[index]->height)
                    rows[r] = _cells[index]->height;

                ILOG_DEBUG(ILX_GRIDLAYOUT, "   -> Index %d -> %d, %d\n", index, cols[c], rows[r]);
            }
        }
    }

    ILOG_DEBUG(ILX_GRIDLAYOUT, " -> Multiple\n");
    int colUsed = 0;
    int rowUsed = 0;
    for (int c = 0; c < _cols; ++c)
    {
        for (int r = 0; r < _rows; ++r)
        {
            index = r * _cols + c;

            if (!_cells[index])
                continue;

            if (_cells[index]->lastRow == r && _cells[index]->lastCol == c)
            {
                colUsed = 0;
                rowUsed = 0;

                for (int i = _cells[index]->col; i < _cells[index]->lastCol; i++)
                    colUsed += cols[i];
                for (int i = _cells[index]->row; i < _cells[index]->lastRow; i++)
                    rowUsed += rows[i];

                int cW = _cells[index]->width - colUsed;
                int cH = _cells[index]->height - rowUsed;

                if (cW && cols[c] < cW)
                    cols[c] = cW;
                if (cH && rows[r] < cH)
                    rows[r] = cH;

                ILOG_DEBUG(ILX_GRIDLAYOUT, "   -> Index %d -> %d, %d\n", index, cols[c], rows[r]);
            }
        }
    }

    int w = 0;
    for (int i = 0; i < _cols; ++i)
        w += cols[i] + spacing();
    int h = 0;
    for (int i = 0; i < _rows; ++i)
        h += rows[i] + spacing();

    ILOG_DEBUG(ILX_GRIDLAYOUT, " -> Size: %d, %d\n", w, h);

    return Size(w - spacing(), h - spacing());
}

unsigned int
GridLayout::columns() const
{
    return _cols;
}

unsigned int
GridLayout::rows() const
{
    return _rows;
}

unsigned int
GridLayout::columnWidth(int column) const
{
    if (column < _cols)
        return _colWidths[column];
    return 0;
}

unsigned int
GridLayout::rowHeight(int row) const
{
    if (row < _rows)
        return _rowHeights[row];
    return 0;
}

void
GridLayout::clear()
{
    for (unsigned int c = 0; c < _cols; c++)
    {
        for (unsigned int r = 0; r < _rows; r++)
        {
            int index = r * _cols + c;

            if (_cells[index] == NULL)
                continue;

            else if (_cells[index]->lastRow == r && _cells[index]->lastCol == c)
            {
                delete _cells[index];
                _cells[index] = NULL;
            }
        }
    }

    _cells.assign(_rows * _cols, (CellData*) NULL);

    for (WidgetList::const_iterator it = _children.begin(); it != _children.end(); ++it)
        removeWidget(*it);
}

void
GridLayout::setColumnWidth(unsigned int column, unsigned int colWidth)
{
    if (column < _cols)
        _colWidths[column] = colWidth;
}

void
GridLayout::setRowHeight(unsigned int row, unsigned int rowHeight)
{
    if (row < _rows)
        _rowHeights[row] = rowHeight;
}

bool
GridLayout::addWidget(Widget* widget)
{
    ILOG_TRACE_W(ILX_GRIDLAYOUT);
    if (!widget || isChild(widget))
        return false;

    bool result = false;

    for (unsigned int i = 0; i < _cells.size(); i++)
    {
        // add widget if cell is empty.
        if (_cells[i] == NULL)
        {
            addChild(widget);

            RadioButton* radio = dynamic_cast<RadioButton*>(widget);
            if (radio)
                _group->add(radio);

            int r = i / _cols;
            int c = r ? i % (r * _cols) : i;
            CellData* data = new CellData(widget, r, c, r, c);
            _cells[i] = data;
            result = true;
            doLayout();

            ILOG_DEBUG(ILX_GRIDLAYOUT, " -> Widget added at Cell [%d] (%d, %d) expands to (%d, %d)\n", i, r, c, r, c);
            break;
        }
    }

    if (!result)
    {
        ILOG_ERROR(ILX_GRIDLAYOUT, "No more space in this layout!\n");
        return false;
    } else
        return true;
}

bool
GridLayout::addWidget(Widget* widget, int row, int col, int rowSpan, int colSpan)
{
    ILOG_TRACE_W(ILX_GRIDLAYOUT);
    if (!widget || isChild(widget))
        return false;

    if (row > _rows)
    {
        ILOG_ERROR(ILX_GRIDLAYOUT, "Row index %d is outside grid!\n", row);
        return false;
    }

    if (col > _cols)
    {
        ILOG_ERROR(ILX_GRIDLAYOUT, "Column index %d is outside grid!", col);
        return false;
    }

    int index = row * _cols + col;
    if (_cells[index])
    {
        ILOG_ERROR(ILX_GRIDLAYOUT, "Cell [%d, %d] is occupied!", row, col);
        return false;
    }

    addChild(widget);

    RadioButton* radio = dynamic_cast<RadioButton*>(widget);
    if (radio)
        _group->add(radio);

    int lastRow = row;
    if (rowSpan == -1)
        lastRow = _rows - 1;
    else if (rowSpan > 1)
        lastRow = row + rowSpan - 1;
    if (lastRow >= _rows)
        lastRow = _rows - 1;

    int lastCol = col;
    if (colSpan == -1)
        lastCol = _cols - 1;
    else if (colSpan > 1)
        lastCol = col + colSpan - 1;
    if (lastCol >= _cols)
        lastCol = _cols - 1;

    ILOG_DEBUG(ILX_GRIDLAYOUT, " -> Request to add at Cell(%d, %d) expands to Cell(%d, %d)\n", row, col, lastRow, lastCol);

    CellData* data = new CellData(widget, row, col, lastRow, lastCol);
    _cells[index] = data;

    for (int r = row; r <= data->lastRow; r++)
    {
//        if (_cells[r * _cols] != NULL)
//        {
//            lastRow = r;
//            break;
//        }

        for (int c = col; c <= data->lastCol; c++)
        {
//            if (_cells[r * _cols + c] != NULL)
//            {
//                lastCol = c;
//                break;
//            }

            _cells[r * _cols + c] = data;
        }
    }
//    data->lastRow = lastRow;
//    data->lastCol = lastCol;

    ILOG_DEBUG(ILX_GRIDLAYOUT, " -> Widget added at Cell(%d, %d) expands to Cell(%d, %d)\n", row, col, lastRow, lastCol);
    doLayout();
    return true;
}

bool
GridLayout::removeWidget(Widget* widget, bool destroy)
{
    ILOG_TRACE_W(ILX_GRIDLAYOUT);
    if (isChild(widget))
    {
        RadioButton* radio = dynamic_cast<RadioButton*>(widget);
        if (radio)
            _group->remove(radio);
    }

    if (removeChild(widget, destroy))
    {
        for (int i = 0; i < _rows * _cols; i++)
        {
            if (_cells[i] && _cells[i]->widget == widget)
            {
                delete _cells[i];
                _cells[i] = NULL;
            }
        }
        doLayout();
        return true;
    }
    return false;
}

void
GridLayout::tile()
{
    ILOG_TRACE_W(ILX_GRIDLAYOUT);
    if (!_modified)
        return;

    int index = 0;      // Cell index
    Widget* widget;     // Widget at _cells[index]
    int expanding = 0;  // number of expanding columns or rows.
    int spaceReq;       // cell's required space in horizontal or vertical direction.
    int spaceUsed = 0;  // space made available to widget in horizontal or vertical direction.
    int cActive = 0;    // number of active columns
    int rActive = 0;    // number of active rows

    //***********************************************************
    //                    Clear Cell Data
    //***********************************************************
    for (int i = 0; i < _rows * _cols; i++)
    {
        if (!_cells[i])
            continue;
        _cells[i]->ignored = false;
        _cells[i]->width = -2;
        _cells[i]->height = -2;
        _cells[i]->h4w = -2;
    }

    //***********************************************************
    //                  Initialise Column Data
    //***********************************************************
    ILOG_DEBUG(ILX_GRIDLAYOUT, " -> Initialising column data...\n");
    LineDataVector cd;
    for (unsigned int c = 0; c < _cols; c++)
    {
        cd.push_back(LineData());

        // if column has user set width, fix it and ignore cell data.
        if (_colWidths[c])
        {
            cd[c].min = _colWidths[c];
            cd[c].value = _colWidths[c];
            cd[c].active = true;
            cActive++;
            continue;
        }

        // For each cell on this column...
        for (unsigned int r = 0; r < _rows; r++)
        {
            index = r * _cols + c;

            // if the cell is empty, go to next cell.
            if (!_cells[index])
                continue;

            // lookup widget on this cell
            widget = _cells[index]->widget;

            // if the widget is ignored, go to next cell.
            if (!widget->visible() && (widget->yConstraint() == IgnoredConstraint || widget->xConstraint() == IgnoredConstraint))
            {
                // this cell becomes ignored from now on...
                _cells[index]->ignored = true;
                continue;
            }

            // this column has active widget(s)
            cd[c].active = true;

            // request preferred size from the widget only once.
            if (_cells[index]->width == -2)
            {
                Size s = widget->preferredSize();
                _cells[index]->width = s.width();
                _cells[index]->height = s.height();
            }

            // This flag is used to specify that data for current column should be updated.
            // If the widget does not span horizontally, flag is set to true.
            bool updateCol = false;

            // the widget occupies a single cell.
            if (_cells[index]->col == _cells[index]->lastCol)
            {
                spaceUsed = 0;
                spaceReq = _cells[index]->width;
                updateCol = true;
            }

            // the widget on the cell is not spanning any more.
            else if (c == _cells[index]->lastCol)
            {
                spaceUsed = 0;

                // if previous cells can not shrink or they have min. width set, subtract their total width (cUsed) from this widget's width.
                // this amount is always available to the widget for use.
                for (int i = _cells[index]->col; i < _cells[index]->lastCol; i++)
                {
                    if (cd[i].min)
                        spaceUsed += cd[i].min + spacing();
                    else if (!(cd[i].constraint & ShrinkPolicy))
                        spaceUsed += cd[i].value + spacing();
                }
                spaceReq = _cells[index]->width - spaceUsed;

                if (spaceReq < 0)
                    spaceReq = 0;
                updateCol = true;
            }

            if (updateCol)
            {
                // handle max. and min. size for column
                if (cd[c].min < (widget->minWidth() - spaceUsed))
                    cd[c].min = widget->minWidth() - spaceUsed;

                if (widget->maxWidth() > 0 && cd[c].max > (widget->maxWidth() - spaceUsed))
                    cd[c].max = widget->maxWidth() - spaceUsed;

                // the widget's width is fixed.
                if (widget->xConstraint() == FixedConstraint)
                {
                    // update column value as cell can not shrink any more...
                    if (spaceReq > cd[c].value)
                    {
                        cd[c].value = spaceReq;
                        ILOG_DEBUG( ILX_GRIDLAYOUT, " -> Column %d value is updated to %d on cell [%d, %d]\n", c, spaceReq, r, c);
                    }
                } else
                {
                    // the widget requires space.
                    if (!(widget->xConstraint() & ShrinkPolicy) && cd[c].value < spaceReq)
                    {
                        cd[c].value = spaceReq;
                        ILOG_DEBUG( ILX_GRIDLAYOUT, " -> Column %d value is updated to %d on cell [%d, %d]\n", c, spaceReq, r, c);
                    }

                    if (!(cd[c].constraint & GrowPolicy) && (widget->xConstraint() & GrowPolicy))
                    {
                        ILOG_DEBUG( ILX_GRIDLAYOUT, " -> Column %d acquires GrowPolicy on cell [%d, %d]\n", c, r, c);
                        cd[c].constraint = cd[c].constraint | GrowPolicy;
                    }

                    // column does not grow but cell can not shrink
                    else if ((widget->xConstraint() & ShrinkPolicy) && (cd[c].value < spaceReq))
                    {
                        ILOG_DEBUG(ILX_GRIDLAYOUT, " -> Column %d can shrink\n", c);
                        cd[c].constraint = cd[c].constraint | ShrinkPolicy;
                        cd[c].value = spaceReq;
                    }
                }
                // if cell should expand, set the expand flag to 1.
                if (widget->xConstraint() & ExpandPolicy)
                {
                    if (!(cd[c].constraint & ExpandPolicy))
                        expanding++;
                    cd[c].constraint = cd[c].constraint | ExpandPolicy;
                }
            }
        }
        ILOG_DEBUG( ILX_GRIDLAYOUT, " -> Col[%d] value: %d, min: %d, max: %d, cons: %d\n", c, cd[c].value, cd[c].min, cd[c].max, cd[c].constraint);
        if (cd[c].active)
            cActive++;
    }

    // Arrange columns
    arrangeLine(cd, width(), cActive, expanding);

    //***********************************************************
    //                  Initialise Row Data
    //***********************************************************
    ILOG_DEBUG(ILX_GRIDLAYOUT, " -> Initialising row data...\n");
    expanding = 0;
    LineDataVector rd;

    for (unsigned int r = 0; r < _rows; r++)
    {
        rd.push_back(LineData());
        rd[r].constraint = FixedConstraint; // FIXME ?

        // if row has user set height, fix it and ignore cell data.
        if (_rowHeights[r])
        {
            rd[r].min = _rowHeights[r];
            rd[r].value = _rowHeights[r];
            rd[r].active = true;
            rd[r].constraint = FixedConstraint;
            rActive++;
            continue;
        }

        // For each cell on this row...
        for (unsigned int c = 0; c < _cols; c++)
        {
            index = r * _cols + c;

            // if the cell is empty, go to next cell.
            if (!_cells[index])
                continue;

            // if widget is ignored, continue to next cell.
            if (_cells[index]->ignored)
                continue;

            // lookup widget on this cell
            widget = _cells[index]->widget;

            // this row has active widget(s)
            rd[r].active = true;

            // This flag is used to specify that data for current row should be updated.
            // If the widget does not span vertically, then flag is set to true.
            bool updateRow = false;

            if (c != _cells[index]->lastCol)
                continue;

            // the widget on the cell is not spanning.
            if (_cells[index]->row == _cells[index]->lastRow)
            {
                spaceUsed = 0;

                // calculate height for width and update height if widget allows...
                _cells[index]->h4w = widget->heightForWidth(cd[c].value);
                if (_cells[index]->h4w)
                {
                    // widget can shrink and h4w is less than widget's preferred height.
                    if ((widget->yConstraint() & ShrinkPolicy) && _cells[index]->h4w < _cells[index]->height)
                        _cells[index]->height = _cells[index]->h4w;

                    // widget can grow and h4w is greater than widget's preferred height.
                    else if ((widget->yConstraint() & GrowPolicy) && _cells[index]->h4w > _cells[index]->height)
                        _cells[index]->height = _cells[index]->h4w;
                }

                spaceReq = _cells[index]->height;
                updateRow = true;
                ILOG_DEBUG(ILX_GRIDLAYOUT, " -> R-Single > req: %d used: %d\n", spaceReq, spaceUsed);
            }

            //  spanning widget ends on this cell.

            else if (r == _cells[index]->lastRow)
            {
                // if previous cells can not shrink or they have min. width set, subtract their total height (cUsed)from this widget's width
                for (int i = _cells[index]->row; i < _cells[index]->lastRow; i++)
                {
                    if (rd[i].min)
                        spaceUsed += rd[i].min + spacing();
                    //                  else if (cd[c].max < INT_MAX)
                    //                    cUsed += cd[c].max;

                    else if (!(rd[i].constraint & ShrinkPolicy))
                        spaceUsed += rd[i].value + spacing();
                }

                // calculate height for width and update height if widget allows...
                _cells[index]->h4w = widget->heightForWidth(spaceUsed + cd[c].value);
                if (_cells[index]->h4w)
                {
                    // can shrink
                    if ((widget->yConstraint() & ShrinkPolicy) && _cells[index]->h4w < _cells[index]->height)
                        _cells[index]->height = _cells[index]->h4w;

                    // cannot grow
                    else if ((widget->yConstraint() & GrowPolicy) && _cells[index]->h4w > _cells[index]->height)
                        _cells[index]->height = _cells[index]->h4w;
                }

                spaceReq = _cells[index]->height - spaceUsed;
                if (spaceReq < 0)
                    spaceReq = 0;
                updateRow = true;
                ILOG_DEBUG(ILX_GRIDLAYOUT, " -> R-Span > req: %d used: %d\n", spaceReq, spaceUsed);
            }

            // calculate best height for row.
            if (updateRow)
            {
                // update min if it is less than this cells' min. (greatest min)
                if (rd[r].min < (widget->minHeight() - spaceUsed))
                    rd[r].min = widget->minHeight() - spaceUsed;

                // update max if it is greater than this cells' max. (smallest max)
                if (widget->maxHeight() > 0 && rd[r].max > (widget->maxHeight() - spaceUsed))
                    rd[r].max = widget->maxHeight() - spaceUsed;

                if (widget->yConstraint() == FixedConstraint)
                {
                    // update row value as cell can not shrink any more...
                    if (spaceReq > rd[r].value)
                    {
                        rd[r].value = spaceReq;
                        ILOG_DEBUG(ILX_GRIDLAYOUT, " -> Row %d value is updated to %d on cell [%d, %d]\n", r, spaceReq, r, c);
                    }
                } else
                {
                    // the widget requires space.
                    if (!(widget->yConstraint() & ShrinkPolicy) && rd[r].value < spaceReq)
                    {
                        rd[r].value = spaceReq;
                        ILOG_DEBUG(ILX_GRIDLAYOUT, " -> Row %d value is updated to %d on cell [%d, %d]\n", r, spaceReq, r, c);
                    }

                    if (!(rd[r].constraint & GrowPolicy) && (widget->yConstraint() & GrowPolicy))
                    {
                        ILOG_DEBUG(ILX_GRIDLAYOUT, " -> Row %d acquires GrowPolicy on cell [%d, %d]\n", r, r, c);
                        rd[r].constraint = rd[r].constraint | GrowPolicy;
                    }

                    // row does not grow but cell can not shrink
                    else if ((widget->yConstraint() & ShrinkPolicy) && rd[r].value < spaceReq)
                    {
                        ILOG_DEBUG(ILX_GRIDLAYOUT, " -> Row %d can shrink\n", r);
                        rd[r].constraint = rd[r].constraint | ShrinkPolicy;
                        rd[r].value = spaceReq;
                    }
                }
                // if cell should expand, set the expand flag to 1.
                if (widget->yConstraint() & ExpandPolicy)
                {
                    if (!(rd[r].constraint & ExpandPolicy))
                        expanding++;
                    rd[r].constraint = rd[r].constraint | ExpandPolicy;
                }
            }
        }
        ILOG_DEBUG( ILX_GRIDLAYOUT, " -> Row[%d] h: %d constraint: %d\n", r, rd[r].value, rd[r].constraint);
        if (rd[r].active)
            rActive++;
    }
    // Arrange rows
    arrangeLine(rd, height(), rActive, expanding);

    //***********************************************************
    //                      Arrange Widgets
    //***********************************************************
    int wWidth = 0; // widget width
    int wHeight = 0; // widget height

    int wX = 0;
    int wY = 0;
    int up, down, left;
    bool updateWidget = false;

    for (unsigned int c = 0; c < _cols; c++)
    {
        // ignore inactive columns
        if (!cd[c].active)
            continue;

        for (unsigned int r = 0; r < _rows; r++)
        {
            // ignore inactive rows.
            if (!rd[r].active)
                continue;

            // if current cell at index is empty, continue to next cell
            index = r * _cols + c;
            if (!_cells[index])
                continue;

            // if widget on the cell is ignored, continue to next cell
            widget = _cells[index]->widget;
            if (_cells[index]->ignored)
                continue;

            updateWidget = false;

            // update widget if it occupies a single cell.
            if (_cells[index]->lastRow == _cells[index]->row && _cells[index]->lastCol == _cells[index]->col)
            {
                wWidth = cd[c].value;
                wHeight = rd[r].value;
                updateWidget = true;
            }
            // update widget if we are on its last cell in south east direction.
            else if (_cells[index]->lastRow == r && _cells[index]->lastCol == c)
            {
                wWidth = 0;
                wHeight = 0;

                for (int i = _cells[index]->col; i < c; i++)
                    wWidth += cd[i].value + spacing();
                wWidth += cd[c].value;

                for (int i = _cells[index]->row; i < r; i++)
                    wHeight += rd[i].value + spacing();
                wHeight += rd[r].value;
                updateWidget = true;
            }

            // Set widget geometry...
            if (updateWidget)
            {
                int x = cd[_cells[index]->col].pos;
                int y = rd[_cells[index]->row].pos;

                // set width
                if (widget->xConstraint() == FixedConstraint)
                    widget->setWidth(_cells[index]->width);

                else if (wWidth < _cells[index]->width && !(widget->xConstraint() & ShrinkPolicy))
                    widget->setWidth(_cells[index]->width);

                else if (wWidth > _cells[index]->width && !(widget->xConstraint() & GrowPolicy))
                    widget->setWidth(_cells[index]->width);

                else
                    widget->setWidth(wWidth);

                // set height
                if (widget->yConstraint() == FixedConstraint)
                {
                    widget->setHeight(_cells[index]->height);
                    y += (rd[_cells[index]->row].value - widget->height()) / 2;
                }

                else if (wHeight < _cells[index]->height && !(widget->yConstraint() & ShrinkPolicy))
                    widget->setHeight(_cells[index]->height);

                else if (wHeight > _cells[index]->height && !(widget->yConstraint() & GrowPolicy))
                    widget->setHeight(_cells[index]->height);

                else
                    widget->setHeight(wHeight);

                widget->moveTo(x, y);

                ILOG_DEBUG(ILX_GRIDLAYOUT, " -> Widget in Cell[%d, %d] - Pos(%d, %d) - Size(%d, %d)\n", _cells[index]->row, _cells[index]->col, widget->x(), widget->y(), widget->width(), widget->height());

//                index = r * _cols + c;
                up = (r - (_cells[index]->lastRow - _cells[index]->row) - 1) * _cols + c - (_cells[index]->lastCol - _cells[index]->col);
                down = (r + 1) * _cols + c;
                left = (r - (_cells[index]->lastRow - _cells[index]->row)) * _cols + c - (_cells[index]->lastCol - _cells[index]->col) - 1;

                // set Neighbours
                if (c == 0)
                {
                    if (_keyNavChildrenFirst)
                    {
                        int lC = r * _cols - 1;
                        widget->setNeighbour(Left, lC > 0 ? (_cells[lC] ? _cells[lC]->widget : NULL) : getNeighbour(Left));
                    } else
                        widget->setNeighbour(Left, getNeighbour(Left));

                    if (_cols > 1)
                        widget->setNeighbour(Right, _cells[index + 1] ? _cells[index + 1]->widget : NULL);
                    else
                        widget->setNeighbour(Right, getNeighbour(Right)); // single column case
                } else if (c == _cols - 1)
                {
                    if (left >= 0)
                        widget->setNeighbour(Left, _cells[left] ? (_cells[left] ? _cells[left]->widget : NULL) : NULL);
                    else
                        widget->setNeighbour(Left, getNeighbour(Left));
                    if (_keyNavChildrenFirst)
                    {
                        int rC = (r + 1) * _cols;
                        widget->setNeighbour(Right, ((rC < _rows * _cols) && _cells[rC]) ? _cells[rC]->widget : getNeighbour(Right));
                    } else
                        widget->setNeighbour(Right, getNeighbour(Right));
                } else
                {
                    widget->setNeighbour(Left, _cells[left] ? _cells[left]->widget : NULL);
                    widget->setNeighbour(Right, _cells[index + 1] ? _cells[index + 1]->widget : NULL);
                }

                if (r == 0)
                {
                    widget->setNeighbour(Up, getNeighbour(Up));
                    if (_rows > 1)
                        widget->setNeighbour(Down, _cells[down] ? _cells[down]->widget : NULL);
                    else
                        widget->setNeighbour(Down, getNeighbour(Down)); // single row case
                } else if (r == _rows - 1)
                {
                    if (up >= 0)
                        widget->setNeighbour(Up, _cells[up] ? _cells[up]->widget : NULL);
                    else
                        widget->setNeighbour(Up, getNeighbour(Up));
                    widget->setNeighbour(Down, getNeighbour(Down));
                } else
                {
                    widget->setNeighbour(Up, _cells[up] ? _cells[up]->widget : NULL);
                    widget->setNeighbour(Down, _cells[down] ? _cells[down]->widget : NULL);
                }
            }
        }
    }
    _modified = false;
}

void
GridLayout::arrangeLine(LineDataVector& ld, int availableSpace, int nActive, int expanding)
{
    ILOG_TRACE_W(ILX_GRIDLAYOUT);
    availableSpace -= ((nActive - 1) * spacing());
    int lineAverage = availableSpace / nActive;
    ILOG_DEBUG( ILX_GRIDLAYOUT, " -> Count: %d\t availableSpace: %d\t average: %d\n", nActive, availableSpace, lineAverage);
    LineDataVector ldCopy = ld;
//***********************************************************
//                    FixedConstraint
//***********************************************************
    int spaceUsed = 0;
    LineDataVectorIterator it = ldCopy.begin();
    while (it != ldCopy.end())
    {
        //        ignore non-active columns...
        if (!((LineData) *it).active)
            it = ldCopy.erase(it);
        else if (((LineData) *it).constraint == FixedConstraint && ((LineData) *it).min == 0)
        //          && ((ActiveWidget) *it).widget->maxHeight() < 0)

        {
            spaceUsed += ((LineData) *it).value;
            it = ldCopy.erase(it);
            nActive--;
        } else
            ++it;
    }
    if (spaceUsed)
    {
        availableSpace -= spaceUsed;
        if (ldCopy.size())
            lineAverage = availableSpace / ldCopy.size();
        ILOG_DEBUG( ILX_GRIDLAYOUT, " -> Count@fixed: %lu\t availableSpace: %d\t average: %d\n", ldCopy.size(), availableSpace, lineAverage);
    }

// TODO: Error in max-min size.

//***********************************************************
//                      MaximumSize
//***********************************************************
    spaceUsed = 0;
    it = ldCopy.begin();
    while (it != ldCopy.end())
    {
        if (!(((LineData) *it).constraint & ExpandPolicy) && lineAverage > ((LineData) *it).max)
        {
            spaceUsed += ((LineData) *it).max;
            it = ldCopy.erase(it);
        } else
            ++it;
    }
    if (spaceUsed)
    {
        availableSpace -= spaceUsed;
        if (ldCopy.size())
            lineAverage = availableSpace / ldCopy.size();
        ILOG_DEBUG( ILX_GRIDLAYOUT, " -> Count@Max: %lu\t availableSpace: %d\t average: %d\n", ldCopy.size(), availableSpace, lineAverage);
    }

//***********************************************************
//                       MinimumSize
//***********************************************************
    spaceUsed = 0;
    it = ldCopy.begin();
    while (it != ldCopy.end())
    {
        if (lineAverage < ((LineData) *it).min)
        {
            spaceUsed += ((LineData) *it).min;
            it = ldCopy.erase(it);
        } else
            ++it;
    }
    if (spaceUsed)
    {
        availableSpace -= spaceUsed;
        if (ldCopy.size())
            lineAverage = availableSpace / ldCopy.size();
        ILOG_DEBUG( ILX_GRIDLAYOUT, " -> Count@Min: %lu\t spaceUsed=%d\t availableSpace: %d\t average: %d\n", ldCopy.size(), spaceUsed, availableSpace, lineAverage);
    }

//***********************************************************
//                     ShrinkPolicy
//***********************************************************
    spaceUsed = 0;
    it = ldCopy.begin();
    while (it != ldCopy.end())
    {
        if (lineAverage < ((LineData) *it).value && !(((LineData) *it).constraint & ShrinkPolicy))
        {
            spaceUsed += ((LineData) *it).value;
            it = ldCopy.erase(it);
        } else
            ++it;
    }
    if (spaceUsed)
    {
        availableSpace -= spaceUsed;
        if (ldCopy.size())
            lineAverage = availableSpace / ldCopy.size();
        ILOG_DEBUG( ILX_GRIDLAYOUT, " -> Count@MiC: %lu\t availableSpace: %d\t average: %d\n", ldCopy.size(), availableSpace, lineAverage);
    }

//***********************************************************
//                     GrowPolicy
//***********************************************************
    spaceUsed = 0;
    it = ldCopy.begin();
    while (it != ldCopy.end())
    {
        if (lineAverage > ((LineData) *it).value && !(((LineData) *it).constraint & GrowPolicy))
        {
            spaceUsed += ((LineData) *it).value;
            it = ldCopy.erase(it);
        } else
            ++it;
    }
    if (spaceUsed)
    {
        availableSpace -= spaceUsed;
        if (ldCopy.size())
            lineAverage = availableSpace / ldCopy.size();
        ILOG_DEBUG( ILX_GRIDLAYOUT, " -> Count@MaC: %lu\t availableSpace: %d\t average: %d\n", ldCopy.size(), availableSpace, lineAverage);
    }

//***********************************************************
//                       ExpandPolicy
//***********************************************************
    int expandAvg = 0;
    if (expanding)
    {
        ILOG_DEBUG(ILX_GRIDLAYOUT, " -> Entering expanding... Avg: %d\n", lineAverage);
        int expandSpace = 0;
        it = ldCopy.begin();
        while (it != ldCopy.end())
        {
            if (!(((LineData) *it).constraint & ExpandPolicy))
            {
                if (((LineData) *it).min > 0 && lineAverage > ((LineData) *it).min && ((LineData) *it).min > ((LineData) *it).value)
                {
                    expandSpace += lineAverage - ((LineData) *it).min;
                    // FIXME ((LineData) *it).value = ((LineData) *it).min;
                    ILOG_DEBUG( ILX_GRIDLAYOUT, " -> Additional space found: %d, min: %d\n", lineAverage - ((LineData) *it).min, ((LineData) *it).min);
                } else if (lineAverage > ((LineData) *it).value)
                {
                    expandSpace += lineAverage - ((LineData) *it).value;
                    ILOG_DEBUG( ILX_GRIDLAYOUT, " -> Additional space found: %d, value: %d\n", lineAverage - ((LineData) *it).value, ((LineData) *it).value);
                }
            }
            ++it;
        }
        if (expandSpace)
        {
            expandAvg = expandSpace / expanding;
            ILOG_DEBUG(ILX_GRIDLAYOUT, " -> expandAverage: %d, average: %d\n", expandAvg, lineAverage);
        }
    }

//***********************************************************
//                    Set Line Geometry
//***********************************************************
    int artifact = availableSpace - lineAverage * ldCopy.size();
    int pos = 0;
    ILOG_DEBUG( ILX_GRIDLAYOUT, " -> available: %d, average: %d, artifact: %d\n", availableSpace, lineAverage, artifact);

    for (unsigned int i = 0; i < ld.size(); i++)
    {
        if (!ld[i].active) // ignore inactive rows
            continue;

        if (expanding)
        {
            if (ld[i].constraint & ExpandPolicy)
            {
                if (artifact)
                {
                    ld[i].value = lineAverage + expandAvg + artifact;
                    artifact = 0;
                } else
                    ld[i].value = lineAverage + expandAvg;
            } else if (ld[i].min > ld[i].value)
                ld[i].value = ld[i].min;

            else if (ld[i].max < ld[i].value)
                ld[i].value = ld[i].max;

            else if ((ld[i].constraint & ShrinkPolicy) && lineAverage < ld[i].value)
                ld[i].value = lineAverage;
        } else
        {
            if (ld[i].min > lineAverage)
                ld[i].value = ld[i].min;

            else if (ld[i].max < lineAverage)
                ld[i].value = ld[i].max;

            else if ((ld[i].constraint & ShrinkPolicy) && ld[i].value > lineAverage)
                ld[i].value = lineAverage;

            else if ((ld[i].constraint & GrowPolicy) && ld[i].value < lineAverage)
            {
                if (artifact)
                {
                    ld[i].value = lineAverage + artifact;
                    artifact = 0;
                } else
                    ld[i].value = lineAverage;
            }
        }
        ld[i].pos = pos;
        ILOG_DEBUG(ILX_GRIDLAYOUT, " -> line[%d] - pos=%d, value=%d, cons=%d\n", i, pos, ld[i].value, ld[i].constraint);
        pos += ld[i].value + spacing();
    }
}

} /* namespace ilixi */

