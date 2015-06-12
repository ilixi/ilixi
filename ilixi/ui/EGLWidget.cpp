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

#include <ui/EGLWidget.h>
#include <core/Logger.h>
#include <core/PlatformManager.h>

#include <GLES2/gl2.h>

namespace ilixi
{

D_DEBUG_DOMAIN(ILX_EGLWIDGET, "ilixi/ui/EGLWidget", "EGLWidget");

static EGLDisplay gEGLDisplay = NULL;
static EGLConfig gEGLConfig = NULL;
static EGLContext gEGLContext = NULL;

static EGLDisplay
sharedEGLDisplay()
{
    ILOG_TRACE_F(ILX_EGLWIDGET);
    if (!gEGLDisplay)
    {
        const char *s;
        gEGLDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY );
        if (gEGLDisplay == EGL_NO_DISPLAY )
        {
            ILOG_FATAL(ILX_EGLWIDGET, "EGL Error: GetDisplay failed!\n");
            return 0;
        }

        if (!eglInitialize(gEGLDisplay, 0, 0))
        {
            ILOG_FATAL(ILX_EGLWIDGET, "EGL Error: Could not initialize EGL!\n");
            return 0;
        }

        s = eglQueryString(gEGLDisplay, EGL_VERSION);
        ILOG_INFO(ILX_EGLWIDGET, "EGL_VERSION = %s\n", s);

        s = eglQueryString(gEGLDisplay, EGL_VENDOR);
        ILOG_INFO(ILX_EGLWIDGET, "EGL_VENDOR = %s\n", s);

        s = eglQueryString(gEGLDisplay, EGL_EXTENSIONS);
        ILOG_INFO(ILX_EGLWIDGET, "EGL_EXTENSIONS = %s\n", s);

        s = eglQueryString(gEGLDisplay, EGL_CLIENT_APIS);
        ILOG_INFO(ILX_EGLWIDGET, "EGL_CLIENT_APIS = %s\n", s);
    }
    return gEGLDisplay;
}

static EGLConfig
sharedEGLConfig(IDirectFBSurface* surface)
{
    ILOG_TRACE_F(ILX_EGLWIDGET);
    if (!gEGLConfig)
    {
        EGLint fbConfigAttrbs[] = { EGL_ALPHA_SIZE, 0, EGL_RED_SIZE, 0, EGL_GREEN_SIZE, 0, EGL_RED_SIZE, 0, EGL_DEPTH_SIZE, 16, EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT, EGL_SURFACE_TYPE, EGL_WINDOW_BIT, EGL_NONE };
        EGLint numOfConfig;

        if (!eglGetConfigAttribsDIRECTFB(sharedEGLDisplay(), (EGLNativeWindowType) surface, fbConfigAttrbs, 0))
            ILOG_FATAL(ILX_EGLWIDGET, "EGL Error: eglGetConfigAttribsDIRECTFB failed!\n");
        else if (!eglChooseConfig(sharedEGLDisplay(), fbConfigAttrbs, &gEGLConfig, 1, &numOfConfig))
            ILOG_FATAL(ILX_EGLWIDGET, "EGL Error: egl choose config failed!\n");
        else if (!numOfConfig)
            ILOG_FATAL(ILX_EGLWIDGET, "EGL Error: no egl configs found!\n");
        else
            ILOG_DEBUG(ILX_EGLWIDGET, "Got shared EGLConfig.\n");
    }
    return gEGLConfig;
}

static EGLContext
sharedEGLContext(IDirectFBSurface* surface)
{
    ILOG_TRACE_F(ILX_EGLWIDGET);
    if (!gEGLContext)
    {
        const EGLint eglContextAttrbs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
        gEGLContext = eglCreateContext(sharedEGLDisplay(), sharedEGLConfig(surface), EGL_NO_CONTEXT, eglContextAttrbs);
        if (gEGLContext)
        {
            if (eglBindAPI(EGL_OPENGL_ES_API) != EGL_TRUE)
                ILOG_FATAL(ILX_EGLWIDGET, "EGL Error: Could not set OPENGL_ES rendering!\n");
            else
                ILOG_DEBUG(ILX_EGLWIDGET, "Got shared EGLContext.\n");
        } else
            ILOG_FATAL(ILX_EGLWIDGET, "EGL Error: Could not create context!\n");
    }
    return gEGLContext;
}

