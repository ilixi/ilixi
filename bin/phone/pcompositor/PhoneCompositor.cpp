#include "PhoneCompositor.h"
#include "CarouselSwitcher.h"
#include <core/Logger.h>

namespace ilixi
{

PhoneCompositor::PhoneCompositor(int argc, char* argv[])
        : ILXCompositor(argc, argv)
{
    setPaletteFromFile(ILIXI_DATADIR"phone/statusbar/palette.xml");
    appMan()->parseFolder(ILIXI_DATADIR"phone/apps");
    setSwitcher(new CarouselSwitcher());

    sigVisible.connect(sigc::mem_fun(this, &PhoneCompositor::setUIGeomety));
}

PhoneCompositor::~PhoneCompositor()
{
}

void
PhoneCompositor::setUIGeomety()
{
    setAppGeometry(Rectangle(150, 0, width() - 150, height()));
    setBarGeometry(Rectangle(0, 0, 150, height()));
    setOSKGeometry(Rectangle(0, height(), width(), 400));
    setSwitcherGeometry(Rectangle(150, 0, width() - 150, height()));
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
