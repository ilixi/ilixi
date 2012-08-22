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
    if (_cState == APPCOMP_READY)
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
    if (_cState == APPCOMP_NONE)
        setVisible(true);
    _cState = APPCOMP_READY;
}

} /* namespace ilixi */
