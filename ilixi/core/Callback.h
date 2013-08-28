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

#ifndef ILIXI_CALLBACK_H_
#define ILIXI_CALLBACK_H_

namespace ilixi
{

//! Base class for classes with methods that are executed at intervals.
class Functionoid
{
public:
    /*!
     * This method is executed at each iteration of application main loop.
     */
    virtual bool
    funck() = 0;

    virtual
    ~Functionoid() = 0;
};

inline
Functionoid::~Functionoid()
{
}

//! Inserts/removes Functionoids to/from main loop.
class Callback
{
    friend class Engine;

public:
    /*!
     * Constructor.
     */
    Callback(Functionoid* funck);

    /*!
     * Destructor.
     */
    virtual
    ~Callback();

    /*!
     * Adds callback to AppBase so it can run at next interval.
     */
    void
    start();

    /*!
     * Removes callback from AppBase.
     */
    void
    stop();

private:
    //! Managed.
    Functionoid* _funck;
};
}

#endif /* ILIXI_CALLBACK_H_ */
