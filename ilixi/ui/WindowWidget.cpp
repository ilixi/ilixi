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

#include <ui/WindowWidget.h>
#include <core/AppBase.h>
#include <core/Logger.h>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_WINDOWWIDGET, "ilixi/ui/WindowWidget", "WindowWidget");

IDirectFBSurface* WindowWidget::_exclusiveSurface = NULL;
IDirectFBSurface* WindowWidget::_cursorImage = NULL;

WindowWidget::WindowWidget(Widget* parent)
        : Frame(parent),
          _backgroundFlags(BGFAll),
          _window(NULL),
          _eventManager(NULL)
{
    ILOG_TRACE_W(ILX_WINDOWWIDGET);
    setVisible(false);
    pthread_mutex_init(&_updates._listLock, NULL);
    sem_init(&_updates._updateReady, 0, 0);
    sem_init(&_updates._paintReady, 0, 1);

    _surfaceDesc = WindowDescription;
    setMargins(5, 5, 5, 5);
    setNeighbours(this, this, this, this);

    if (AppBase::appOptions() & OptExclusive)
    {
        if (AppBase::activeWindow())
        {
            ILOG_ERROR( ILX_WINDOWWIDGET, "Error cannot have multiple windows in exclusive mode!\n");
            exit(1);
        }
    } else
        _window = new Window();

    _eventManager = new EventManager(this);
    setRootWindow(this);

    _update_timer.sigExec.connect(sigc::mem_fun(this, &WindowWidget::updateWindow));
}

WindowWidget::~WindowWidget()
{
    pthread_mutex_destroy(&_updates._listLock);
    sem_destroy(&_updates._updateReady);
    sem_destroy(&_updates._paintReady);

    AppBase::removeWindow(this);
    delete _eventManager;
    delete _window;
    if (AppBase::appOptions() & OptExclusive)
    {
        if (_exclusiveSurface)
            _exclusiveSurface->Release(_exclusiveSurface);
        if (_cursorImage)
            _cursorImage->Release(_cursorImage);
    }
    ILOG_TRACE_W(ILX_WINDOWWIDGET);
}

bool
WindowWidget::backgroundFilled() const
{
    return _backgroundFlags & BGFFill;
}

EventManager* const
WindowWidget::windowEventManager() const
{
    return _eventManager;
}

void
WindowWidget::update()
{
    if (!(_state & InvisibleState))
    {
        pthread_mutex_lock(&_updates._listLock);
        AppBase::__buffer->WakeUp(AppBase::__buffer);
        _updates._updateQueue.push_back(frameGeometry());
#ifdef ILIXI_STEREO_OUTPUT
        _updates._updateQueueRight.push_back(frameGeometry());
#endif

        if (!_update_timer.running())
        {
            _update_timer.start(10);
        }

        pthread_mutex_unlock(&_updates._listLock);
    }
}

void
WindowWidget::update(const PaintEvent& event)
{
    if (!(_state & InvisibleState))
    {
        pthread_mutex_lock(&_updates._listLock);
        AppBase::__buffer->WakeUp(AppBase::__buffer);
        _updates._updateQueue.push_back(event.rect);
#ifdef ILIXI_STEREO_OUTPUT
        _updates._updateQueueRight.push_back(event.right);
#endif

        if (!_update_timer.running())
        {
            _update_timer.start(10);
        }

        pthread_mutex_unlock(&_updates._listLock);
    }
}

void
WindowWidget::doLayout()
{
    ILOG_TRACE_W(ILX_WINDOWWIDGET);
    update();
}

