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
//! Helper class for implementing runtime i18n behaviour.
/*!
 * Used for implementing a simple notification mechanism. updateI18nText() is for each object once language changes at runtime.
 *
 * Make sure you instantiated objects are registered using:
 * \code
 *     PlatformManager::instance().addI18N(this);
 * \endcode
 */
class I18NBase
{
    friend class PlatformManager;
public:
    I18NBase();

    virtual
    ~I18NBase();

private:
    //! This method must be implemented in order to re-layout text once language changes.
    virtual void
    updateI18nText() =0;
};

} /* namespace ilixi */
#endif /* ILIXI_I18NBASE_H_ */
