/*
 Copyright 2010-2015 Tarik Sekmen.

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

#ifndef ILIXI_GESTURE_H_
#define ILIXI_GESTURE_H_

#include <core/EventFilter.h>
#include <sigc++/signal.h>
#include <map>
#include <vector>

namespace ilixi
{
//! Simple pointer gesture recognizer
/*!
 * This class filters pointer events and tries to match a defined gesture.
 *
 * A gesture is defined using a string of movements in following directions:
 *
 * 0 - East
 * 1 - South east
 * 2 - South
 * 3 - South west
 * 4 - West
 * 5 - North west
 * 6 - North
 * 7 - North east
 *
 * For example a check gesture can be defined using "117777".
 */
class GestureRecognizer : virtual public sigc::trackable, public EventFilter
{
public:
    GestureRecognizer();

    virtual
    ~GestureRecognizer();

    /*!
     * Adds a new gesture definition.
     *
     * @param gestureName name of gesture to store
     * @param moves a list of moves
     */
    void
    addGesture(const std::string& gestureName, const std::string& moves);

    /*!
     * This method parses pointer events for recognition.
     */
    bool
    pointerEventConsumer(const PointerEvent& pointerEvent);

    /*!
     * Returns a vector of points corresponding to moves.
     */
    const std::vector<Point>&
    path() const;

    /*!
     * Returns a vector of moves.
     */
    const std::vector<int>&
    moves() const;

    /*!
     * Returns movement detection frequency.
     */
    unsigned int
    frequency() const;

    /*!
     * Returns minimum distance required for a  movement detection in pixels.
     */
    unsigned int
    minDistance() const;

    /*!
     * Returns recognizer sensitivity.
     */
    unsigned int
    sensitivity() const;

    /*!
     * Sets movement detection frequency.
     *
     * @param frequency difference between two consecutive pointer events in milliseconds.
     */
    void
    setFrequency(unsigned int frequency = 40);

    /*!
     * Sets minimum distance required for a movement detection in pixels.
     */
    void
    setMinDistance(unsigned int minDistance = 30);

    /*!
     * Sets recognizer sensitivity. Lower value is more sensitive.
     */
    void
    setSensitivity(unsigned int sensitivity = 2);

    /*!
     * This signal is emitted each time recognizer detects a movement.
     */
    sigc::signal<void> sigMovement;

    /*!
     * This signal is emitted once recognizer stops (before matching begins).
     */
    sigc::signal<void> sigStopped;

    /*!
     * This signal is emitted if a gesture is matched.
     */
    sigc::signal<void, std::string> sigGestureMatched;

private:
    //! Previous x value.
    int _preX;
    //! Previous y value.
    int _preY;
    //! Last movement direction.
    int _lastMove;
    //! Time stamp at last movement.
    long int _lastMoveTS;
    //! Movement detection frequency, default: 40ms.
    unsigned int _frequency;
    //! Minimum distance between points used for movement detection, default: 30px.
    unsigned int _minDistance;
    //! Recognizer sensitivity, default: 2.
    unsigned int _sensitivity;

    //! This vector stores points corresponding to moves.
    std::vector<Point> _path;
    //! This vector stores moves.
    std::vector<int> _moves;

    typedef std::map<std::string, std::vector<int> > GestureMap;
    //! This stores defined gestures.
    GestureMap _gestures;

    //! Starts recognition.
    void
    start(int x, int y);

    //! Stops recognition.
    void
    stop();

    //! Checks if pointer motion can be considered as a movement.
    void
    detectMove(int x, int y, long int timeStamp);

    //! Converts pointer motion to a movement.
    void
    addMove(int dx, int dy);

    //! Returns a match if any, otherwise returns empty string.
    std::string
    matchGesture();

    //! Calculates Levenshtein distance.
    unsigned int
    levenshteinDistance(const std::vector<int>& a, const std::vector<int>& b);
};

} /* namespace ilixi */
#endif /* GESTURE_H_ */