void
WindowWidget::paint(const PaintEvent& event)
{
    ILOG_TRACE_W(ILX_WINDOWWIDGET);
    if (visible())
    {
        int ready;
        sem_getvalue(&_updates._updateReady, &ready);
        if (ready)
        {
            sem_wait(&_updates._updateReady);

            updateSurface(event);

#ifdef ILIXI_STEREO_OUTPUT
            PaintEvent evt(_frameGeometry.intersected(_updates._updateRegion),
                    _frameGeometry.intersected(_updates._updateRegionRight));
#else
            PaintEvent evt(_frameGeometry.intersected(_updates._updateRegion));
#endif
            if (evt.isValid())
            {

#ifdef ILIXI_STEREO_OUTPUT
                // Left eye
                ILOG_DEBUG(ILX_WINDOWWIDGET, "  -> Left eye\n");
                evt.eye = PaintEvent::LeftEye;
                surface()->setStereoEye(PaintEvent::LeftEye);
                surface()->clip(evt.rect);
                if (_backgroundFlags & BGFFill)
                {
                    surface()->clear(evt.rect);
                    compose(evt);
                }
                else if (_backgroundFlags & BGFClear)
                surface()->clear(evt.rect);

                paintChildren(evt);

                if (AppBase::appOptions() & OptExclusive)
                {
                    _exclusiveSurface->SetStereoEye(_exclusiveSurface, DSSE_LEFT);
                    _exclusiveSurface->SetBlittingFlags(_exclusiveSurface,
                            DSBLIT_BLEND_ALPHACHANNEL);
                    _exclusiveSurface->Blit(_exclusiveSurface, _cursorImage, NULL,
                            AppBase::cursorPosition().x + 10,
                            AppBase::cursorPosition().y);
                }

                // Right eye
                ILOG_DEBUG(ILX_WINDOWWIDGET, "  -> Right eye\n");
                evt.eye = PaintEvent::RightEye;
                surface()->setStereoEye(PaintEvent::RightEye);
                surface()->clip(evt.right);
                if (_backgroundFlags & BGFFill)
                {
                    surface()->clear(evt.right);
                    compose(evt);
                }
                else if (_backgroundFlags & BGFClear)
                surface()->clear(evt.right);

                paintChildren(evt);

                if (AppBase::appOptions() & OptExclusive)
                {
                    _exclusiveSurface->SetStereoEye(_exclusiveSurface,
                            DSSE_RIGHT);
                    _exclusiveSurface->SetBlittingFlags(_exclusiveSurface,
                            DSBLIT_BLEND_ALPHACHANNEL);
                    _exclusiveSurface->Blit(_exclusiveSurface, _cursorImage, NULL,
                            AppBase::cursorPosition().x - 10,
                            AppBase::cursorPosition().y);
                }
                if ((AppBase::appOptions() & OptExclusive)
                        && (AppBase::appOptions() & OptTripleAccelerated))
                surface()->flipStereo(evt.rect, evt.right, DSFLIP_ONSYNC);
                else
                surface()->flipStereo(evt.rect, evt.right, DSFLIP_WAITFORSYNC);
#else
                surface()->clip(evt.rect);
                if (_backgroundFlags & BGFClear)
                    surface()->clear(evt.rect);
                if (_backgroundFlags & BGFFill)
                    compose(evt);

                paintChildren(evt);

                if ((AppBase::appOptions() & OptExclusive) && !getenv("ILIXI_NO_CURSOR"))
                {
                    _exclusiveSurface->SetStereoEye(_exclusiveSurface, DSSE_LEFT);
                    _exclusiveSurface->SetBlittingFlags(_exclusiveSurface, DSBLIT_BLEND_ALPHACHANNEL);
                    _exclusiveSurface->Blit(_exclusiveSurface, _cursorImage, NULL, AppBase::cursorPosition().x, AppBase::cursorPosition().y);

                }
                if ((AppBase::appOptions() & OptExclusive) && (AppBase::appOptions() & OptTripleAccelerated))
                    surface()->flip(evt.rect, DSFLIP_ONSYNC);
                else
                    surface()->flip(evt.rect, DSFLIP_WAITFORSYNC);
#endif
            }
            sem_post(&_updates._paintReady);
        }
    }
}

void
WindowWidget::repaint(const PaintEvent& event)
{
    if (visible())
    {
        sem_wait(&_updates._paintReady);
        _updates._updateRegion = event.rect;
#ifdef ILIXI_STEREO_OUTPUT
        _updates._updateRegionRight = event.right;
#endif
        sem_post(&_updates._updateReady);
        paint(PaintEvent(_updates._updateRegion, PaintEvent::BothEyes));
    }
}

void
WindowWidget::setBackgroundFilled(bool fill)
{
    _backgroundFlags |= BGFAll;
}

bool
WindowWidget::consumePointerEvent(const PointerEvent& pointerEvent)
{
    if (visible() && (_rootWindow->_eventManager->grabbedWidget() == this || _frameGeometry.contains(pointerEvent.x, pointerEvent.y, true)))
    {
        if (_children.size())
        {
            for (WidgetListReverseIterator it = _children.rbegin();
                    it != _children.rend(); ++it)
                if (((Widget*) *it)->acceptsPointerInput() && ((Widget*) *it)->consumePointerEvent(pointerEvent))
                    return true;
        }

        if (pointerEvent.eventType == PointerButtonDown)
        {
            _eventManager->setFocusedWidget(NULL);
            pointerButtonDownEvent(pointerEvent);
        }
    }
    return false;
}

