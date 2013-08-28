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

#include <types/Event.h>
#include <ui/Widget.h>

namespace ilixi
{

#ifdef ILIXI_STEREO_OUTPUT
PaintEvent::PaintEvent(Widget* widget, const PaintEvent& evt)
        : right(),
          rect(),
          eye(evt.eye)
{
    widget->surface()->updateSurface(evt);
    Rectangle lt = widget->frameGeometry();
    lt.translate(widget->z(), 0);
    Rectangle rt = widget->frameGeometry();
    rt.translate(-widget->z(), 0);
    rect = lt.intersected(evt.rect);
    right = rt.intersected(evt.right);
}
#else
PaintEvent::PaintEvent(Widget* widget, const PaintEvent& evt)
        : rect(),
          eye(evt.eye)
{
    widget->surface()->updateSurface(evt);
    rect = widget->frameGeometry().intersected(evt.rect);
}
#endif

} /* namespace ilixi */
