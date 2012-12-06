/*
 * CarCompositor.cpp
 *
 *  Created on: Dec 4, 2012
 *      Author: tarik
 */

#include "CarCompositor.h"
#include "HorizontalSwitcher.h"
#include <core/Logger.h>

namespace ilixi
{

CarCompositor::CarCompositor(int argc, char* argv[])
        : ILXCompositor(argc, argv)
{
    setPaletteFromFile(ILIXI_DATADIR"car/statusbar/palette.xml");
    appMan()->parseFolder(ILIXI_DATADIR"car/apps");
    setSwitcher(new HorizontalSwitcher());

    sigVisible.connect(sigc::mem_fun(this, &CarCompositor::setUIGeomety));
}

CarCompositor::~CarCompositor()
{
}

void
CarCompositor::setUIGeomety()
{
    setAppGeometry(Rectangle(0, 0, width(), height() - 50));
    setBarGeometry(Rectangle(0, height() - 55, width(), 55));
    setOSKGeometry(Rectangle(0, height(), width(), 400));
    setSwitcherGeometry(Rectangle(0, height() - 50, width(), 196));
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
