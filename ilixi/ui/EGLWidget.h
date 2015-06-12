/*
 Copyright 2010-2015 Tarik Sekmen.

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
//! Provides a widget with its own EGL context.
/*!
 * This class simplifies using OpenGL within an ilixi application.
 *
 * You can create your own custom GL widgets by implementing virtual methods.
 */
class EGLWidget : public Widget
{
public:
    /*!
     * Constructor.
     *
     * Sets widget surface flags for EGL use.
     *
     * @param parent pointer to parent widget.
     */
    EGLWidget(Widget* parent = 0);

    /*!
     * Destructor.
     */
    virtual
    ~EGLWidget();

    /*!
     * Returns a pointer to EGLDisplay.
     */
    EGLDisplay
    eglDisplay() const;

    /*!
     * Returns a pointer to EGLContext.
     */
    EGLContext
    eglContext() const;

    /*!
     * Returns a pointer to EGLSurface.
     */
    EGLSurface
    eglSurface() const;

protected:
    /*!
     * Reimplement this method for drawing.
     *
     * By default, clears GL_COLOR_BUFFER_BIT only.
     */
    virtual void
    renderGL(const PaintEvent& event);

    /*!
     * Initializes surface and calls renderGL()
     */
    void
    compose(const PaintEvent& event);

    /*!
     * Set auto swap flag which specifies whether to swap buffers.
     *
     * By default autoSwap is set to true in constructor.
     */
    void
    setAutoSwap(bool autoSwap);

private:
    //! Flag is set to true once initialiseGL is called.
    bool _initialisedGL;
    //! This property stores EGL surface.
    EGLSurface _eglSurface;
    //! This property stores EGL context.
    EGLContext _eglContext;
    //! This property specifies whether to swap buffers after rendering.
    bool _autoSwap;

    /*!
     * Reimplement this method to initialise shaders etc.
     */
    virtual void
    initialiseGL();

    /*!
     * This function is called if widget's geometry is modified.
     */
    virtual void
    resizeGL();
};

} /* namespace ilixi */
#endif /* GLWIDGET_H_ */
