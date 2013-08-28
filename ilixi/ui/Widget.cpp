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

#include <algorithm>
#include <core/Logger.h>
#include <core/Window.h>
#include <ui/Widget.h>
#include <ui/WindowWidget.h>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_WIDGET, "ilixi/ui/Widget", "Widget");

unsigned int Widget::_idCounter = 0;
StylistBase* Widget::_stylist = NULL;

Widget::Widget(Widget* parent)
        : _state(DefaultState),
          _inputMethod(NoInput),
          _id(_idCounter++),
          _z(0),
          _opacity(255),
          _parent(parent),
          _surface(new Surface(this)),
          _rootWindow(NULL),
          _preSelectedWidget(NULL),
          _xResizeConstraint(NoConstraint),
          _yResizeConstraint(NoConstraint)
{
    _neighbours[0] = NULL;
    _neighbours[1] = NULL;
    _neighbours[2] = NULL;
    _neighbours[3] = NULL;

    sigGeometryUpdated.connect(sigc::mem_fun(this, &Widget::updateFrameGeometry));
    ILOG_TRACE_W(ILX_WIDGET);
}

// TODO widget copy needs more work...
// e.g. children and surface are ignored.
Widget::Widget(const Widget& widget)
        : _state(widget._state),
          _inputMethod(widget._inputMethod),
          _opacity(widget._opacity),
          _parent(widget._parent),
          _surface(NULL),
          _rootWindow(widget._rootWindow),
          _preSelectedWidget(widget._preSelectedWidget),
          _xResizeConstraint(widget._xResizeConstraint),
          _yResizeConstraint(widget._yResizeConstraint)
{
    _id = _idCounter++;
    _z = 0;
    _neighbours[0] = widget._neighbours[0];
    _neighbours[1] = widget._neighbours[1];
    _neighbours[2] = widget._neighbours[2];
    _neighbours[3] = widget._neighbours[3];

    sigGeometryUpdated.connect(sigc::mem_fun(this, &Widget::updateFrameGeometry));
    ILOG_TRACE_W(ILX_WIDGET);
}

Widget::~Widget()
{
    ILOG_TRACE_W(ILX_WIDGET);
    if (eventManager())
        eventManager()->clear(this);

    for (WidgetListIterator it = _children.begin(); it != _children.end(); ++it)
        delete *it;
    delete _surface;
}

unsigned int
Widget::id() const
{
    return _id;
}

int
Widget::x() const
{
    return _surfaceGeometry.x();
}

int
Widget::y() const
{
    return _surfaceGeometry.y();
}

int
Widget::z() const
{
    if (_parent)
        return _parent->z() + _z;
    return _z;
}

int
Widget::absX() const
{
    return _frameGeometry.x();
}

int
Widget::absY() const
{
    return _frameGeometry.y();
}

Point
Widget::position() const
{
    return _surfaceGeometry;
}

Point
Widget::absPosition() const
{
    return Point(_frameGeometry.x(), _frameGeometry.y());
}

int
Widget::height() const
{
    return _frameGeometry.height();
}

int
Widget::width() const
{
    return _frameGeometry.width();
}

Size
Widget::size() const
{
    return _frameGeometry.size();
}

const Rectangle&
Widget::frameGeometry() const
{
    return _frameGeometry;
}

Rectangle
Widget::surfaceGeometry() const
{
    return Rectangle(_surfaceGeometry.x(), _surfaceGeometry.y(), _frameGeometry.width(), _frameGeometry.height());
}

int
Widget::minHeight() const
{
    return _minSize.height();
}

int
Widget::minWidth() const
{
    return _minSize.width();
}

Size
Widget::minSize() const
{
    return _minSize;
}

int
Widget::maxHeight() const
{
    return _maxSize.height();
}

int
Widget::maxWidth() const
{
    return _maxSize.width();
}

Size
Widget::maxSize() const
{
    return _maxSize;
}

