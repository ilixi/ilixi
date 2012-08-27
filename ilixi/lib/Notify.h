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

#ifndef ILIXI_NOTIFY_H_
#define ILIXI_NOTIFY_H_

#include <graphics/Surface.h>
#include <types/TextLayout.h>
#include <string>

namespace ilixi
{

class Notify
{
public:
    Notify(const std::string& title, const std::string& text,
           const std::string& iconPath = "");

    virtual
    ~Notify();

    void
    setIcon(const std::string& iconPath);

    void
    show();

    struct NotifyData
    {
        char path[256];
        char sender[128];
        char text[128];
        char title[128];
    };

private:
    std::string _title;
    std::string _text;
    std::string _icon;

};

} /* namespace ilixi */
#endif /* ILIXI_NOTIFY_H_ */