void
WindowWidget::showWindow()
{
    ILOG_TRACE_W(ILX_WINDOWWIDGET);
    if (AppBase::appOptions() & OptExclusive)
    {
        // setup cursor
        IDirectFBImageProvider* provider;
        DFBSurfaceDescription desc;
        if (AppBase::getDFB()->CreateImageProvider(AppBase::getDFB(), ILIXI_DATADIR"images/pointer.png", &provider) != DFB_OK)
            ILOG_THROW(ILX_WINDOWWIDGET, "Error while creating cursor image provider!\n");

        provider->GetSurfaceDescription(provider, &desc);
        desc.flags = (DFBSurfaceDescriptionFlags) (DSDESC_CAPS | DSDESC_WIDTH | DSDESC_HEIGHT | DSDESC_PIXELFORMAT);
        desc.caps = DSCAPS_PREMULTIPLIED;
        desc.pixelformat = DSPF_ARGB;

        if (AppBase::getDFB()->CreateSurface(AppBase::getDFB(), &desc, &_cursorImage) != DFB_OK)
            ILOG_THROW(ILX_WINDOWWIDGET, "Error while creating cursor surface!\n");

        provider->RenderTo(provider, _cursorImage, NULL);
        provider->Release(provider);

        // setup screen
        IDirectFBScreen* pScreen;
        DFBScreenEncoderConfig sEncoderCfg;
        DFBScreenEncoderDescription sEncoderDesc;

        AppBase::__layer->GetScreen(AppBase::__layer, &pScreen);
        sEncoderCfg.flags = (DFBScreenEncoderConfigFlags) (DSECONF_TV_STANDARD | DSECONF_SCANMODE | DSECONF_FREQUENCY | DSECONF_CONNECTORS | DSECONF_RESOLUTION | DSECONF_FRAMING);

        sEncoderCfg.tv_standard = DSETV_DIGITAL;
        sEncoderCfg.out_connectors = (DFBScreenOutputConnectors) (DSOC_COMPONENT | DSOC_HDMI);
        sEncoderCfg.scanmode = DSESM_PROGRESSIVE;
        sEncoderCfg.resolution = DSOR_1280_720;
        sEncoderCfg.frequency = DSEF_60HZ;

        /* Find the most appropriate framing encoder configuration */
        pScreen->GetEncoderDescriptions(pScreen, &sEncoderDesc);

        if (sEncoderDesc.all_framing & DSEPF_STEREO_FRAME_PACKING)
            sEncoderCfg.framing = DSEPF_STEREO_FRAME_PACKING;
        else if (sEncoderDesc.all_framing & DSEPF_STEREO_SIDE_BY_SIDE_FULL)
            sEncoderCfg.framing = DSEPF_STEREO_SIDE_BY_SIDE_FULL;
        else if (sEncoderDesc.all_framing & DSEPF_STEREO_SIDE_BY_SIDE_HALF)
            sEncoderCfg.framing = DSEPF_STEREO_SIDE_BY_SIDE_HALF;
        else if (sEncoderDesc.all_framing & DSEPF_STEREO_TOP_AND_BOTTOM)
            sEncoderCfg.framing = DSEPF_STEREO_TOP_AND_BOTTOM;
        else
            sEncoderCfg.framing = DSEPF_MONO;

        DFBResult err = pScreen->SetEncoderConfiguration(pScreen, 0, &sEncoderCfg);
        if (err == DFB_UNSUPPORTED)
        {
            sEncoderCfg.framing = DSEPF_MONO;
            pScreen->SetEncoderConfiguration(pScreen, 0, &sEncoderCfg);
        }
        pScreen->Release(pScreen);

        // setup layer
        DFBGraphicsDeviceDescription deviceDesc;
        AppBase::getDFB()->GetDeviceDescription(AppBase::getDFB(), &deviceDesc);

        DFBDisplayLayerConfig config;
        config.flags = (DFBDisplayLayerConfigFlags) (DLCONF_BUFFERMODE | DLCONF_OPTIONS);
        if (deviceDesc.acceleration_mask == DFXL_NONE)
        {
            config.buffermode = DLBM_BACKSYSTEM;
            AppBase::unSetAppOption(OptFullScreenUpdate);
            ILOG_DEBUG(ILX_WINDOWWIDGET, "Not using fullscreen updates\n");
        } else
        {
            config.buffermode = DLBM_BACKVIDEO;
            //AppBase::setAppOption(OptTripleAccelerated);
        }

#ifdef ILIXI_STEREO_OUTPUT
        config.options = DLOP_STEREO;
#else
        config.options = DLOP_NONE;
#endif

        if (AppBase::__layer->SetConfiguration(AppBase::__layer, &config) != DFB_OK)
        {
            ILOG_WARNING(ILX_WINDOWWIDGET, "Cannot set layer buffer mode to TRIPLE!\n");
            AppBase::unSetAppOption(OptTripleAccelerated);
            config.buffermode = DLBM_BACKVIDEO;
            if (AppBase::__layer->SetConfiguration(AppBase::__layer, &config) != DFB_OK)
                ILOG_THROW(ILX_WINDOWWIDGET, "Error while setting layer configuration!\n");
        }

        if (AppBase::__layer->GetSurface(AppBase::__layer, &_exclusiveSurface) != DFB_OK)
            ILOG_THROW(ILX_WINDOWWIDGET, "Error while getting layer surface!\n");

        int w, h;
        _exclusiveSurface->GetSize(_exclusiveSurface, &w, &h);

        AppBase::addWindow(this);
        setSize(Size(w, h));
        setRootWindow(this);
    } else
    {
        if (_window->_dfbWindow)
            return;

        if (_window->initDFBWindow(preferredSize()))
        {
            AppBase::addWindow(this);
            setSize(_window->windowSize());
            setRootWindow(this);
        }
    }

    setVisible(true);

    if (!_eventManager->focusedWidget())
        _eventManager->selectNeighbour(Right);

    update(PaintEvent(Rectangle(0, 0, width(), height()), PaintEvent::BothEyes));
    updateWindow();

#ifdef ILIXI_REDRAW_HACK
    update(PaintEvent(Rectangle(0, 0, width(), height()),
                    PaintEvent::BothEyes));
    updateWindow();

    update(PaintEvent(Rectangle(0, 0, width(), height()),
                    PaintEvent::BothEyes));
    updateWindow();
#endif

    if (!(AppBase::appOptions() & OptExclusive))
        _window->showWindow();

    AppBase::setActiveWindow(this);
    ILOG_TRACE_W(ILX_WINDOWWIDGET);
}

