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

#ifndef ILIXI_INPUTHELPERJP_H_
#define ILIXI_INPUTHELPERJP_H_

#include <lib/InputHelper.h>
#include <map>

namespace ilixi
{

/*!
 *
 */
class InputHelperJP : public ilixi::InputHelper
{
public:
    InputHelperJP();

    virtual
    ~InputHelperJP();

private:
    std::map<std::string, std::string> _hiraganaMap;

    virtual bool
    convert(uint32_t symbol);

    void
    initHiraganaMap();
};

} /* namespace ilixi */
#endif /* ILIXI_INPUTHELPERJP_H_ */
