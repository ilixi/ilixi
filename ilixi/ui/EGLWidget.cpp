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

#include <ui/EGLWidget.h>
#include <core/Logger.h>
#include <core/PlatformManager.h>

#define EGL_EGLEXT_PROTOTYPES

#include <EGL/egl.h>
#include <EGL/egldfbext.h>

#include <GLES2/gl2.h>

namespace ilixi
{

D_DEBUG_DOMAIN(ILX_GLWIDGET, "ilixi/ui/EGLWidget", "EGLWidget");

static EGLDisplay gEGLDisplay = NULL;
static EGLConfig gEGLConfig = NULL;
static EGLContext gEGLContext = NULL;

static EGLDisplay
sharedEGLDisplay()
{
    if (!gEGLDisplay)
    {
        const char *s;
        gEGLDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY );
        if (gEGLDisplay == EGL_NO_DISPLAY ) {
            ILOG_FATAL(ILX_GLWIDGET, "EGL Error: GetDisplay failed!\n");
            return 0;
        }

        if (!eglInitialize(gEGLDisplay, 0, 0)) {
            ILOG_FATAL(ILX_GLWIDGET, "EGL Error: Could not initialize EGL!\n");
            return 0;
        }

        s = eglQueryString(gEGLDisplay, EGL_VERSION);
        ILOG_INFO(ILX_GLWIDGET, "EGL_VERSION = %s\n", s);

        s = eglQueryString(gEGLDisplay, EGL_VENDOR);
        ILOG_INFO(ILX_GLWIDGET, "EGL_VENDOR = %s\n", s);

        s = eglQueryString(gEGLDisplay, EGL_EXTENSIONS);
        ILOG_INFO(ILX_GLWIDGET, "EGL_EXTENSIONS = %s\n", s);

        s = eglQueryString(gEGLDisplay, EGL_CLIENT_APIS);
        ILOG_INFO(ILX_GLWIDGET, "EGL_CLIENT_APIS = %s\n", s);
    }
    return gEGLDisplay;
}

static EGLConfig
sharedEGLConfig(IDirectFBSurface* surface)
{
    if (!gEGLConfig)
    {
         EGLint fbConfigAttrbs[] = {
             EGL_ALPHA_SIZE,      0,
             EGL_RED_SIZE,        0,
             EGL_GREEN_SIZE,      0,
             EGL_RED_SIZE,        0,
             EGL_DEPTH_SIZE,      16,
             EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
             EGL_SURFACE_TYPE,    EGL_WINDOW_BIT,
             EGL_NONE
         };

        if (!eglGetConfigAttribsDIRECTFB(sharedEGLDisplay(), (EGLNativeWindowType) surface, fbConfigAttrbs, 0))
            ILOG_FATAL(ILX_GLWIDGET, "EGL Error: eglGetConfigAttribsDIRECTFB failed!\n");

        EGLint numOfConfig;
        if (!eglChooseConfig(sharedEGLDisplay(), fbConfigAttrbs, &gEGLConfig, 1, &numOfConfig))
            ILOG_FATAL(ILX_GLWIDGET, "EGL Error: egl choose config failed!\n");

        if (!numOfConfig)
            ILOG_FATAL(ILX_GLWIDGET, "EGL Error: no egl configs found!\n");
    }
    return gEGLConfig;
}

static EGLContext
sharedEGLContext(IDirectFBSurface* surface)
{
    if (!gEGLContext)
    {
        const EGLint eglContextAttrbs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
        gEGLContext = eglCreateContext(sharedEGLDisplay(), sharedEGLConfig(surface), EGL_NO_CONTEXT, eglContextAttrbs);
        if (gEGLContext)
        {
            if (eglBindAPI(EGL_OPENGL_ES_API) != EGL_TRUE)
                ILOG_FATAL(ILX_GLWIDGET, "EGL Error: Could not set OPENGL_ES rendering!\n");
        }
        else
            ILOG_FATAL(ILX_GLWIDGET, "EGL Error: Could not create context!\n");
    }
    return gEGLContext;
}

EGLWidget::EGLWidget(Widget* parent)
        : Widget(parent),
          _initialisedGL(false),
          _eglSurface(NULL)
{
    ILOG_TRACE_W(ILX_GLWIDGET);
    surface()->unsetSurfaceFlag(Surface::SharedSurface);
    surface()->setSurfaceFlag(Surface::HasOwnSurface);
    surface()->setSurfaceFlag(Surface::ForceSingleSurface);
    sigGeometryUpdated.connect(sigc::mem_fun(this, &EGLWidget::resizeGL));
}

EGLWidget::~EGLWidget()
{
    ILOG_TRACE_W(ILX_GLWIDGET);
}

void
EGLWidget::renderGL()
{
    ILOG_TRACE_W(ILX_GLWIDGET);
    glClear(GL_COLOR_BUFFER_BIT);
}

void
EGLWidget::compose(const PaintEvent& event)
{
    ILOG_TRACE_W(ILX_GLWIDGET);

    if(!_eglSurface) {
        EGLint windowsAttr[] = { EGL_RENDER_BUFFER, ((PlatformManager::instance().getWindowSurfaceCaps() & DSCAPS_FLIPPING) && !(surface()->flags() & Surface::ForceSingleSurface)) ? EGL_BACK_BUFFER : EGL_SINGLE_BUFFER, EGL_NONE };

        _eglSurface = eglCreateWindowSurface(sharedEGLDisplay(), sharedEGLConfig(surface()->dfbSurface()), (EGLNativeWindowType) surface()->dfbSurface(), windowsAttr);
        if (_eglSurface == EGL_NO_SURFACE )
            ILOG_FATAL(ILX_GLWIDGET, "EGL Error: Could not create surface!\n");
    }

    eglMakeCurrent(sharedEGLDisplay(), _eglSurface, _eglSurface, sharedEGLContext(surface()->dfbSurface()));

    if (!_initialisedGL)
    {
        initialiseGL();
        _initialisedGL = true;
    }

    renderGL();

    eglSwapBuffers(sharedEGLDisplay(), _eglSurface);

    if ((PlatformManager::instance().getWindowSurfaceCaps() & DSCAPS_FLIPPING) && !(surface()->flags() & Surface::ForceSingleSurface))
        eglCopyBuffers(sharedEGLDisplay(), _eglSurface, (EGLNativePixmapType) surface()->dfbSurface());
}

void
EGLWidget::initialiseGL()
{
    ILOG_TRACE_W(ILX_GLWIDGET);
}

void
EGLWidget::resizeGL()
{
    ILOG_TRACE_W(ILX_GLWIDGET);
    glViewport(0, 0, (GLint) width(), (GLint) height());
}

} /* namespace ilixi */
