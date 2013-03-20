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

#include "ImageWidget.h"
#include <graphics/Painter.h>
#include <core/Logger.h>
#include <sstream>

using namespace ilixi;

ImageWidget::ImageWidget(const std::string& text, Widget* parent)
        : Button(text, parent)
{
    _anim.setDuration(300);
    _anim.sigExec.connect(sigc::mem_fun(this, &ImageWidget::tweenSlot));
    _imageScale = new Tween(Tween::SINE, Tween::EASE_OUT, 0, 1);
    _anim.addTween(_imageScale);
    _boxSlide = new Tween(Tween::BOUNCE, Tween::EASE_OUT, 0, 1);
    _anim.addTween(_boxSlide);

    setInputMethod(KeyPointer);
    std::stringstream ss;
    ss << "ID: " << id();
    PushButton* button = new PushButton(ss.str());
    Size s = button->preferredSize();
    button->setGeometry(10, 10, s.width(), s.height());
    addChild(button);
}

ImageWidget::~ImageWidget()
{
    delete _image;
}

void
ImageWidget::setImage(Image* image)
{
    _image = image;
}

void
ImageWidget::tweenSlot()
{
    update();
}

void
ImageWidget::compose(const PaintEvent& event)
{
    Painter p(this);
    p.begin(event);

    float val1 = _imageScale->value();
    float val2 = _boxSlide->value();

    // draw image
    p.setBrush(Color(0, 0, 0, 80 + val1 * 175));
    p.stretchImage(
            _image,
            -20 * val1,
            -20 * val1,
            width() + 40 * val1,
            height() + 40 * val1,
            (DFBSurfaceBlittingFlags) (DSBLIT_SRC_PREMULTCOLOR
                    | DSBLIT_BLEND_COLORALPHA));

    // overlay rect
    int y = height() - val2 * 50;
    p.setBrush(Color("006666"));
    p.fillRectangle(0, y, width(), 50);
    p.setBrush(Color(255, 255, 255, 255));
    p.drawLayout(_layout, 5, y);

    p.end();
}

void
ImageWidget::updateTextBaseGeometry()
{
    _layout.setBounds(0, 0, width() - 10, 50);
    _layout.doLayout(font());
}

void
ImageWidget::pointerButtonUpEvent(const PointerEvent& event)
{
    sigPressed();
}

#if POINTER_MODE
void
ImageWidget::enterEvent(const PointerEvent& event)
{
//  sigFocused(this);
    _anim.stop();
    _imageScale->setInitialValue(_imageScale->value());
    _imageScale->setEndValue(1);
    _boxSlide->setInitialValue(_boxSlide->value());
    _boxSlide->setEndValue(1);
    _anim.start();
}

void
ImageWidget::leaveEvent(const PointerEvent& event)
{
    _anim.stop();
    _imageScale->setInitialValue(_imageScale->value());
    _imageScale->setEndValue(0);
    _boxSlide->setInitialValue(_boxSlide->value());
    _boxSlide->setEndValue(0);
    _anim.start();
}
#else
void
ImageWidget::focusInEvent()
{
    _anim.stop();
    _imageScale->setInitialValue(0);
    _imageScale->setEndValue(1);
    _boxSlide->setInitialValue(0);
    _boxSlide->setEndValue(1);
    _anim.start();
}

void
ImageWidget::focusOutEvent()
{
    _anim.stop();
    _imageScale->setInitialValue(1);
    _imageScale->setEndValue(0);
    _boxSlide->setInitialValue(1);
    _boxSlide->setEndValue(0);
    _anim.start();
}
#endif
