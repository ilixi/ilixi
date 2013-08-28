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

#ifndef ILIXI_MEMORYMONITOR_H_
#define ILIXI_MEMORYMONITOR_H_

#include <lib/Timer.h>
#include <sigc++/signal.h>

namespace ilixi
{

class ApplicationManager;

//! Tracks changes in memory for OOM.
class MemoryMonitor
{
public:
    //! This enum specifies memory state.
    enum MemoryState
    {
        Critical,
        Low,
        Normal,
    };

    /*!
     * Constructor.
     */
    MemoryMonitor(ApplicationManager* manager, float memCritical, float memLow, long unsigned int pgCritical, long unsigned int pgLow);

    /*!
     * Destructor.
     */
    virtual
    ~MemoryMonitor();

    /*!
     * Returns critical memory usage threshold.
     */
    float
    getMemCritical() const;

    /*!
     * Sets critical memory usage threshold.
     */
    void
    setMemCritical(float memCritical);

    /*!
     * Returns low memory usage threshold.
     */
    float
    getMemLow() const;

    /*!
     * Sets low memory usage threshold.
     */
    void
    setMemLow(float memLow);

    /*!
     * Returns critical page fault threshold.
     */
    int
    getPgCritical() const;

    /*!
     * Sets critical page fault threshold.
     */
    void
    setPgCritical(int pgCritical);

    /*!
     * Returns low page fault threshold.
     */
    int
    getPgLow() const;

    /*!
     * Sets low page fault threshold.
     */
    void
    setPgLow(int pgLow);

    /*!
     * Returns current memory state.
     */
    MemoryState
    getState() const;

    /*!
     * This signal is emitted when a state change from Normal to Low or Critical happens.
     */
    sigc::signal<void, MemoryState> sigStateChanged;

private:
    //! Owner.
    ApplicationManager* _manager;
    //! This property stores critical memory usage threshold.
    float _memCritical;
    //! This property stores critical memory usage threshold.
    float _memLow;
    //! This property stores critical page fault threshold.
    int _pgCritical;
    //! This property stores low page fault threshold.
    int _pgLow;
    //! This property stores the sum of previous page faults.
    int _pgPre;
    //! Current memory state.
    MemoryState _state;
    //! This timer is executed at various intervals.
    Timer _timer;

    //! Timer slot.
    void
    refresh();

    //! Tracks memory usage.
    void
    calcMemoryUsed();

    //! Tracks page faults.
    void
    calcPageFaults();
};

} /* namespace ilixi */
#endif /* ILIXI_MEMORYMONITOR_H_ */
