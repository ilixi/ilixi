/*
 Copyright 2010, 2011 Tarik Sekmen.

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

#include "OSK.h"
#include <graphics/Painter.h>
#include <sigc++/bind.h>
#include <core/Logger.h>
#include <ui/VBoxLayout.h>
#include <ilixiConfig.h>
#include "utf8-decoder.h"

D_DEBUG_DOMAIN( ILX_OSK, "ilixi/osk/ILXOSK", "ILXOSK");

ILXOSK::ILXOSK(int argc, char* argv[])
        : Application(&argc, &argv, OptDaleAuto),
          _keyboard(NULL)
{
    ILOG_TRACE_F(ILX_OSK);
    setBackgroundImage(ILIXI_DATADIR"osk/osk-bg.png");
    setMargin(10);

    setLayout(new VBoxLayout());

    _helper = new OSKHelper();
    addWidget(_helper);

    _keyboard = new Keyboard(_helper);

    bool numpad = false;
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "numpad") == 0)
            numpad = true;
    }

    if (numpad)
        _keyboard->parseLayoutFile(ILIXI_DATADIR"osk/numpad.xml");
    else
        _keyboard->parseLayoutFile(ILIXI_DATADIR"osk/keyboard.xml");
    addWidget(_keyboard);

    _helper->sigSubmit.connect(sigc::mem_fun(this, &ILXOSK::forwardString));
}

ILXOSK::~ILXOSK()
{
    ILOG_TRACE_F(ILX_OSK);
}

void
ILXOSK::forwardString(const std::string& str)
{
    std::vector<uint32_t> ucs32;
    decode((uint8_t*) str.c_str(), ucs32);
    _keyboard->forwardKeyData(ucs32, 0, true);
}

#if ILIXI_HAVE_LIBWNN
bool
ILXOSK::windowPreEventFilter(const DFBWindowEvent &event)
{
    switch (event.type)
    {
    case DWET_KEYDOWN:
        if ((event.modifiers & DIMM_CONTROL) && (event.key_symbol == DIKS_SPACE))
        {
            _keyboard->toggleHelper();
            return true;
        } else if (_helper->visible() && event.key_symbol == DIKS_SHIFT)
        {
            _helper->setResizeMode(true);
            return true;
        }

        return _keyboard->handleKeyPress(event.key_symbol);

    case DWET_KEYUP:
        if (_helper->visible() && event.key_symbol == DIKS_SHIFT)
            _helper->setResizeMode(false);
        return true;

    default:
        break;
    }

    return false;
}
#endif

int
main(int argc, char* argv[])
{
    ILXOSK app(argc, argv);
    app.exec();
    return 0;
}
