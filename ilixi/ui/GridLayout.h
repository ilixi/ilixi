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

#ifndef ILIXI_GRIDLAYOUT_H_
#define ILIXI_GRIDLAYOUT_H_

#include <ui/LayoutBase.h>
#include <vector>
#include <climits>

namespace ilixi
{
//! A grid layout with fixed number of rows and columns.
/*!
 * In this layout widgets are placed inside cells.
 *
 * A widget can span across multiple rows or columns to the left or down respectively.
 *
 * Height/width of an individual row/column can be set as well.
 */
class GridLayout : public LayoutBase
{
public:
    //! Used for storing and caching parameters for a grid cell.
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

    //! Used for storing and caching parameters of a row or column.
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

    /*!
     * Constructor.
     *
     * @param rows number of rows.
     * @param columns number of columns.
     * @param parent pointer to parent widget.
     */
    GridLayout(unsigned int rows, unsigned int columns, Widget* parent = 0);

    /*!
     * Destructor.
     */
    virtual
    ~GridLayout();

    /*!
     * Not implemented.
     */
    virtual int
    heightForWidth(int width) const;

    /*!
     * Not implemented.
     */
    virtual Size
    preferredSize() const;

    /*!
     * Returns number of columns.
     */
    unsigned int
    columns() const;

    /*!
     * Returns number of rows.
     */
    unsigned int
    rows() const;

    /*!
     * Returns width in pixels for given column.
     *
     * @param column starts from 0.
     */
    unsigned int
    columnWidth(int column) const;

    /*!
     * Returns height in pixels for given row.
     *
     * @param row starts from 0.
     */
    unsigned int
    rowHeight(int row) const;

    /*!
     * Removes all widgets from layout.
     */
    void
    clear();

    /*!
     * Sets the width of a given column.
     *
     * @param column starts from 0.
     * @param colWidth in pixels.
     */
    void
    setColumnWidth(unsigned int column, unsigned int colWidth);

    /*!
     * Sets the height of a given row.
     *
     * @param row starts from 0.
     * @param rowHeight in pixels.
     */
    void
    setRowHeight(unsigned int row, unsigned int rowHeight);

    /*!
     * Adds widget to first empty cell.
     *
     * @param widget is owned by layout.
     *
     * @return true on success.
     */
    virtual bool
    addWidget(Widget* widget);

    /*!
     * Adds widget to given cell (row, column) on grid.
     *
     * @param widget is owned by layout.
     * @param row starts from 0.
     * @param col starts from 0.
     * @param rowSpan rows to occupy. 0 or 1 for single cell, -1 will expand along the row until the end or next occupied cell.
     * @param colSpan columns to occupy. 0 or 1 for single cell, -1 will expand along the row until the end or next occupied cell.
     *
     * @return true on success.
     */
    bool
    addWidget(Widget* widget, int row, int col, int rowSpan = 0, int colSpan = 0);

    /*!
     * Removes and deletes a child widget if destroy is true.
     *
     * Returns true if successful.
     */
    virtual bool
    removeWidget(Widget* widget, bool destroy = true);

    void
    tile();

private:
    //! This property stores the number of rows.
    int _rows;
    //! This property stores the number of columns.
    int _cols;

    typedef std::vector<CellData*> CellDataVector;
    CellDataVector _cells;

    typedef std::vector<LineData> LineDataVector;
    typedef LineDataVector::iterator LineDataVectorIterator;

    std::vector<unsigned int> _colWidths;
    std::vector<unsigned int> _rowHeights;

    //! Arrange widgets on a line.
    void
    arrangeLine(LineDataVector& ld, int availableSpace, int nActive, int expanding);
};

}

#endif /* ILIXI_GRIDLAYOUT_H_ */
