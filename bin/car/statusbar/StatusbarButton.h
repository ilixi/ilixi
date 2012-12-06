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

#ifndef STATUSBARBUTTON_H_
#define STATUSBARBUTTON_H_

#include <ui/Widget.h>
#include <types/Image.h>
#include <vector>

namespace ilixi
{

class StatusbarButton : public Widget
{
public:
    StatusbarButton(Widget* parent = 0);

    virtual
    ~StatusbarButton();

    Size
    preferredSize() const;

    bool
    active() const;

    unsigned int
    buttonState() const;

    void
    setActive(bool active = true);

    void
    setButtonState(unsigned int state);

    void
    addImage(Image* image);

    sigc::signal<void> sigClicked;

protected:
    virtual void
    pointerButtonDownEvent(const PointerEvent& pointerEvent);

    virtual void
    pointerButtonUpEvent(const PointerEvent& pointerEvent);

    virtual void
    enterEvent(const PointerEvent& event);

    virtual void
    leaveEvent(const PointerEvent& event);

    void
    compose(const PaintEvent& event);

private:
    unsigned int _myState;bool _click;bool _active;
    std::vector<Image*> _images;

    static Image* bgDef;
};

} /* namespace ilixi */
#endif /* STATUSBARBUTTON_H_ */