int
Widget::heightForWidth(int width) const
{
    return -1;
}

Size
Widget::preferredSize() const
{
    return Size();
}

bool
Widget::enabled() const
{
    if (_parent)
        return !(_state & DisabledState) && _parent->enabled();
    return !(_state & DisabledState);
}

bool
Widget::visible() const
{
    if (_parent)
        return (!(_state & InvisibleState) && _parent->visible());
    return !(_state & InvisibleState);
}

u8
Widget::opacity() const
{
    if (_parent)
        return (_parent->opacity() / 255.0) * _opacity;
    return _opacity;
}

bool
Widget::hasFocus() const
{
    return _state & FocusedState;
}

bool
Widget::exposed() const
{
    return _state & ExposedState;
}

bool
Widget::pressed() const
{
    return _state & PressedState;
}

WidgetState
Widget::state() const
{
    return _state;
}

bool
Widget::acceptsPointerInput() const
{
    return (_inputMethod & PointerInput) && visible() && enabled();
}

bool
Widget::acceptsKeyInput() const
{
    return (_inputMethod & KeyInput) && visible() && enabled();
}

WidgetInputMethod
Widget::inputMethod() const
{
    return _inputMethod;
}

WidgetResizeConstraint
Widget::xConstraint() const
{
    return _xResizeConstraint;
}

WidgetResizeConstraint
Widget::yConstraint() const
{
    return _yResizeConstraint;
}

void
Widget::moveTo(int x, int y)
{
    if (x != _surfaceGeometry.x() || y != _surfaceGeometry.y())
    {
        ILOG_TRACE_W(ILX_WIDGET);
        ILOG_DEBUG(ILX_WIDGET, " -> P(%d, %d)\n", x, y);
        _surfaceGeometry.moveTo(x, y);
        _dirtyFrameGeometry.moveTo(_frameGeometry.x(), _frameGeometry.y());
        _frameGeometry.setX(_parent ? _surfaceGeometry.x() + _parent->_frameGeometry.x() : _surfaceGeometry.x());
        _frameGeometry.setY(_parent ? _surfaceGeometry.y() + _parent->_frameGeometry.y() : _surfaceGeometry.y());
        _surface->setSurfaceFlag(Surface::ModifiedPosition);
    }
}

void
Widget::moveTo(const Point& point)
{
    moveTo(point.x(), point.y());
}

void
Widget::translate(int deltaX, int deltaY)
{
    if (deltaX != 0 || deltaY != 0)
    {
        _surfaceGeometry.translate(deltaX, deltaY);
        _dirtyFrameGeometry.moveTo(_frameGeometry.x(), _frameGeometry.y());
        _frameGeometry.setX(_parent ? _surfaceGeometry.x() + _parent->_frameGeometry.x() : _surfaceGeometry.x());
        _frameGeometry.setY(_parent ? _surfaceGeometry.y() + _parent->_frameGeometry.y() : _surfaceGeometry.y());
        _surface->setSurfaceFlag(Surface::ModifiedPosition);
    }
}

void
Widget::setX(int x)
{
    if (x != _surfaceGeometry.x())
    {
        _surfaceGeometry.setX(x);
        _dirtyFrameGeometry.setX(_frameGeometry.x());
        _frameGeometry.setX(_parent ? _surfaceGeometry.x() + _parent->_frameGeometry.x() : _surfaceGeometry.x());
        _surface->setSurfaceFlag(Surface::ModifiedPosition);
    }
}

void
Widget::setY(int y)
{
    if (y != _surfaceGeometry.y())
    {
        _surfaceGeometry.setY(y);
        _dirtyFrameGeometry.setY(_frameGeometry.y());
        _frameGeometry.setY(_parent ? _surfaceGeometry.y() + _parent->_frameGeometry.y() : _surfaceGeometry.y());
        _surface->setSurfaceFlag(Surface::ModifiedPosition);
    }
}

