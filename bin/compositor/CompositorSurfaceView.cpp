/*
 * CompositorSurfaceView.cpp
 *
 *  Created on: Mar 30, 2012
 *      Author: tarik
 */

#include "CompositorSurfaceView.h"

namespace ilixi
{

  CompositorSurfaceView::CompositorSurfaceView(Widget* parent) :
      SurfaceView(0, parent), _aniVal(0)
  {
    _ani.setDuration(200);
    _opacityTween = new Tween(Tween::SINE, Tween::EASE_OUT, _aniVal, 0, 1);
    _scaleTween = new Tween(Tween::SINE, Tween::EASE_IN, _scaleVal, 0, 1);
    _ani.addTween(_opacityTween);
    _ani.addTween(_scaleTween);
    _ani.sigExec.connect(
        sigc::mem_fun(this, &CompositorSurfaceView::tweenSlot));

    _ani.sigFinished.connect(
        sigc::mem_fun(this, &CompositorSurfaceView::tweenEndSlot));
    setVisible(false);
  }

  CompositorSurfaceView::~CompositorSurfaceView()
  {
  }

  void
  CompositorSurfaceView::show()
  {
    _ani.stop();
    setVisible(true);
    _opacityTween->setInitialValue(0);
    _opacityTween->setEndValue(255);
    _scaleTween->setInitialValue(40);
    _scaleTween->setEndValue(0);
    _ani.start();
  }

  void
  CompositorSurfaceView::hide()
  {
    _ani.stop();
    _opacityTween->setInitialValue(255);
    _opacityTween->setEndValue(0);
    _scaleTween->setInitialValue(0);
    _scaleTween->setEndValue(40);
    _ani.start();
  }

  void
  CompositorSurfaceView::tweenSlot()
  {
    update();
  }

  void
  CompositorSurfaceView::tweenEndSlot()
  {
    if (_aniVal == 0)
      setVisible(false);
  }

  void
  CompositorSurfaceView::renderSource()
  {
    if (_source)
      {
        IDirectFBSurface* dfbSurface = surface()->DFBSurface();
        dfbSurface->SetBlittingFlags(dfbSurface,
            (DFBSurfaceBlittingFlags) (DSBLIT_BLEND_ALPHACHANNEL
                | DSBLIT_BLEND_COLORALPHA));
        dfbSurface->SetColor(dfbSurface, 0, 0, 0, _aniVal);

        DFBRectangle rect =
          { _scaleVal, _scaleVal, width() - 2 * _scaleVal, height()
              - 2 * _scaleVal };
        dfbSurface->StretchBlit(dfbSurface, _source, NULL, &rect);
        dfbSurface->SetBlittingFlags(dfbSurface, DSBLIT_NOFX);
      }
  }

} /* namespace ilixi */
