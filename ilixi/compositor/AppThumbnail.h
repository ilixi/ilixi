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

#ifndef ILIXI_APPTHUMBNAIL_H_
#define ILIXI_APPTHUMBNAIL_H_

#include <compositor/AppCompositor.h>
#include <lib/TweenAnimation.h>
#include <ui/ToolButton.h>

namespace ilixi
{
class ILXCompositor;

//! Provides a thumbnail for an application instance.
class AppThumbnail : public AppCompositor
{
public:
    /*!
     * Constructor.
     */
    AppThumbnail(ILXCompositor* compositor, AppInstance* instance, Widget* parent = 0);

    /*!
     * Destructor.
     */
    virtual
    ~AppThumbnail();

    /*!
     * Returns a fixed size (196, 156).
     */
    Size
    preferredSize() const;

    virtual void
    addWindow(IDirectFBWindow* window, bool eventHandling = true, bool blocking = true);

    /*!
     * Shows/hides close button.
     */
    void
    setCloseVisible(bool visible);

    /*!
     * This signal is emitted when thumbnail is selected by pressing Space key or releasing pointer button.
     */
    sigc::signal<void, AppInstance*> sigSelected;

    /*!
     * This signal is emitted when thumbnail is focused.
     */
    sigc::signal<void, AppThumbnail*> sigFocused;
    ToolButton* _close;

protected:
    virtual void
    compose(const PaintEvent& event);

    virtual void
    pointerButtonUpEvent(const PointerEvent& pointerEvent);

    virtual void
    keyUpEvent(const KeyEvent& keyEvent);

    virtual void
    focusInEvent();

    virtual void
    focusOutEvent();

private:
    //! Sets geometry of close button.
    void
    updateThumbGeometry();

    void
    handleRestack();
};

} /* namespace ilixi */
#endif /* ILIXI_APPTHUMBNAIL_H_ */
