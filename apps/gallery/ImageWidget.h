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

#ifndef IMAGEWIDGET_H_
#define IMAGEWIDGET_H_

#include <ui/PushButton.h>
#include <lib/TweenAnimation.h>

#define POINTER_MODE true

class ImageWidget : public ilixi::Button
{
public:
    ImageWidget(const std::string& text, ilixi::Widget* parent = 0);

    ~ImageWidget();

    void
    setImage(ilixi::Image* image);

    sigc::signal<void> sigPressed;

protected:
    void
    tweenSlot();

    virtual void
    compose(const ilixi::PaintEvent& event);

    virtual void
    pointerButtonUpEvent(const ilixi::PointerEvent& event);

    void
    updateTextBaseGeometry();

#if POINTER_MODE
    /*!
     * Starts/stops tweens.
     */
    virtual void
    enterEvent(const ilixi::PointerEvent& event);

    /*!
     * Starts/stops tweens.
     */
    virtual void
    leaveEvent(const ilixi::PointerEvent& event);
#else
    /*!
     * Starts/stops tweens.
     */
    virtual void
    focusInEvent();

    /*!
     * Starts/stops tweens.
     */
    virtual void
    focusOutEvent();
#endif

private:
    ilixi::Image* _image;
    ilixi::TweenAnimation _anim;
    ilixi::Tween* _imageScale;
    ilixi::Tween* _boxSlide;
};

#endif /* IMAGEWIDGET_H_ */