EGLWidget::EGLWidget(Widget* parent)
        : Widget(parent),
          _initialisedGL(false),
          _eglSurface(NULL),
          _eglContext(NULL),
          _autoSwap(true)
{
    ILOG_TRACE_W(ILX_EGLWIDGET);
    surface()->unsetSurfaceFlag(Surface::SharedSurface);
    surface()->setSurfaceFlag(Surface::HasOwnSurface);
    surface()->setSurfaceFlag(Surface::ForceSingleSurface);
    surface()->setSurfaceFlag(Surface::DisableAutoFlip);
    sigGeometryUpdated.connect(sigc::mem_fun(this, &EGLWidget::resizeGL));
}

EGLWidget::~EGLWidget()
{
    ILOG_TRACE_W(ILX_EGLWIDGET);
}

EGLDisplay
EGLWidget::eglDisplay() const
{
    ILOG_TRACE_W(ILX_EGLWIDGET);
    return gEGLDisplay;
}

EGLContext
EGLWidget::eglContext() const
{
    ILOG_TRACE_W(ILX_EGLWIDGET);
    return _eglContext;
}

EGLSurface
EGLWidget::eglSurface() const
{
    ILOG_TRACE_W(ILX_EGLWIDGET);
    return _eglSurface;
}

void
EGLWidget::renderGL(const PaintEvent& event)
{
    ILOG_TRACE_W(ILX_EGLWIDGET);
//    glClear(GL_COLOR_BUFFER_BIT);
}

void
EGLWidget::compose(const PaintEvent& event)
{
    ILOG_TRACE_W(ILX_EGLWIDGET);

    if (!_eglSurface)
    {
        EGLint windowsAttr[] = { EGL_RENDER_BUFFER, ((PlatformManager::instance().getWindowSurfaceCaps() & DSCAPS_FLIPPING) && !(surface()->flags() & Surface::ForceSingleSurface)) ? EGL_BACK_BUFFER : EGL_SINGLE_BUFFER, EGL_NONE };

        _eglSurface = eglCreateWindowSurface(sharedEGLDisplay(), sharedEGLConfig(surface()->dfbSurface()), (EGLNativeWindowType) surface()->dfbSurface(), windowsAttr);
        if (_eglSurface == EGL_NO_SURFACE )
            ILOG_FATAL(ILX_EGLWIDGET, "EGL Error: Could not create surface!\n");
        else
            ILOG_INFO(ILX_EGLWIDGET, "Created EGLSurface %p\n", _eglSurface);
    }
    if (!_eglContext)
        _eglContext = sharedEGLContext(surface()->dfbSurface());
    eglMakeCurrent(sharedEGLDisplay(), _eglSurface, _eglSurface, _eglContext);

    if (!_initialisedGL)
    {
        initialiseGL();
        _initialisedGL = true;
    }

    ILOG_DEBUG(ILX_EGLWIDGET, " -> begin renderGL()\n");
    renderGL(event);
    ILOG_DEBUG(ILX_EGLWIDGET, " -> end renderGL()\n");

    if (_autoSwap) {
        ILOG_DEBUG(ILX_EGLWIDGET, " -> eglSwapBuffers()\n");
        eglSwapBuffers(sharedEGLDisplay(), _eglSurface);
    }

    if ((PlatformManager::instance().getWindowSurfaceCaps() & DSCAPS_FLIPPING) && !(surface()->flags() & Surface::ForceSingleSurface)) {
        ILOG_DEBUG(ILX_EGLWIDGET, " -> eglCopyBuffers()\n");
        eglCopyBuffers(sharedEGLDisplay(), _eglSurface, (EGLNativePixmapType) surface()->dfbSurface());
    }
}

void
EGLWidget::setAutoSwap(bool autoSwap)
{
    _autoSwap = autoSwap;
}

void
EGLWidget::initialiseGL()
{
    ILOG_TRACE_W(ILX_EGLWIDGET);
}

void
EGLWidget::resizeGL()
{
    ILOG_TRACE_W(ILX_EGLWIDGET);
//    glViewport(0, 0, (GLint) width(), (GLint) height());
}

} /* namespace ilixi */