void
Widget::setZ(int z)
{
    if (z != _z)
    {
        if (_parent)
            _parent->_surface->setSurfaceFlag(Surface::DoZSort);
        _z = z;
        _surface->setSurfaceFlag(Surface::ModifiedPosition);
    }
}

void
Widget::setHeight(int height)
{
    if (height != _frameGeometry.height())
    {
        ILOG_TRACE_W(ILX_WIDGET);
        ILOG_DEBUG(ILX_WIDGET, " -> H(%d)\n", height);
        if (height < _minSize.height())
            height = _minSize.height();
        else if (_maxSize.height() > 0 && height > _maxSize.height())
            height = _maxSize.height();
        _frameGeometry.setHeight(height);
        _surface->setSurfaceFlag(Surface::ModifiedSize);
    }
}

void
Widget::setWidth(int width)
{
    if (width != _frameGeometry.width())
    {
        ILOG_TRACE_W(ILX_WIDGET);
        ILOG_DEBUG(ILX_WIDGET, " -> W(%d)\n", width);
        if (width < _minSize.width())
            width = _minSize.width();
        else if (_maxSize.width() > 0 && width > _maxSize.width())
            width = _maxSize.width();
        _frameGeometry.setWidth(width);
        _surface->setSurfaceFlag(Surface::ModifiedSize);
    }
}

void
Widget::setSize(int width, int height)
{
    setWidth(width);
    setHeight(height);
}

void
Widget::setSize(const Size &size)
{
    setWidth(size.width());
    setHeight(size.height());
}

void
Widget::setGeometry(int x, int y, int width, int height)
{
    moveTo(x, y);
    setWidth(width);
    setHeight(height);
}

void
Widget::setGeometry(const Rectangle& rect)
{
    setGeometry(rect.x(), rect.y(), rect.width(), rect.height());
}

void
Widget::setMinimumSize(const Size &size)
{
    _minSize = size;
}

void
Widget::setMinimumSize(int minWidth, int minHeight)
{
    _minSize.setWidth(minWidth);
    _minSize.setHeight(minHeight);
}

void
Widget::setMaximumSize(const Size &size)
{
    _maxSize = size;
}

void
Widget::setMaximumSize(int maxWidth, int maxHeight)
{
    _maxSize.setWidth(maxWidth);
    _maxSize.setHeight(maxHeight);
}

void
Widget::setXConstraint(WidgetResizeConstraint constraint)
{
    _xResizeConstraint = constraint;
}

void
Widget::setYConstraint(WidgetResizeConstraint constraint)
{
    _yResizeConstraint = constraint;
}

void
Widget::setConstraints(WidgetResizeConstraint x, WidgetResizeConstraint y)
{
    _xResizeConstraint = (WidgetResizeConstraint) x;
    _yResizeConstraint = (WidgetResizeConstraint) y;
}

void
Widget::setEnabled()
{
    if ((_state & DisabledState))
    {
        _state = (WidgetState) (_state & ~DisabledState);
        sigStateChanged(this, _state);
        update();
    }
}

void
Widget::setDisabled()
{
    if (!(_state & DisabledState))
    {
        if (_state & FocusedState)
            _rootWindow->_eventManager->setFocusedWidget(NULL);

        _state = (WidgetState) (_state | DisabledState);
        sigStateChanged(this, _state);
        update();
    }
}

void
Widget::setVisible(bool visible)
{
    if (visible && (_state & InvisibleState))
    {
        _state = (WidgetState) (_state & ~InvisibleState);
        sigStateChanged(this, _state);
        doLayout();
    } else if (!visible && !(_state & InvisibleState))
    {
        _state = (WidgetState) (_state | InvisibleState);
        sigStateChanged(this, _state);
        doLayout();
    }
}

void
Widget::setOpacity(u8 opacity)
{
    if (opacity != _opacity)
    {
        _opacity = opacity;
        if (_opacity == 0)
            setVisible(false);
    }
}