void
WindowWidget::closeWindow()
{
    ILOG_TRACE_W(ILX_WINDOWWIDGET);
    setVisible(false);

    if (!(AppBase::appOptions() & OptExclusive))
        _window->hideWindow();

    _eventManager->setGrabbedWidget(NULL);
    _eventManager->setExposedWidget(NULL);

    AppBase::removeWindow(this);

    invalidateSurface();

    if (!(AppBase::appOptions() & OptExclusive))
        _window->releaseDFBWindow();
}

bool
WindowWidget::handleWindowEvent(const DFBWindowEvent& event)
{
    // handle all other events...
    Widget* target = this;
    Widget* grabbed = _eventManager->grabbedWidget();
    if (grabbed && grabbed->acceptsPointerInput())
        target = grabbed;

    switch (event.type)
    {
    case DWET_CLOSE: // handle Close, can be signalled by viewport manager.
        sigAbort();
        return true;

    case DWET_LEAVE:// handle Leave, can be signalled if pointer moves outside window.
        _eventManager->setExposedWidget(NULL, PointerEvent(PointerMotion, event.x, event.y));
        _eventManager->setGrabbedWidget(NULL, PointerEvent(PointerMotion, event.x, event.y));
        return true;

    case DWET_BUTTONUP:
        return target->consumePointerEvent(PointerEvent(PointerButtonUp, event));

    case DWET_BUTTONDOWN:
        return target->consumePointerEvent(PointerEvent(PointerButtonDown, event));

    case DWET_MOTION:
        return target->consumePointerEvent(PointerEvent(PointerMotion, event));

    case DWET_WHEEL:
        return target->consumePointerEvent(PointerEvent(PointerWheel, event));

    case DWET_KEYUP:
        if (_eventManager->focusedWidget())
            return _eventManager->focusedWidget()->consumeKeyEvent(KeyEvent(KeyUpEvent, event));
        return false;

    case DWET_KEYDOWN:
        switch (event.key_symbol)
        {

        // TODO Remove ESCAPE and sigAbort() ?
        case DIKS_ESCAPE:
            sigAbort();
            return true;

        case DIKS_BACK:
            sigAbort();
            return true;

        case DIKS_CURSOR_LEFT:
            if (target == this)
            {
                _eventManager->selectNeighbour(Left);
                return true;
            }
            break;

        case DIKS_CURSOR_RIGHT:
            if (target == this)
            {
                _eventManager->selectNeighbour(Right);
                return true;
            }
            break;

        case DIKS_CURSOR_UP:
            if (target == this)
            {
                _eventManager->selectNeighbour(Up);
                return true;
            }
            break;

        case DIKS_CURSOR_DOWN:
            if (target == this)
            {
                _eventManager->selectNeighbour(Down);
                return true;
            }
            break;
// TODO Grab using key input.
//    case DIKS_OK:
//    case DIKS_RETURN:
//      if (_eventManager->focusedWidget())
//        {
//          if (_eventManager->focusedWidget() != target)
//            _eventManager->setGrabbedWidget(_eventManager->focusedWidget());
//          else
//            _eventManager->setGrabbedWidget(NULL);
//          return true;
//        }
//      break;

        case DIKS_TAB: // handle TAB release.
            if (event.modifiers == DIMM_SHIFT)
//        _eventManager->selectPrevious();
                ILOG_DEBUG( ILX_WINDOWWIDGET, "TAB %d\n", _eventManager->selectPrevious());
            else
//        _eventManager->selectNext();
                ILOG_DEBUG(ILX_WINDOWWIDGET, "TAB %d\n", _eventManager->selectNext());
            return true;

        } // end switch

//    else if (_eventManager->grabbedWidget())
//      return _eventManager->grabbedWidget()->consumeKeyEvent(
//          KeyEvent(KeyDownEvent, event.key_symbol, event.modifiers,
//              event.locks));

        if (_eventManager->focusedWidget())
            return _eventManager->focusedWidget()->consumeKeyEvent(KeyEvent(KeyDownEvent, event));
        else
            return false;

    default:
        return false;
    }
}

