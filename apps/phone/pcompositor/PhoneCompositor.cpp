/*
 Copyright 2010-2012 Tarik Sekmen.

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

#include "PhoneCompositor.h"
#include "CarouselSwitcher.h"
#include <core/Logger.h>
#include <core/PlatformManager.h>

namespace ilixi
{

PhoneCompositor::PhoneCompositor(int argc, char* argv[])
        : ILXCompositor(argc, argv)
{
    appMan()->parseFolder(ILIXI_DATADIR"phone/apps");
    setSwitcher(new CarouselSwitcher(this));

    Size size = PlatformManager::instance().getLayerSize();
    setAppGeometry(Rectangle(150, 0, size.width() - 150, size.height()));
    setBarGeometry(Rectangle(0, 0, 150, size.height()));
    setOSKGeometry(Rectangle(150, size.height(), size.width() - 150, 250));
    setSwitcherGeometry(Rectangle(150, 0, size.width() - 150, size.height()));
}

PhoneCompositor::~PhoneCompositor()
{
}

bool
PhoneCompositor::windowCustomEventFilter(const DFBWindowEvent& event)
{
    switch (event.type)
    {
    case DWET_KEYDOWN:
        {
            if (event.key_symbol == DIKS_MENU || event.key_symbol == DIKS_F1)
            {
                _statusBar->view()->setWindowFocus();
                return true;
            } else if (event.key_symbol == DIKS_BACK || event.key_symbol == DIKS_F2)
            {
                if (_currentApp)
                    _currentApp->view()->setWindowFocus();
                else
                    _home->view()->setWindowFocus();
                return true;
            } else if (_switcher->visible() && (event.key_symbol == DIKS_CLEAR || event.key_symbol == DIKS_ESCAPE))
            {
                _switcher->killCurrentApp();
                return true;
            }
        }
        break;

    default:
        return false;
    }
    return false;
}

} /* namespace ilixi */

int
main(int argc, char* argv[])
{
    try
    {
        ilixi::PhoneCompositor app(argc, argv);
        app.exec();
    } catch (const std::exception& e)
    {
        ILOG_FATAL(ilixi::ILX, "Exception: %s\n", e.what());
    }
    return 0;
}
