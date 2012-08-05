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

#ifndef ILIXI_LABEL_H_
#define ILIXI_LABEL_H_

#include <ui/TextBase.h>
#include <types/Margin.h>

namespace ilixi
{
//! A simple text label with optional border
class Label : public Widget, public TextBase
{
public:
    /*!
     * Constructor.
     *
     * @param text Label's text.
     * @param parent Label's parent.
     */
    Label(std::string text, Widget* parent = 0);

    /*!
     * Copy constructor.
     */
    Label(const Label& label);

    /*!
     * Destructor.
     */
    virtual
    ~Label();

    virtual int
    heightForWidth(int width) const;

    virtual Size
    preferredSize() const;

    /*!
     * Returns the margin around text.
     */
    Margin
    margin() const;

    /*!
     * Sets the margin around text.
     */
    void
    setMargin(const Margin& margin);

protected:
    //! This property holds the margin around text.
    Margin _margin;

    /*!
     * Draws label using stylist.
     */
    virtual void
    compose(const PaintEvent& event);

    //! Sets text layout geometry of label.
    virtual void
    updateTextBaseGeometry();
};
}

#endif /* ILIXI_LABEL_H_ */
