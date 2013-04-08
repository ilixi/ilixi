/*
 Copyright 2010-2013 Tarik Sekmen.

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

#ifndef ILIXI_I18NBASE_H_
#define ILIXI_I18NBASE_H_

#include <string>

namespace ilixi
{

class I18NBase
{
    friend class PlatformManager;
public:
    I18NBase();

    virtual
    ~I18NBase();

    /*!
     * Sets the text using i18n support.
     */
    virtual void
    setI18nText(const std::string& text) =0;

protected:
    std::string _i18nID;

private:
    virtual void
    updateI18nText() =0;
};

} /* namespace ilixi */
#endif /* ILIXI_I18NBASE_H_ */
