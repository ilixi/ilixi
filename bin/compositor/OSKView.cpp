/*
 * OSKView.cpp
 *
 *  Created on: Aug 8, 2012
 *      Author: tarik
 */

#include "OSKView.h"

namespace ilixi
{

OSKView::OSKView(Compositor* compositor, AppInstance* instance, Widget* parent)
        : AppCompositor(compositor, instance, parent)
{
    setInputMethod(PointerInput);
    setVisible(false);
}

OSKView::~OSKView()
{
}

void
OSKView::show()
{
    if (_state == APPCOMP_READY)
        setVisible(true);
}

void
OSKView::hide()
{
    setVisible(false);
}

void
OSKView::madeAvailable()
{
    if (_state == APPCOMP_NONE)
        setVisible(true);
    _state = APPCOMP_READY;
}

} /* namespace ilixi */
