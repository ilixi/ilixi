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

#ifndef ILIXI_GLWIDGET_H_
#define ILIXI_GLWIDGET_H_

#include <ui/Widget.h>

#define EGL_EGLEXT_PROTOTYPES

#include <EGL/egl.h>
#include <EGL/egldfbext.h>

namespace ilixi
{

class EGLWidget : public Widget
{
public:
    /*!
     * Constructor.
     * Initialises GL context.
     * @param parent
     */
    EGLWidget(Widget* parent = 0);

    virtual
    ~EGLWidget();

    EGLDisplay
    eglDisplay() const;

    EGLContext
    eglContext() const;

    EGLSurface
    eglSurface() const;

protected:
    /*!
     * Reimplement this method.
     * By default, clears GL_COLOR_BUFFER_BIT only.
     */
    virtual void
    renderGL(const PaintEvent& event);

    /*!
     * Simply calls renderGL()
     */
    void
    compose(const PaintEvent& event);

    void
    setAutoSwap(bool autoSwap);

private:
    //! Flag is set to true once initialiseGL is called.
    bool _initialisedGL;
    //! EGL surface.
    EGLSurface _eglSurface;
    EGLContext _eglContext;
    bool _autoSwap;


    /*!
     * Reimplement this method to initialise shaders etc.
     */
    virtual void
    initialiseGL();

    /*!
     * This function is called if widget's geometry is modified.
     * By default, it setups viewport.
     */
    virtual void
    resizeGL();
};

} /* namespace ilixi */
#endif /* GLWIDGET_H_ */
