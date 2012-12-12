/*
 Copyright 2010, 2011 Tarik Sekmen.

 All Rights Reserved.

 Written by Tarik Sekmen <tarik@ilixi.org>.

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CPUMONITOR_H_
#define CPUMONITOR_H_

#include <vector>
#include <string>

//! cpuMonitor class parses cpu related proc filesystem files
class CPUMonitor
{
public:

    CPUMonitor();

    virtual
    ~CPUMonitor();

    //! Refresh readings
    void
    refresh();

    //! Data structure to store and process cpu readings
    struct CPU
    {
        struct Ticks
        {
            int U, S, N, I;
            //! Constructor initialises all ticks to zero
            Ticks();
            //! Returns user ticks
            int
            user() const
            {
                return U;
            }
            //! Returns system ticks
            int
            system() const
            {
                return S + N;
            }
            //! Returns total (non-idle) ticks
            int
            total() const
            {
                return U + S + N;
            }
            //! Returns elapsed ticks
            int
            elapsed() const
            {
                return U + S + N + I;
            }
        };
        //! Previous reading
        Ticks previous;
        //! Recent reading
        Ticks current;

        //! Returns cpu idle ticks  percentage
        int
        getIdle() const;
        //! Returns cpu user load percentage
        int
        getUser() const;
        //! Returns cpu system load percentage
        int
        getSystem() const;
        //! Returns total cpu load percentage
        int
        getUsage() const;
        //! Parses proc/stat returns true if successful, false otherwise.
        bool
        parse(FILE* fd);
    };

    /*!
     * This method ensures index is valid.
     * @param index 0 for _cores total, 1: first core, 2: second core, ...
     * @return Returns _cores structure, use methods \sa CPU::getSystem()
     */
    const CPU&
    getCpu(unsigned int index) const;

    //! Returns cache size of CPU
    int
    getCacheSize() const;

    //! Returns number of cores on CPU
    unsigned int
    getCpuCores() const;

    //! Returns CPU MHz
    float
    getCpuMHz() const;

    //! Returns CPU Model
    std::string
    getModel() const;

    //! Returns CPU Vendor
    std::string
    getVendor() const;

    //! Returns CPU and IO utilization for last one minute
    float
    getLoadAvg1() const;

    //! Returns CPU and IO utilization for last five minutes
    float
    getLoadAvg10() const;

    //! Returns CPU and IO utilization for last ten minutes
    float
    getLoadAvg5() const;

    //! Returns the number of currently running processes and the total number of processes
    std::string
    getProcesses() const;

private:
    FILE *fd;

    //! Aggregated reading
    CPU all;

    //! Core readings
    std::vector<CPU> _cores;

    //! CPU Model name
    std::string _model;

    //! CPU Vendor
    std::string _vendor;

    //! CPU MHz
    float _cpuMHz;

    //! CPU Cache size in KB
    int _cacheSize;

    //! CPU cores
    unsigned int _cpuCores;

    //! CPU and IO utilization for last one minute
    float _loadAvg1;

    //! CPU and IO utilization for last five minutes
    float _loadAvg5;

    //! CPU and IO utilization for last ten minutes
    float _loadAvg10;

    //! The number of currently running processes and the total number of processes
    std::string _processes;

    void
    readTicks();

    void
    readCpuInfo();

    void
    readLoadAverages();
};

#endif /* CPUMONITOR_H_ */