void
WindowWidget::updateWindow()
{
    pthread_mutex_lock(&_updates._listLock);
    int size = _updates._updateQueue.size();
    if (size == 0)
    {
        pthread_mutex_unlock(&_updates._listLock);
        return;
    }
    ILOG_TRACE_W(ILX_WINDOWWIDGET);

    Rectangle updateTemp = _updates._updateQueue[0];
    if (size > 1)
        for (int i = 1; i < size; ++i)
            updateTemp = updateTemp.united(_updates._updateQueue[i]);

    _updates._updateQueue.clear();

#ifdef ILIXI_STEREO_OUTPUT
    Rectangle updateTempRight = _updates._updateQueueRight[0];
    if (size > 1)
    for (int i = 1; i < size; ++i)
    updateTempRight = updateTempRight.united(_updates._updateQueueRight[i]);

    _updates._updateQueueRight.clear();
#endif
    pthread_mutex_unlock(&_updates._listLock);

    if (!updateTemp.isNull())
    {
        sem_wait(&_updates._paintReady);
#ifdef ILIXI_STEREO_OUTPUT
        if (AppBase::appOptions() & OptFullScreenUpdate)
        {
            _updates._updateRegion = frameGeometry();
            _updates._updateRegionRight = frameGeometry();
        }
        else
        {
            _updates._updateRegion = updateTemp;
            _updates._updateRegionRight = updateTempRight;
        }
#else
        if (AppBase::appOptions() & OptFullScreenUpdate)
            _updates._updateRegion = frameGeometry();
        else
            _updates._updateRegion = updateTemp;
#endif

        sem_post(&_updates._updateReady);
        ILOG_DEBUG( ILX_WINDOWWIDGET, " -> UpdateRegion(%d, %d, %d, %d)\n", _updates._updateRegion.x(), _updates._updateRegion.y(), _updates._updateRegion.width(), _updates._updateRegion.height());
#ifdef ILIXI_STEREO_OUTPUT
        paint(PaintEvent(_updates._updateRegion, _updates._updateRegionRight));
#else
        paint(PaintEvent(_updates._updateRegion, PaintEvent::BothEyes));
#endif
    }
}

IDirectFBSurface*
WindowWidget::windowSurface()
{
    if ((AppBase::appOptions() & OptExclusive))
        return _exclusiveSurface;
    else if (_window)
        return _window->dfbSurface();
    return NULL;
}

} /* namespace ilixi */