void
Widget::setFocus()
{
    if (!(_state & FocusedState) && _rootWindow)
        _rootWindow->_eventManager->setFocusedWidget(this);
}

void
Widget::clearFocus()
{
    if ((_state & FocusedState) && _rootWindow)
        _rootWindow->_eventManager->setFocusedWidget(NULL);
}

void
Widget::setInputMethod(WidgetInputMethod method)
{
    _inputMethod = method;
}

void
Widget::setPointerTracking(bool pointerTracking)
{
    if (pointerTracking)
        _inputMethod = (WidgetInputMethod) (_inputMethod | PointerInputTracking);
    else
        _inputMethod = (WidgetInputMethod) (_inputMethod & ~PointerInputTracking);
}

void
Widget::setParent(Widget* parent)
{
//    if (parent != _parent)
    {
        // remove me from old parent's children list...
        if (_parent)
        {
            WidgetListIterator it = std::find(_parent->_children.begin(), _parent->_children.end(), this);
            if (this == *it)
                _parent->_children.erase(it);
        }

        _parent = parent;
        // Fixme move required?
        //      moveTo(0, 0);
        setRootWindow(_parent->_rootWindow);
    }
}

bool
Widget::lower()
{
    if (_parent)
        return _parent->lowerChild(this);
    return false;
}

bool
Widget::raise()
{
    if (_parent)
        return _parent->raiseChild(this);
    return false;
}

bool
Widget::sendToBack()
{
    if (_parent)
        return _parent->lowerChildToBottom(this);
    return false;
}

bool
Widget::bringToFront()
{
    if (_parent)
        return _parent->raiseChildToFront(this);
    return false;
}

void
Widget::paint(const PaintEvent& event)
{
    if (visible())
    {
        ILOG_TRACE_W(ILX_WIDGET);
        PaintEvent evt(this, event);
        if (evt.isValid())
        {
            compose(evt);
            paintChildren(evt);
        }
    }
}

void
Widget::repaint()
{
    if (visible())
    {
        if (_parent)
            _rootWindow->repaint(PaintEvent(_frameGeometry, z()));
        else
            repaint(PaintEvent(_frameGeometry, z()));
    }
}

void
Widget::repaint(const PaintEvent& event)
{
    if (visible())
    {
        if (_parent)
        {
            if (_surface->flags() & Surface::HasOwnSurface)
            {
                _surface->clear(mapToSurface(event.rect));
                _parent->repaint(PaintEvent(this, event));
            } else
                _parent->repaint(event);
        } else
            repaint(event);
    }
}

void
Widget::update()
{
    if (visible())
    {
        if (_rootWindow) // FIXME invis check
            _rootWindow->update(PaintEvent(_frameGeometry.united(_dirtyFrameGeometry), z()));
        else if ((_surface->flags() & Surface::HasOwnSurface) || (_surface->flags() & Surface::RootSurface))
            paint(PaintEvent(_frameGeometry.united(_dirtyFrameGeometry), z()));
    }
    _dirtyFrameGeometry = _frameGeometry;
}

void
Widget::update(const PaintEvent& event)
{
    if (visible())
    {
        if (_parent)
        {
            if (_surface->flags() & Surface::HasOwnSurface)
            {
                _surface->clear(mapToSurface(event.rect));
                _parent->update(PaintEvent(this, event));
            } else
                _parent->update(event);
        } else if ((_surface->flags() & Surface::HasOwnSurface) || (_surface->flags() & Surface::RootSurface))
            paint(event);
    }
}

void
Widget::doLayout()
{
    if (_parent)
        _parent->doLayout();
}

Widget*
Widget::getNeighbour(Direction direction) const
{
    return _neighbours[direction];
}

void
Widget::setNeighbour(Direction direction, Widget* neightbour)
{
    _neighbours[direction] = neightbour;
}

