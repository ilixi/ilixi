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

#ifndef ILIXI_ICONS_H_
#define ILIXI_ICONS_H_

#include <types/Image.h>
#include <map>
#include <iostream>

namespace ilixi
{
//! Provides an icon pack.
class IconPack
{
public:
    /*!
     * Constructor.
     */
    IconPack();

    /*!
     * Destructor.
     */
    virtual
    ~IconPack();

    /*!
     * If name is found returns icon as a sub-image, else returns NOIMAGE image.
     */
    Image*
    getIcon(std::string name);

    /*!
     * Initialise icons from an XML file.
     *
     * Returns false on error.
     */
    bool
    parseIcons(const char* iconsFile);

private:
    //! This image stores the icon pack.
    Image* _iconPack;
    //! Default size for icons in the icon pack, e.g. Size(48, 48).
    int _iconSize;

    typedef std::map<std::string, Point> IconMap;
    //! This is for mapping icons to sub_images.
    IconMap _iconMap;

    //! Release icon pack image.
    void
    release();

    friend std::istream&
    operator>>(std::istream& is, IconPack& obj);

    friend std::ostream&
    operator<<(std::ostream& os, const IconPack& obj);
};

} /* namespace ilixi */
#endif /* ILIXI_ICONS_H_ */
