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

#ifndef ILIXI_APPCOMPOSITOR_H_
#define ILIXI_APPCOMPOSITOR_H_

#include <ui/SurfaceView.h>
#include <compositor/AppInstance.h>
#include <vector>

namespace ilixi
{
class ILXCompositor;
//! Arranges SurfaceView(s) which belong to an Application.
class AppCompositor : public Widget
{
    friend class ILXCompositor;
public:
    AppCompositor(ILXCompositor* compositor, AppInstance* instance, Widget* parent = 0);

    virtual
    ~AppCompositor();

    AppInstance*
    instance() const;

    virtual void
    addWindow(IDirectFBWindow* window, bool eventHandling = true, bool blocking = true);

    void
    removeWindow(DFBWindowID windowID);

    float
    zoomFactor() const;

    void
    setZoomFactor(float zoomFactor);

    void
    setWindowFocus();

protected:
    enum AppCompState
    {
        APPCOMP_NONE,
        APPCOMP_READY
    };

    ILXCompositor* _compositor;
    AppInstance* _instance;
    AppCompState _cState;

    virtual void
    compose(const PaintEvent& event);

    /*!
     * This signal is emitted when child views are restacked.
     */
    sigc::signal<void> sigRestacked;

private:
    float _zoomFactor;
    float _hScale;
    float _vScale;

    virtual void
    updateAppCompositorGeometry();

    void
    onWindowConfig(DFBWindowID windowID, const SaWManWindowReconfig *reconfig);

    virtual void
    madeAvailable();
};

} /* namespace ilixi */
#endif /* ILIXI_APPCOMPOSITOR_H_ */