void
Widget::setNeighbours(Widget* up, Widget* down, Widget* left, Widget* right)
{
    _neighbours[Up] = up;
    _neighbours[Down] = down;
    _neighbours[Left] = left;
    _neighbours[Right] = right;
}

Rectangle
Widget::mapToSurface(const Rectangle& rect) const
{
    return mapToSurface(rect.x(), rect.y(), rect.width(), rect.height());
}

Rectangle
Widget::mapToSurface(int x, int y, int width, int height) const
{
    if (_surface->flags() & Surface::HasOwnSurface)
        return Rectangle(x - (_surfaceGeometry.x() + _frameGeometry.x()), y - (_surfaceGeometry.y() + _frameGeometry.y()), width, height);
    else
        return Rectangle(x - _frameGeometry.x(), y - _frameGeometry.y(), width, height);
}

Point
Widget::mapToSurface(const Point& point) const
{
    if (_surface->flags() & Surface::HasOwnSurface)
        return Point(point.x() - (_surfaceGeometry.x() + _frameGeometry.x()), point.y() - (_surfaceGeometry.y() + _frameGeometry.y()));
    else
        return Point(point.x() - _frameGeometry.x(), point.y() - _frameGeometry.y());
}

Rectangle
Widget::mapFromSurface(const Rectangle& rect) const
{
    return Rectangle(_frameGeometry.x() + rect.x(), _frameGeometry.y() + rect.y(), rect.width(), rect.height());
}

Rectangle
Widget::mapFromSurface(int x, int y, int width, int height) const
{
    return Rectangle(_frameGeometry.x() + x, _frameGeometry.y() + y, width, height);
}

Point
Widget::mapFromSurface(const Point& point) const
{
    return Point(_frameGeometry.x() + point.x(), _frameGeometry.y() + point.y());
}

bool
Widget::consumePointerEvent(const PointerEvent& pointerEvent)
{
    if (visible() && enabled() && (_rootWindow->_eventManager->grabbedWidget() == this || _frameGeometry.contains(pointerEvent.x, pointerEvent.y, true)))
    {
        if (_inputMethod & PointerInput)
        {
            if (_rootWindow->_eventManager->grabbedWidget() != this && (_inputMethod & PointerGrabbing) && ((pointerEvent.buttonMask & ButtonMaskLeft) && (pointerEvent.eventType == PointerMotion)))
                _rootWindow->_eventManager->setGrabbedWidget(this, pointerEvent);
            else if ((_inputMethod & PointerGrabbing) && (pointerEvent.eventType == PointerWheel))
            {
                pointerWheelEvent(pointerEvent);
                return true;
            } else if (_children.size())
            {
                for (WidgetListReverseIterator it = _children.rbegin(); it != _children.rend(); ++it)
                    if (((Widget*) *it)->consumePointerEvent(pointerEvent))
                        return true;
            }

            if (pointerEvent.eventType == PointerButtonDown)
            {
                _state = (WidgetState) (_state | PressedState);
                _rootWindow->_eventManager->setFocusedWidget(this);
                pointerButtonDownEvent(pointerEvent);
            } else if (pointerEvent.eventType == PointerButtonUp)
            {
                _state = (WidgetState) (_state & ~PressedState);
                pointerButtonUpEvent(pointerEvent);
                if (_state & GrabbedState)
                    _rootWindow->_eventManager->setGrabbedWidget(NULL, pointerEvent);
            } else if (pointerEvent.eventType == PointerWheel)
            {
                _rootWindow->_eventManager->setFocusedWidget(this);
                pointerWheelEvent(pointerEvent);
            } else if (pointerEvent.eventType == PointerMotion)
            {
                if (_inputMethod & PointerTracking)
                    pointerMotionEvent(pointerEvent);
                if (_rootWindow->_eventManager->exposedWidget() != this)
                    _rootWindow->_eventManager->setExposedWidget(this, pointerEvent);
            }
            return true;
        } else if ((_inputMethod & PointerPassthrough) && _children.size())
        {
            for (WidgetListReverseIterator it = _children.rbegin(); it != _children.rend(); ++it)
                if (((Widget*) *it)->consumePointerEvent(pointerEvent))
                    return true;
        }
    }
    return false;
}

