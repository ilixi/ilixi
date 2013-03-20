/*
 * CarCompositor.cpp
 *
 *  Created on: Dec 4, 2012
 *      Author: tarik
 */

#include "CarCompositor.h"
#include "HorizontalSwitcher.h"
#include <core/Logger.h>
#include <core/PlatformManager.h>

namespace ilixi
{

CarCompositor::CarCompositor(int argc, char* argv[])
        : ILXCompositor(argc, argv)
{
    appMan()->parseFolder(ILIXI_DATADIR"car/apps");
    setSwitcher(new HorizontalSwitcher(this));

    Size size = PlatformManager::instance().getLayerSize();
    setAppGeometry(Rectangle(0, 0, size.width(), size.height() - 50));
    setBarGeometry(Rectangle(0, size.height() - 55, size.width(), 55));
    setOSKGeometry(Rectangle(0, size.height(), size.width(), 400));
    setSwitcherGeometry(Rectangle(0, size.height() - 50, size.width(), 196));
}

CarCompositor::~CarCompositor()
{
}

} /* namespace ilixi */

int
main(int argc, char* argv[])
{
    try
    {
        ilixi::CarCompositor app(argc, argv);
        app.exec();
    } catch (const std::exception& e)
    {
        ILOG_FATAL(ilixi::ILX, "Exception: %s\n", e.what());
    }
    return 0;
}
