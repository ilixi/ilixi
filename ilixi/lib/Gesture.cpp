/*
 Copyright 2010-2013 Tarik Sekmen.

 All Rights Reserved.

 Written by Tarik Sekmen <tarik@ilixi.org>.

 Based on work by Didier Brun <webmaster@foxaweb.com>.

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

#include <lib/Gesture.h>
#include <core/Logger.h>
#include <cmath>

namespace ilixi
{

static const double RAD_2_DIRECTION = 1.27323954474;

D_DEBUG_DOMAIN(ILX_GESTURE, "ilixi/lib/GestureRecognizer", "GestureRecognizer");

GestureRecognizer::GestureRecognizer()
        : _preX(-1),
          _preY(-1),
          _lastMove(-1),
          _lastMoveTS(0),
          _frequency(40),
          _minDistance(900),
          _sensitivity(2)
{
    ILOG_TRACE_F(ILX_GESTURE);
}

GestureRecognizer::~GestureRecognizer()
{
    ILOG_TRACE_F(ILX_GESTURE);
}

void
GestureRecognizer::addGesture(const std::string& gestureName, const std::string& moves)
{
    ILOG_TRACE_F(ILX_GESTURE);
    if (gestureName.empty())
    {
        ILOG_WARNING(ILX_GESTURE, "Gesture name is empty!\n");
        return;
    }

    std::vector<int> g;
    for (unsigned int i = 0; i < moves.size(); i++)
        g.push_back(moves[i] - '0');

    std::pair<GestureMap::iterator, bool> res = _gestures.insert(std::make_pair(gestureName, g));
    if (!res.second)
        ILOG_WARNING(ILX_GESTURE, "Gesture \"%s\" already exists!\n", gestureName.c_str());
    else
        ILOG_DEBUG(ILX_GESTURE, " -> Added \"%s\".\n", gestureName.c_str());
}

bool
GestureRecognizer::pointerEventConsumer(const PointerEvent& event)
{
    switch (event.eventType)
    {
    case PointerButtonDown:
        start(event.x, event.y);
        break;

    case PointerButtonUp:
        detectMove(event.x, event.y, event.timestamp);
        stop();
        break;

    case PointerMotion:
        if (event.buttonMask & ButtonMaskLeft)
            detectMove(event.x, event.y, event.timestamp);
        break;

    default:
        break;
    }

    return false;
}

const std::vector<Point>&
GestureRecognizer::path() const
{
    return _path;
}

const std::vector<int>&
GestureRecognizer::moves() const
{
    return _moves;
}

unsigned int
GestureRecognizer::frequency() const
{
    return _frequency;
}

unsigned int
GestureRecognizer::minDistance() const
{
    return sqrt(_minDistance);
}

unsigned int
GestureRecognizer::sensitivity() const
{
    return _sensitivity;
}

void
GestureRecognizer::setFrequency(unsigned int frequency)
{
    _frequency = frequency;
}

void
GestureRecognizer::setMinDistance(unsigned int minDistance)
{
    _minDistance = minDistance * minDistance;
}

void
GestureRecognizer::setSensitivity(unsigned int sensitivity)
{
    _sensitivity = sensitivity;
}

void
GestureRecognizer::start(int x, int y)
{
    ILOG_TRACE_F(ILX_GESTURE);
    _moves.clear();
    _path.clear();
    _lastMove = -1;
    _lastMoveTS = direct_clock_get_millis();
    _preX = x;
    _preY = y;
}

void
GestureRecognizer::stop()
{
    ILOG_TRACE_F(ILX_GESTURE);
    std::string match = matchGesture();
    sigStopped();
    if (match != "")
    {
        ILOG_DEBUG(ILX_GESTURE, " -> Matched \"%s\"\n", match.c_str());
        sigGestureMatched(match);
    }
}

void
GestureRecognizer::detectMove(int x, int y, long int timeStamp)
{
    if (timeStamp - _lastMoveTS < _frequency)
        return;

    _lastMoveTS = timeStamp;
    int dx = x - _preX;
    int dy = y - _preY;

    if ((dx * dx + dy * dy) > _minDistance)
    {
        addMove(dx, dy);
        _preX = x;
        _preY = y;
        _path.push_back(Point(x, y));
    }
}

void
GestureRecognizer::addMove(int dx, int dy)
{
    ILOG_TRACE_F(ILX_GESTURE);
    int dir = round(atan2f(dy, dx) * RAD_2_DIRECTION);
    if (dir < 0)
        dir += 8;

    if (_lastMove != dir)
    {
        ILOG_DEBUG(ILX_GESTURE, " -> movement direction: %d\n", dir);
        _moves.push_back(dir);
        sigMovement();
    }
}

std::string
GestureRecognizer::matchGesture()
{
    if (_moves.empty())
        return "";

    int distance = INT_MAX;
    std::string match = "";

    for (GestureMap::iterator i = _gestures.begin(); i != _gestures.end(); ++i)
    {
        int temp = levenshteinDistance(i->second, _moves);
        if (temp <= _sensitivity && temp < distance)
        {
            distance = temp;
            match = i->first;
        }
    }
    return match;
}

unsigned int
GestureRecognizer::levenshteinDistance(const std::vector<int>& a, const std::vector<int>& b)
{
    const size_t len1 = a.size(), len2 = b.size();
    std::vector<unsigned int> col(len2 + 1), prevCol(len2 + 1);

    for (unsigned int i = 0; i < prevCol.size(); i++)
        prevCol[i] = i;

    for (unsigned int i = 0; i < len1; i++)
    {
        col[0] = i + 1;
        for (unsigned int j = 0; j < len2; j++)
            col[j + 1] = std::min(std::min(1 + col[j], 1 + prevCol[1 + j]), prevCol[j] + (a[i] == b[j] ? 0 : 1));
        col.swap(prevCol);
    }
    return prevCol[len2];
}

} /* namespace ilixi */
