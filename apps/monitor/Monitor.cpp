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

#include "Monitor.h"
#include <ui/VBoxLayout.h>
#include <ui/HBoxLayout.h>
#include <ui/GridLayout.h>
#include <ui/GroupBox.h>
#include <ui/TabPanel.h>
#include <ui/Icon.h>
#include <ui/ScrollArea.h>
#include <ui/Spacer.h>

#include <sstream>
#include "ilixiConfig.h"

template<class T>
    inline std::string
    toString(const T& t)
    {
        std::stringstream ss;
        ss << t;
        return ss.str();
    }

Monitor::Monitor(int argc, char* argv[])
        : Application(&argc, &argv)
{
    setMargin(15);
    setBackgroundImage(ILIXI_DATADIR"images/ilixi_bg.jpg");

    _cpuMon = new CPUMonitor();
    _cpuMon->refresh();

    _fsMon = new FSMonitor();
    _fsMon->refresh();

    _memMon = new MEMMonitor();
    _memMon->refresh();

    _netMon = new NETMonitor();
    _netMon->refresh();

    _osMon = new OSMonitor();
    _osMon->refresh();

    setLayout(new VBoxLayout());
    TabPanel* tabPanel = new TabPanel();
    addWidget(tabPanel);

    //**************************************
    VBoxLayout* overviewLayout = new VBoxLayout();
    tabPanel->addTab(overviewLayout, "Overview");

    _uptime = new Label("Uptime: " + _osMon->getUptime());
    overviewLayout->addWidget(_uptime);
    overviewLayout->addWidget(new Label("System: " + _osMon->getSysName()));
    overviewLayout->addWidget(new Label("Node: " + _osMon->getNodeName()));
    overviewLayout->addWidget(new Label("Release: " + _osMon->getRelease()));
    overviewLayout->addWidget(new Label("Version: " + _osMon->getVersion()));
    overviewLayout->addWidget(new Label("Machine: " + _osMon->getMachine()));

    overviewLayout->addWidget(new Label("CPU Vendor: " + _cpuMon->getVendor()));
    overviewLayout->addWidget(new Label("CPU Model: " + _cpuMon->getModel()));
    overviewLayout->addWidget(new Label("CPU Cores: " + toString(_cpuMon->getCpuCores())));
    overviewLayout->addWidget(new Label("CPU Cache: " + toString(_cpuMon->getCacheSize()) + " KB"));
    overviewLayout->addWidget(new Label("Processes: " + _cpuMon->getProcesses()));

    overviewLayout->addWidget(new Spacer(Vertical));

    //****************************************************************************
    GridLayout* fm = new GridLayout(3, 2);
    tabPanel->addTab(fm, "Resources");

    GroupBox* memGroup = new GroupBox("Memory");
    memGroup->setTitleIcon(StyleHint::RAM);
//    memGroup->setYConstraint(FixedConstraint);
    memGroup->setLayout(new VBoxLayout());
    fm->addWidget(memGroup);

    memGroup->addWidget(new Label("Total: " + toString(_memMon->getTotal()) + " MB"));
    memGroup->addWidget(new Label("Cached: " + toString(_memMon->getCached()) + " MB"));
    memGroup->addWidget(new Label("Free: " + toString(_memMon->getFree()) + " MB"));
    memGroup->addWidget(new Label("Buffers: " + toString(_memMon->getBuffers()) + " MB"));
    memGroup->addWidget(new Spacer(Vertical));

    memGroup->addWidget(new Label("Usage:"));
    _memUsage = new ProgressBar();
    memGroup->addWidget(_memUsage);
    //**************************************
    GroupBox* fsGroup = new GroupBox("File System");
    fsGroup->setTitleIcon(StyleHint::File);
//    fsGroup->setYConstraint(FixedConstraint);
    fsGroup->setLayout(new VBoxLayout());
    fm->addWidget(fsGroup);

    fsGroup->addWidget(new Label("Total: " + toString(_fsMon->getTotal()) + " GB"));
    fsGroup->addWidget(new Label("Free: " + toString(_fsMon->getFree()) + " GB"));
    fsGroup->addWidget(new Spacer(Vertical));

    fsGroup->addWidget(new Label("Usage:"));
    _fsUsage = new ProgressBar();
    fsGroup->addWidget(_fsUsage);
    //**************************************
    GroupBox* netGroup = new GroupBox("Network");
    netGroup->setTitleIcon(StyleHint::Network);
//    netGroup->setYConstraint(FixedConstraint);
    VBoxLayout* netGroupLayout = new VBoxLayout();
    netGroup->setLayout(netGroupLayout);
    fm->addWidget(netGroup);

    _netRXC = new Label("Receiving:" + _netMon->getReceiving());
    netGroup->addWidget(_netRXC);
    _netTXC = new Label("Sending:" + _netMon->getTransmitting());
    netGroup->addWidget(_netTXC);
    _netRXT = new Label("Total Received:" + _netMon->getTotalReceived());
    netGroup->addWidget(_netRXT);
    _netTXT = new Label("Total Sent:" + _netMon->getTotalTransmitted());
    netGroup->addWidget(_netTXT);
    netGroup->addWidget(new Spacer(Vertical));
    //**************************************
    GroupBox* cpuGroup = new GroupBox("CPU");
    cpuGroup->setTitleIcon(StyleHint::CPU);
    VBoxLayout* cpuGroupLayout = new VBoxLayout();
    cpuGroup->setLayout(cpuGroupLayout);
    fm->addWidget(cpuGroup);

    cpuGroupLayout->addWidget(new Label("Idle:"));
    _cpuIdle = new ProgressBar();
    cpuGroupLayout->addWidget(_cpuIdle);

    _cpuChart = new BarChart();
    char core[10];
    for (unsigned int i = 0; i < _cpuMon->getCpuCores(); ++i)
    {
        snprintf(core, 10, "Core %d", i);
        _cpuChart->addBar(core, Color(rand() % 255, rand() % 255, rand() % 255));
    }
    cpuGroupLayout->addWidget(_cpuChart);

//    fm->addWidget(new Spacer(Vertical));
    //****************************************************************************

    _monitorTimer = new Timer();
    _monitorTimer->sigExec.connect(sigc::mem_fun(this, &Monitor::updateMonitor));
    _monitorTimer->start(1000);
}

Monitor::~Monitor()
{
    delete _monitorTimer;
    delete _cpuMon;
    delete _fsMon;
    delete _memMon;
    delete _netMon;
    delete _osMon;
}

void
Monitor::updateMonitor()
{
    _cpuMon->refresh();
    _fsMon->refresh();
    _memMon->refresh();
    _netMon->refresh();
    _osMon->refresh();

    _uptime->setText("Uptime: " + _osMon->getUptime());
    _fsUsage->setValue(_fsMon->getUsage());
    _memUsage->setValue(_memMon->getUsage());
    _netRXC->setText("Receiving:" + _netMon->getReceiving());
    _netTXC->setText("Sending:" + _netMon->getTransmitting());
    _netRXT->setText("Total Received:" + _netMon->getTotalReceived());
    _netTXT->setText("Total Sent:" + _netMon->getTotalTransmitted());
    for (unsigned int i = 0; i < _cpuMon->getCpuCores(); ++i)
        _cpuChart->addValue(i, _cpuMon->getCpu(i + 1).getUsage());

    _cpuIdle->setValue(_cpuMon->getCpu(0).getIdle());
}

int
main(int argc, char* argv[])
{
    Monitor app(argc, argv);
    app.exec();
    return 0;
}
