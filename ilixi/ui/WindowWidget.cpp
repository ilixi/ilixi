/*
 Copyright 2010-2013 Tarik Sekmen.

 All Rights Reserved.

 Written by Tarik Sekmen <tarik@ilixi.org>,
            Denis Oliver Kropp <dok@directfb.org>.

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
#include <core/Application.h>
#include <core/EventFilter.h>
#include <core/Logger.h>
#include <core/PlatformManager.h>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_WINDOWWIDGET, "ilixi/ui/WindowWidget", "WindowWidget");
D_DEBUG_DOMAIN( ILX_WINDOWWIDGET_EVENTS, "ilixi/ui/WindowWidget/Events", "WindowWidget Events");
D_DEBUG_DOMAIN( ILX_WINDOWWIDGET_UPDATES, "ilixi/ui/WindowWidget/Updates", "WindowWidget Updates");

WindowWidget::WindowWidget(Widget* parent)
        : Frame(parent),
          _backgroundFlags(BGFFill | BGFClear),
          _window(NULL),
          _eventManager(NULL),
          _modality(WindowModal),
          _dragWindow(false)
{
    ILOG_TRACE_W(ILX_WINDOWWIDGET);
    setVisible(false);
    pthread_mutex_init(&_updates._listLock, NULL);
    sem_init(&_updates._updateReady, 0, 0);
    sem_init(&_updates._paintReady, 0, 1);

    _surface->setSurfaceFlag(Surface::WindowDescription);
    setMargins(5, 5, 5, 5);
    setNeighbours(this, this, this, this);

    _window = new Window();

    _eventManager = new EventManager(this);
    setRootWindow(this);
}

WindowWidget::~WindowWidget()
{
    ILOG_TRACE_W(ILX_WINDOWWIDGET);
    pthread_mutex_destroy(&_updates._listLock);
    sem_destroy(&_updates._updateReady);
    sem_destroy(&_updates._paintReady);

    closeWindow();
    delete _eventManager;
    delete _window;
}

bool
WindowWidget::backgroundFilled() const
{
    return _backgroundFlags & BGFFill;
}

void
WindowWidget::update()
{
    ILOG_TRACE_W(ILX_WINDOWWIDGET_UPDATES);
    if (!(_state & InvisibleState))
    {
        pthread_mutex_lock(&_updates._listLock);
        ILOG_DEBUG(ILX_WINDOWWIDGET_UPDATES, " -> using frameGeometry.\n");
        _updates._updateQueue.add(frameGeometry());
#ifdef ILIXI_STEREO_OUTPUT
        _updates._updateQueueRight.add(frameGeometry());
#endif
        pthread_mutex_unlock(&_updates._listLock);
    }
}

void
WindowWidget::update(const PaintEvent& event)
{
    ILOG_TRACE_W(ILX_WINDOWWIDGET_UPDATES);
    if (!(_state & InvisibleState))
    {
        pthread_mutex_lock(&_updates._listLock);
        ILOG_DEBUG(ILX_WINDOWWIDGET_UPDATES, " -> left %d, %d, %d, %d.\n", event.rect.x(), event.rect.y(), event.rect.width(), event.rect.height());
        _updates._updateQueue.add(event.rect);
#ifdef ILIXI_STEREO_OUTPUT
        ILOG_DEBUG(ILX_WINDOWWIDGET_UPDATES, " -> right %d, %d, %d, %d.\n", event.right.x(), event.right.y(), event.right.width(), event.right.height());
        _updates._updateQueueRight.add(event.right);
#endif
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
    ILOG_TRACE_W(ILX_WINDOWWIDGET_UPDATES);
    if (visible())
    {
        int ready;
        sem_getvalue(&_updates._updateReady, &ready);
        if (ready)
        {
            sem_wait(&_updates._updateReady);

            _surface->updateSurface(event);

#ifdef ILIXI_STEREO_OUTPUT
            PaintEvent evt(_frameGeometry.intersected(_updates._updateRegion), _frameGeometry.intersected(_updates._updateRegionRight));
#else
            PaintEvent evt(_frameGeometry.intersected(_updates._updateRegion));
#endif
            if (evt.isValid())
            {

#ifdef ILIXI_STEREO_OUTPUT
                // Left eye
                ILOG_DEBUG(ILX_WINDOWWIDGET_UPDATES, "  -> Left eye\n");
                evt.eye = PaintEvent::LeftEye;
                surface()->setStereoEye(evt.eye);
                surface()->clip(evt.rect);

                if (_backgroundFlags & BGFClear)
                surface()->clear(evt.rect);

                if (_backgroundFlags & BGFFill)
                compose(evt);

                paintChildren(evt);

// Right eye
                ILOG_DEBUG(ILX_WINDOWWIDGET_UPDATES, "  -> Right eye\n");
                evt.eye = PaintEvent::RightEye;
                surface()->setStereoEye(evt.eye);
                surface()->clip(evt.right);

                if (_backgroundFlags & BGFClear)
                surface()->clear(evt.right);

                if (_backgroundFlags & BGFFill)
                compose(evt);

                paintChildren(evt);
                // FIXME render cursor for stereo.
                PlatformManager::instance().renderCursor(AppBase::cursorPosition());

                surface()->flipStereo(evt.rect, evt.right);
#else
                surface()->clip(evt.rect);
                if (_backgroundFlags & BGFClear)
                    surface()->clear(evt.rect);

                if (_backgroundFlags & BGFFill)
                    compose(evt);

                paintChildren(evt);

                surface()->flip(evt.rect);
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
    if (fill)
        _backgroundFlags |= BGFFill;
    else
        _backgroundFlags &= ~BGFFill;
}

void
WindowWidget::setBackgroundClear(bool clear)
{
    if (clear)
        _backgroundFlags |= BGFClear;
    else
        _backgroundFlags &= ~BGFClear;
}

void
WindowWidget::setLayerName(const std::string& layerName)
{
    _window->_layerName = layerName;
}

WindowWidget::Modality
WindowWidget::modality() const
{
    return _modality;
}

void
WindowWidget::setModality(Modality modality)
{
    _modality = modality;
}

bool
WindowWidget::consumePointerEvent(const PointerEvent& pointerEvent)
{
    ILOG_TRACE_W(ILX_WINDOWWIDGET_EVENTS);
    PointerEvent event = pointerEvent;
    if (PlatformManager::instance().appOptions() & OptExclusive)
    {
        Point p = _window->windowPosition();
        event.x -= p.x();
        event.y -= p.y();
    }

    if (visible() && (_rootWindow->_eventManager->grabbedWidget() == this || _frameGeometry.contains(event.x, event.y, true)))
    {
        if (_eventFilter && _eventFilter->pointerEventConsumer(event))
            return true;

        if (_children.size())
        {
            for (WidgetListReverseIterator it = _children.rbegin(); it != _children.rend(); ++it)
                if (((Widget*) *it)->consumePointerEvent(event))
                    return true;
        }

        _eventManager->setExposedWidget(NULL, event);

        if (event.eventType == PointerButtonDown)
        {
            _eventManager->setFocusedWidget(NULL);
            pointerButtonDownEvent(event);
        }
    }
    return false;
}

bool
WindowWidget::consumeDragEvent(const PointerEvent& pointerEvent)
{
    ILOG_TRACE_W(ILX_WINDOWWIDGET_EVENTS);
    PointerEvent event = pointerEvent;
    if (PlatformManager::instance().appOptions() & OptExclusive)
    {
        Point p = _window->windowPosition();
        event.x -= p.x();
        event.y -= p.y();
    }

    if (visible() && enabled() && _frameGeometry.contains(event.x, event.y, true))
    {
        if (_inputMethod & AcceptsDrop)
        {
            if (pointerEvent.eventType == PointerButtonUp)
                dropEvent(event);
            else if (pointerEvent.eventType == PointerMotion)
            {
                if (_inputMethod & PointerTracking)
                    dragMotionEvent(pointerEvent);
                if (_rootWindow->_eventManager->exposedWidget() != this)
                    _rootWindow->_eventManager->setExposedWidget(this, event, true);
            }
            return true;
        } else if (_children.size())
        {
            for (WidgetListReverseIterator it = _children.rbegin(); it != _children.rend(); ++it)
                if (((Widget*) *it)->consumeDragEvent(event))
                    return true;
            _eventManager->setExposedWidget(NULL, event, true);
        }
    }
    return false;
}

void
WindowWidget::showWindow(const Point& position)
{
    ILOG_TRACE_W(ILX_WINDOWWIDGET);
    if (_window->_dfbWindow)
        return;

    if (_window->initDFBWindow(position, preferredSize()))
    {
        Application::addWindow(this);
        setSize(_window->windowSize());
        setRootWindow(this);
    } else
        ILOG_FATAL(ILX_WINDOWWIDGET, "Could not create window!\n");

    setVisible(true);

    if (!_eventManager->focusedWidget())
        _eventManager->selectNext(this);

    update(PaintEvent(Rectangle(0, 0, width(), height()), PaintEvent::BothEyes));
    updateWindow();

    if (!(PlatformManager::instance().appOptions() & OptExclusive))
        _window->showWindow();

    Application::setActiveWindow(this);
}

void
WindowWidget::closeWindow()
{
    ILOG_TRACE_W(ILX_WINDOWWIDGET);
    setVisible(false);

    if (!(PlatformManager::instance().appOptions() & OptExclusive))
        _window->hideWindow();

    _eventManager->clear();

    Application::removeWindow(this);

    setRootWindow(NULL);

    if (_window->dfbSurface() && (PlatformManager::instance().appOptions() & OptExclusive))
    {
        _window->dfbSurface()->SetDrawingFlags(_window->dfbSurface(), DSDRAW_NOFX);
        _window->dfbSurface()->Clear(_window->dfbSurface(), 0, 0, 0, 0);
        _window->dfbSurface()->Flip(_window->dfbSurface(), NULL, DSFLIP_NONE);
        _window->dfbSurface()->Clear(_window->dfbSurface(), 0, 0, 0, 0);
    }
    _window->releaseDFBWindow();
}

DFBWindowID
WindowWidget::windowID() const
{
    return _window->windowID();
}

bool
WindowWidget::handleWindowEvent(const DFBWindowEvent& event, bool dragging)
{
    ILOG_TRACE_W(ILX_WINDOWWIDGET_EVENTS);
    ILOG_DEBUG(ILX_WINDOWWIDGET_EVENTS, " -> event at %d, %d\n", event.x, event.y);

    // handle all other events...
    bool exclusive = (PlatformManager::instance().appOptions() & OptExclusive);
    if (exclusive)
    {
        if (!_window->windowRectangle().contains(event.x, event.y))
        {
            if (_eventManager->focusedWidget() && !(_eventManager->focusedWidget()->inputMethod() & OSKInput))
                _eventManager->setFocusedWidget(NULL);
            if (_eventManager->exposedWidget())
                _eventManager->setExposedWidget(NULL, PointerEvent(PointerMotion, event.x, event.y));
            if (_eventManager->grabbedWidget())
                _eventManager->setGrabbedWidget(NULL, PointerEvent(PointerMotion, event.x, event.y));
            return false;
        }
    } else if (!dragging && event.window_id != _window->windowID())
        return false;

    Widget* target = this;
    Widget* grabbed = _eventManager->grabbedWidget();
    if (grabbed && grabbed->acceptsPointerInput())
        target = grabbed;

    switch (event.type)
    {
    case DWET_GOTFOCUS:
        ILOG_DEBUG(ILX_WINDOWWIDGET, " -> DWET_GOTFOCUS\n");
        if (!_eventManager->focusedWidget())
            _eventManager->selectNext(this);
        return true;

    case DWET_LOSTFOCUS:
        ILOG_DEBUG(ILX_WINDOWWIDGET, " -> DWET_LOSTFOCUS\n");
        if (_eventManager->focusedWidget() && !(_eventManager->focusedWidget()->inputMethod() & OSKInput))
            _eventManager->setFocusedWidget(NULL);
        return true;

    case DWET_CLOSE: // handle Close, can be signalled by viewport manager.
        ILOG_DEBUG(ILX_WINDOWWIDGET, " -> DWET_CLOSE\n");
        sigAbort();
        return true;

    case DWET_LEAVE: // handle Leave, can be signalled if pointer moves outside window.
        ILOG_DEBUG(ILX_WINDOWWIDGET, " -> DWET_LEAVE\n");
        _eventManager->setExposedWidget(NULL, PointerEvent(PointerMotion, event.x, event.y));
        _eventManager->setGrabbedWidget(NULL, PointerEvent(PointerMotion, event.x, event.y));
        return true;

    case DWET_BUTTONUP:
        _eventManager->setGrabbedWidget(NULL, PointerEvent(PointerMotion, event.x, event.y));
        if (dragging)
            return consumeDragEvent(PointerEvent(PointerButtonUp, event));
        return target->consumePointerEvent(PointerEvent(PointerButtonUp, event));

    case DWET_BUTTONDOWN:
        return target->consumePointerEvent(PointerEvent(PointerButtonDown, event));

    case DWET_MOTION:
        if (dragging)
            return consumeDragEvent(PointerEvent(PointerMotion, event));
        return target->consumePointerEvent(PointerEvent(PointerMotion, event));

    case DWET_WHEEL:
        if (dragging)
            return false;
        return target->consumePointerEvent(PointerEvent(PointerWheel, event));

    case DWET_KEYUP:
        if (dragging)
            return false;
        switch (event.key_symbol)
        {
//        case DIKS_SHIFT:
        case DIKS_CONTROL:
        case DIKS_ALT:
        case DIKS_ALTGR:
        case DIKS_META:
        case DIKS_SUPER:
        case DIKS_HYPER:
        case DIKS_CAPS_LOCK:
        case DIKS_NUM_LOCK:
        case DIKS_SCROLL_LOCK:
            break;

        default:
            if (_eventManager->focusedWidget())
                return _eventManager->focusedWidget()->consumeKeyEvent(KeyEvent(KeyUpEvent, event));
            break;
        }
        return false;

    case DWET_KEYDOWN:
        if (dragging)
            return false;
        switch (event.key_symbol)
        {
//        case DIKS_SHIFT:
        case DIKS_CONTROL:
        case DIKS_ALT:
        case DIKS_ALTGR:
        case DIKS_META:
        case DIKS_SUPER:
        case DIKS_HYPER:
        case DIKS_CAPS_LOCK:
        case DIKS_NUM_LOCK:
        case DIKS_SCROLL_LOCK:
            break;

            // TODO Remove ESCAPE and sigAbort() ?
        case DIKS_ESCAPE:
            sigAbort();
            return true;

        case DIKS_BACK:
            sigAbort();
            return true;

        case DIKS_CURSOR_LEFT:
            if (_eventManager->grabbedWidget())
                _eventManager->grabbedWidget()->consumeKeyEvent(KeyEvent(KeyDownEvent, event));
            else if ((event.modifiers & DIMM_SHIFT) || _eventManager->oskWidget())
                _eventManager->focusedWidget()->consumeKeyEvent(KeyEvent(KeyDownEvent, event));
            else
                _eventManager->selectNeighbour(Left);
            return true;

        case DIKS_CURSOR_RIGHT:
            if (_eventManager->grabbedWidget())
                _eventManager->grabbedWidget()->consumeKeyEvent(KeyEvent(KeyDownEvent, event));
            else if ((event.modifiers & DIMM_SHIFT) || _eventManager->oskWidget())
                _eventManager->focusedWidget()->consumeKeyEvent(KeyEvent(KeyDownEvent, event));
            else
                _eventManager->selectNeighbour(Right);
            return true;

        case DIKS_CURSOR_UP:
            if (_eventManager->grabbedWidget())
                _eventManager->grabbedWidget()->consumeKeyEvent(KeyEvent(KeyDownEvent, event));
            else if (event.modifiers & DIMM_SHIFT)
                _eventManager->focusedWidget()->consumeKeyEvent(KeyEvent(KeyDownEvent, event));
            else
                _eventManager->selectNeighbour(Up);
            return true;

        case DIKS_CURSOR_DOWN:
            if (_eventManager->grabbedWidget())
                _eventManager->grabbedWidget()->consumeKeyEvent(KeyEvent(KeyDownEvent, event));
            else if (event.modifiers & DIMM_SHIFT)
                _eventManager->focusedWidget()->consumeKeyEvent(KeyEvent(KeyDownEvent, event));
            else
                _eventManager->selectNeighbour(Down);
            return true;

        case DIKS_OK:
        case DIKS_RETURN:
            {
                if (_eventManager->focusedWidget() && (_eventManager->focusedWidget()->inputMethod() & OSKInput))
                    return _eventManager->focusedWidget()->consumeKeyEvent(KeyEvent(KeyDownEvent, event));
                else
                {
                    if (_eventManager->grabbedWidget() != _eventManager->focusedWidget())
                        _eventManager->setGrabbedWidget(_eventManager->focusedWidget());
                    else
                        _eventManager->setGrabbedWidget(NULL);
                }
                return true;
            }
            break;

        case DIKS_TAB: // handle TAB release.
            if (event.modifiers == DIMM_SHIFT)
                _eventManager->selectPrevious();
            else
                _eventManager->selectNext();
            return true;

        default:
            if (_eventManager->focusedWidget())
                return _eventManager->focusedWidget()->consumeKeyEvent(KeyEvent(KeyDownEvent, event));
            else
                return false;
        } // end switch
        break;

//    else if (_eventManager->grabbedWidget())
//      return _eventManager->grabbedWidget()->consumeKeyEvent(
//          KeyEvent(KeyDownEvent, event.key_symbol, event.modifiers,
//              event.locks));

    default:
        return false;
    }
    return false;
}

void
WindowWidget::updateWindow()
{
    pthread_mutex_lock(&_updates._listLock);
    if (!_updates._updateQueue.valid)
    {
        pthread_mutex_unlock(&_updates._listLock);
        return;
    }
    ILOG_TRACE_W(ILX_WINDOWWIDGET_UPDATES);

    Rectangle updateTemp = _updates._updateQueue.rect;

    _updates._updateQueue.reset();

#ifdef ILIXI_STEREO_OUTPUT
    Rectangle updateTempRight = _updates._updateQueueRight.rect;

    _updates._updateQueueRight.reset();
#endif
    pthread_mutex_unlock(&_updates._listLock);

    if (!updateTemp.isNull())
    {
        sem_wait(&_updates._paintReady);
#ifdef ILIXI_STEREO_OUTPUT
        if (PlatformManager::instance().useFSU(_layerName))
        {
            _updates._updateRegion = frameGeometry();
            _updates._updateRegionRight = frameGeometry();
        } else
        {
            _updates._updateRegion = updateTemp;
            _updates._updateRegionRight = updateTempRight;
        }
#else
        if (PlatformManager::instance().useFSU(_window->_layerName))
            _updates._updateRegion = frameGeometry();
        else
            _updates._updateRegion = updateTemp;
#endif

        sem_post(&_updates._updateReady);

        ILOG_DEBUG( ILX_WINDOWWIDGET_UPDATES, " -> UpdateRegion(%d, %d, %d, %d)\n", _updates._updateRegion.x(), _updates._updateRegion.y(), _updates._updateRegion.width(), _updates._updateRegion.height());

        long long micros = 0;

        if (_surface && _surface->dfbSurface())
        {
            _surface->dfbSurface()->GetFrameTime( _surface->dfbSurface(), &micros );
            ILOG_DEBUG( ILX_WINDOWWIDGET_UPDATES, " -> GetFrameTime returned %lld (%lld advance)\n", micros, micros - direct_clock_get_time(DIRECT_CLOCK_MONOTONIC));
            Application::setFrameTime(micros);
        }

#ifdef ILIXI_STEREO_OUTPUT
        PaintEvent p(_updates._updateRegion, _updates._updateRegionRight);
        p.micros = micros;
        paint( p );
#else
        PaintEvent p(_updates._updateRegion, PaintEvent::BothEyes);
        p.micros = micros;
        paint(p);
#endif
    }
}

IDirectFBSurface*
WindowWidget::windowSurface()
{
    if (_window)
        return _window->dfbSurface();
    return NULL;
}

std::string
WindowWidget::layerName() const
{
    return _window->_layerName;
}

} /* namespace ilixi */