bool
Widget::consumeKeyEvent(const KeyEvent& keyEvent)
{
    if (_inputMethod & KeyInput)
    {
        if (keyEvent.eventType == KeyUpEvent)
            keyUpEvent(keyEvent);
        else
            keyDownEvent(keyEvent);
        return true;
    } else
    {
        for (WidgetListReverseIterator it = _children.rbegin(); it != _children.rend(); ++it)
            if (((Widget*) *it)->consumeKeyEvent(keyEvent))
                return true;
    }
    return false;
}

StylistBase* const
Widget::stylist()
{
    return _stylist;
}

Widget*
Widget::parent() const
{
    return _parent;
}

Surface*
Widget::surface() const
{
    return _surface;
}

EventManager*
Widget::eventManager() const
{
    if (_rootWindow)
        return _rootWindow->_eventManager;
    return NULL;
}

bool
Widget::isChild(Widget* child)
{
    if (!child)
        return false;

    WidgetListIterator it = std::find(_children.begin(), _children.end(), child);
    if (child == *it)
        return true;
    return false;
}

////////////////////////////////////////////////////////////////////////////////////
// Protected methods...
////////////////////////////////////////////////////////////////////////////////////
bool
Widget::addChild(Widget* child)
{
    if (!child)
        return false;

    WidgetListIterator it = std::find(_children.begin(), _children.end(), child);
    if (child == *it)
    {
        ILOG_WARNING(ILX_WIDGET, "Widget %p is already a child.", child);
        return false;
    }

    child->setParent(this);
    _children.push_back(child);

    // Fixme this might be unnecessary since layout should do it.
    child->setNeighbours(getNeighbour(Up), getNeighbour(Down), getNeighbour(Left), getNeighbour(Right));

    ILOG_DEBUG(ILX_WIDGET, "Added child %p\n", child);
    return true;
}

bool
Widget::removeChild(Widget* child, bool destroy)
{
    if (!child)
        return false;

    WidgetListIterator it = std::find(_children.begin(), _children.end(), child);
    if (child == *it)
    {
        if (destroy)
            delete *it;
        _children.erase(it);
        ILOG_DEBUG(ILX_WIDGET, "Removed child %p\n", child);
        return true;
    }
    return false;
}

bool
Widget::insertChild(Widget* child, unsigned int index)
{
    if (index > _children.size())
        return false;

    removeChild(child, false);
    WidgetListIterator it = _children.begin();
    for (unsigned node = 0; node < index; ++node)
        ++it;

    _children.insert(it, child);
    return false;
}

bool
Widget::raiseChildToFront(Widget* child)
{
    if (!child)
        return false;

    if (_children.size() == 1)
        return false;

    WidgetListIterator it = std::find(_children.begin(), _children.end(), child);
    if (child == *it && it != _children.end())
    {
        _children.erase(it);
        _children.push_back(child);
        return true;
    }
    return false;
}

bool
Widget::lowerChildToBottom(Widget* child)
{
    if (!child)
        return false;

    if (_children.size() == 1)
        return false;

    WidgetListIterator it = std::find(_children.begin(), _children.end(), child);
    if (child == *it && it != _children.begin())
    {
        _children.erase(it);
        _children.push_front(child);
        return true;
    }
    return false;
}

bool
Widget::raiseChild(Widget* child)
{
    if (!child)
        return false;

    if (_children.size() == 1)
        return false;

    WidgetListIterator it = std::find(_children.begin(), _children.end(), child);
    if (child == *it && it != _children.end())
    {
        WidgetListIterator temp = it;
        temp++;
        if (temp != _children.end())
        {
            std::iter_swap(it, temp);
            return true;
        }
        return false;
    }
    return false;
}

