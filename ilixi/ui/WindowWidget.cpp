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
#include <core/Logger.h>
#include <core/PlatformManager.h>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_WINDOWWIDGET, "ilixi/ui/WindowWidget", "WindowWidget");

IDirectFBSurface* WindowWidget::_exclusiveSurface = NULL;

WindowWidget::WindowWidget(Widget* parent)
        : Frame(parent),
          _backgroundFlags(BGFFill | BGFClear),
          _window(NULL),
          _eventManager(NULL),
          _layerName("ui")
{
    ILOG_TRACE_W(ILX_WINDOWWIDGET);
    setVisible(false);
    pthread_mutex_init(&_updates._listLock, NULL);
    sem_init(&_updates._updateReady, 0, 0);
    sem_init(&_updates._paintReady, 0, 1);

    _surface->setSurfaceFlag(Surface::WindowDescription);
    setMargins(5, 5, 5, 5);
    setNeighbours(this, this, this, this);

    if (PlatformManager::instance().appOptions() & OptExclusive)
    {
        if (Application::activeWindow())
            ILOG_THROW( ILX_WINDOWWIDGET, "Error cannot have multiple windows in exclusive mode!\n");
    } else
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

    Application::removeWindow(this);
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
    ILOG_TRACE_W(ILX_WINDOWWIDGET);
    if (!(_state & InvisibleState))
    {
        pthread_mutex_lock(&_updates._listLock);
        ILOG_DEBUG(ILX_WINDOWWIDGET, " -> using frameGeometry.\n");
        _updates._updateQueue.add(frameGeometry());
#ifdef ILIXI_STEREO_OUTPUT
        _updates._updateQueueRight.add(frameGeometry());
#endif
        pthread_mutex_unlock(&_updates._listLock);
        Application::__instance->updateFromWindow();
    }
}

void
WindowWidget::update(const PaintEvent& event)
{
    ILOG_TRACE_W(ILX_WINDOWWIDGET);
    if (!(_state & InvisibleState))
    {
        pthread_mutex_lock(&_updates._listLock);
        ILOG_DEBUG(ILX_WINDOWWIDGET, " -> left %d, %d, %d, %d.\n", event.rect.x(), event.rect.y(), event.rect.width(), event.rect.height());
        _updates._updateQueue.add(event.rect);
#ifdef ILIXI_STEREO_OUTPUT
        ILOG_DEBUG(ILX_WINDOWWIDGET, " -> right %d, %d, %d, %d.\n", event.right.x(), event.right.y(), event.right.width(), event.right.height());
        _updates._updateQueueRight.add(event.right);
#endif
        pthread_mutex_unlock(&_updates._listLock);
        Application::__instance->updateFromWindow();
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
                ILOG_DEBUG(ILX_WINDOWWIDGET, "  -> Left eye\n");
                evt.eye = PaintEvent::LeftEye;
                surface()->setStereoEye(evt.eye);
                surface()->clip(evt.rect);

                if (_backgroundFlags & BGFClear)
                surface()->clear(evt.rect);

                if (_backgroundFlags & BGFFill)
                compose(evt);

                paintChildren(evt);
//                PlatformManager::instance().renderCursor(AppBase::cursorPosition());

// Right eye
                ILOG_DEBUG(ILX_WINDOWWIDGET, "  -> Right eye\n");
                evt.eye = PaintEvent::RightEye;
                surface()->setStereoEye(evt.eye);
                surface()->clip(evt.right);

                if (_backgroundFlags & BGFClear)
                surface()->clear(evt.right);

                if (_backgroundFlags & BGFFill)
                compose(evt);

                paintChildren(evt);
                PlatformManager::instance().renderCursor(AppBase::cursorPosition());

                surface()->flipStereo(evt.rect, evt.right);
#else
                surface()->clip(evt.rect);
                if (_backgroundFlags & BGFClear)
                    surface()->clear(evt.rect);

                if (_backgroundFlags & BGFFill)
                    compose(evt);

                paintChildren(evt);

                PlatformManager::instance().renderCursor(Application::cursorPosition());

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

bool
WindowWidget::consumePointerEvent(const PointerEvent& pointerEvent)
{
    ILOG_TRACE_W(ILX_WINDOWWIDGET);
    if (visible() && (_rootWindow->_eventManager->grabbedWidget() == this || _frameGeometry.contains(pointerEvent.x, pointerEvent.y, true)))
    {
        if (_children.size())
        {
            for (WidgetListReverseIterator it = _children.rbegin(); it != _children.rend(); ++it)
                if (((Widget*) *it)->consumePointerEvent(pointerEvent))
                    return true;
        }

        _eventManager->setExposedWidget(NULL, pointerEvent);

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
    if (!Application::activeWindow() && (PlatformManager::instance().appOptions() & OptExclusive))
    {
        _exclusiveSurface = PlatformManager::instance().getLayerSurface(_layerName);
        if (_exclusiveSurface == NULL)
            ILOG_THROW(ILX_WINDOWWIDGET, "Couldn't get layer surface!\n");
        int w, h;
        _exclusiveSurface->GetSize(_exclusiveSurface, &w, &h);

        Application::addWindow(this);
        setSize(Size(w, h));
        setRootWindow(this);
    } else // DIALOGS
    {
        if (_window->_dfbWindow)
            return;

        if (_window->initDFBWindow(preferredSize()))
        {
            Application::addWindow(this);
            setSize(_window->windowSize());
            setRootWindow(this);
        }
    }

    setVisible(true);

    if (!_eventManager->focusedWidget())
        _eventManager->selectNext(this);

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

    if (!(PlatformManager::instance().appOptions() & OptExclusive))
        _window->releaseDFBWindow();
}

DFBWindowID
WindowWidget::windowID() const
{
    if (_window)
        return _window->windowID();
    return 0;
}

bool
WindowWidget::handleWindowEvent(const DFBWindowEvent& event)
{
//    ILOG_TRACE_W(ILX_WINDOWWIDGET);
    // handle all other events...
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
        return target->consumePointerEvent(PointerEvent(PointerButtonUp, event));

    case DWET_BUTTONDOWN:
        return target->consumePointerEvent(PointerEvent(PointerButtonDown, event));

    case DWET_MOTION:
        return target->consumePointerEvent(PointerEvent(PointerMotion, event));

    case DWET_WHEEL:
        return target->consumePointerEvent(PointerEvent(PointerWheel, event));

    case DWET_KEYUP:
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
    ILOG_TRACE_W(ILX_WINDOWWIDGET);

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
        if (PlatformManager::instance().useFSU(_layerName))
            _updates._updateRegion = frameGeometry();
        else
            _updates._updateRegion = updateTemp;
#endif

        sem_post(&_updates._updateReady);

        ILOG_DEBUG( ILX_WINDOWWIDGET, " -> UpdateRegion(%d, %d, %d, %d)\n", _updates._updateRegion.x(), _updates._updateRegion.y(), _updates._updateRegion.width(), _updates._updateRegion.height());

        long long micros = 0;

        if (_surface && _surface->dfbSurface())
        {
            // _surface->dfbSurface()->GetFrameTime( _surface->dfbSurface(), &micros );
            ILOG_DEBUG( ILX_WINDOWWIDGET, " -> GetFrameTime returned %lld (%lld advance)\n", micros, micros - direct_clock_get_time(DIRECT_CLOCK_MONOTONIC));
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
    if ((PlatformManager::instance().appOptions() & OptExclusive))
        return _exclusiveSurface;
    else if (_window)
        return _window->dfbSurface();
    return NULL;
}

std::string
WindowWidget::layerName() const
{
    return _layerName;
}

} /* namespace ilixi */
