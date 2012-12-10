#include "PhoneCompositor.h"
#include "CarouselSwitcher.h"
#include <core/Logger.h>
#include <core/PlatformManager.h>

namespace ilixi
{

PhoneCompositor::PhoneCompositor(int argc, char* argv[])
        : ILXCompositor(argc, argv)
{
    setPaletteFromFile(ILIXI_DATADIR"phone/statusbar/palette.xml");
    appMan()->parseFolder(ILIXI_DATADIR"phone/apps");
    setSwitcher(new CarouselSwitcher());

    Size size = PlatformManager::instance().getLayerSize();
    setAppGeometry(Rectangle(150, 0, size.width() - 150, size.height()));
    setBarGeometry(Rectangle(0, 0, 150, size.height()));
    setOSKGeometry(Rectangle(150, size.height(), size.width() - 150, 300));
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