bool
Widget::lowerChild(Widget* child)
{
    if (!child)
        return false;

    if (_children.size() == 1)
        return false;

    WidgetListIterator it = std::find(_children.begin(), _children.end(), child);
    if (child == *it && it != _children.begin())
    {
        WidgetListIterator temp = it;
        temp--;
        if (temp != _children.begin())
        {
            std::iter_swap(it, temp);
            return true;
        }
        return false;
    }
    return false;
}

void
Widget::paintChildren(const PaintEvent& event)
{
    ILOG_TRACE_W(ILX_WIDGET);
    for (WidgetListIterator it = _children.begin(); it != _children.end(); ++it)
        ((Widget*) *it)->paint(event);
}

void
Widget::updateFrameGeometry()
{
    _dirtyFrameGeometry.moveTo(_frameGeometry.x(), _frameGeometry.y());
    _frameGeometry.setX(_parent ? _surfaceGeometry.x() + _parent->_frameGeometry.x() : _surfaceGeometry.x());
    _frameGeometry.setY(_parent ? _surfaceGeometry.y() + _parent->_frameGeometry.y() : _surfaceGeometry.y());

    ILOG_DEBUG( ILX_WIDGET, "Widget %d updateFrameGeometry( %d, %d)\n", _id, _frameGeometry.x(), _frameGeometry.y());

    Surface::SurfaceFlags flags = (Surface::SurfaceFlags) ((_surface->flags() & Surface::ModifiedPosition) | (_surface->flags() & Surface::ModifiedSize));
    _surface->unsetSurfaceFlag(Surface::ModifiedGeometry);

    for (WidgetList::const_iterator it = _children.begin(); it != _children.end(); ++it)
        ((Widget*) *it)->_surface->setSurfaceFlag(flags);
}

void
Widget::keyDownEvent(const KeyEvent& keyEvent)
{
}

void
Widget::keyUpEvent(const KeyEvent& keyEvent)
{
}

void
Widget::pointerButtonDownEvent(const PointerEvent& mouseEvent)
{
}

void
Widget::pointerButtonUpEvent(const PointerEvent& mouseEvent)
{
}

void
Widget::pointerGrabEvent(const PointerEvent& pointerEvent)
{
}

void
Widget::pointerReleaseEvent(const PointerEvent& pointerEvent)
{
}

void
Widget::pointerMotionEvent(const PointerEvent& mouseEvent)
{
}

void
Widget::pointerWheelEvent(const PointerEvent& mouseEvent)
{
}

void
Widget::focusInEvent()
{
}

void
Widget::focusOutEvent()
{
}

void
Widget::enterEvent(const PointerEvent& mouseEvent)
{
}

void
Widget::leaveEvent(const PointerEvent& mouseEvent)
{
}

void
Widget::universalEvent(const UniversalEvent* event)
{
    ILOG_TRACE_W(ILX_WIDGET);
    ILOG_DEBUG(ILX_WIDGET, " -> Type = %u\n", event->type);
    ILOG_DEBUG(ILX_WIDGET, " -> Data = %p\n", event->data);
}

////////////////////////////////////////////////////////////////////////////////////
// Private methods...
////////////////////////////////////////////////////////////////////////////////////
void
Widget::setRootWindow(WindowWidget* root)
{
    if (root != NULL)
    {
        _surface->setSurfaceFlag(Surface::InitialiseSurface);
        _rootWindow = root;

        setNeighbours(_neighbours[Up], _neighbours[Down], _neighbours[Left], _neighbours[Right]);
    } else if ((_surface->flags() & Surface::SharedSurface) || (_surface->flags() & Surface::SubSurface))
    {
        _surface->setSurfaceFlag(Surface::InitialiseSurface);
        _surface->release();
    }

    for (WidgetListIterator it = _children.begin(); it != _children.end(); ++it)
        (*it)->setRootWindow(root);
}

}
