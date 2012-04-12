/*
 * CompositorSurfaceView.h
 *
 *  Created on: Mar 30, 2012
 *      Author: tarik
 */

#ifndef COMPOSITORSURFACEVIEW_H_
#define COMPOSITORSURFACEVIEW_H_

#include "ui/SurfaceView.h"
#include "lib/TweenAnimation.h"

namespace ilixi
{

  class CompositorSurfaceView : public SurfaceView
  {
  public:
    CompositorSurfaceView(Widget* parent = 0);

    virtual
    ~CompositorSurfaceView();

    void
    show();

    void
    hide();

    IDirectFBWindow* _window;

  private:
    float _aniVal;
    float _scaleVal;
    TweenAnimation _ani;
    Tween* _opacityTween;
    Tween* _scaleTween;

    void
    tweenSlot();

    void
    tweenEndSlot();

    virtual void
    renderSource();
  };

} /* namespace ilixi */
#endif /* COMPOSITORSURFACEVIEW_H_ */
