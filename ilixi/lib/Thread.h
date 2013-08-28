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

#ifndef ILIXI_THREAD_H_
#define ILIXI_THREAD_H_

#include <pthread.h>
#include <sigc++/signal.h>

namespace ilixi
{
//! Create and maintain threads.
/*!
 * This class is used for creating a thread within the program which executes
 * a user defined method.
 */
class Thread : public sigc::trackable
{
public:
    /*!
     * Constructor.
     */
    Thread();

    /*!
     * Destructor.
     */
    virtual
    ~Thread();

    /*!
     * Starts running thread.
     */
    bool
    start();

    /*!
     * Cancels thread using a cancellation point.
     */
    bool
    cancel();

    /*!
     * Waits until thread finishes execution.
     */
    bool
    join();

    /*!
     * This method is executed when thread starts.
     *
     * By default, threads are created with PTHREAD_CANCEL_DEFERRED type, so
     * you could create cancellation points inside, e.g. pthread_testcancel().
     */
    virtual int
    run() = 0;

    /*!
     * Returns the exit code.
     */
    int
    exit_code();

    /*!
     * Returns the stack size specified for this thread. By default, this function
     * will return zero if stack size of the thread is determined by operating system.
     */
    int
    stackSize() const;

    /*!
     * Sets the minimum stack size attribute which is set during the creation of thread.
     * If stackSize is 0, then operating system's default limit is used.
     *
     * @param stackSize
     */
    void
    setStackSize(size_t stackSize);

    //! This signal is emitted when the thread starts executing.
    sigc::signal<void> sigStarted;
    //! This signal is emitted when the thread finishes executing.
    sigc::signal<void> sigFinished;
    //! This signal is emitted when the thread terminates by calling cancel.
    sigc::signal<void> sigTerminated;

private:
    //! This property specifies whether thread is running.
    bool _running;
    //! This property stores the exit code.
    int _code;
    //! Stack size for thread.
    size_t _stackSize;
    //! Serialise access to running flag.
    pthread_mutex_t _runMutex;
    //! Thread id.
    pthread_t _pThread;

    //! Starts running the user thread.
    void
    executeRun();

    //! Starts executeRun() for the thread.
    static void*
    wrapper(void* arg);
};
}

#endif /* ILIXI_THREAD_H_ */
