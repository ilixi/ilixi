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

#include <graphics/IconPack.h>
#include <lib/FileSystem.h>
#include <lib/XMLReader.h>
#include <core/Logger.h>
#include <fstream>

namespace ilixi
{

D_DEBUG_DOMAIN(ILX_ICONPACK, "ilixi/graphics/IconPack", "IconPack");

IconPack::IconPack()
        : _iconPack(NULL),
          _iconSize(48)
{
}

IconPack::~IconPack()
{
    release();
}

Image*
IconPack::getIcon(std::string name)
{
    ILOG_TRACE(ILX_ICONPACK);
    ILOG_DEBUG(ILX_ICONPACK, " -> name: %s\n", name.c_str());
    IconMap::iterator it = _iconMap.find(name);
    if (it != _iconMap.end())
    {
        ILOG_DEBUG(ILX_ICONPACK, " -> %s @ (%d, %d, %d, %d)\n", name.c_str(), it->second.x(), it->second.y(), _iconSize, _iconSize);
        return new Image(_iconPack, Rectangle(it->second.x(), it->second.y(), _iconSize, _iconSize));
    }
    ILOG_WARNING(ILX_ICONPACK, " -> Cannot find icon: %s\n", name.c_str());
    return NULL;
}

bool
IconPack::parseIcons(const char* iconsFile)
{
    ILOG_TRACE(ILX_ICONPACK);
    ILOG_DEBUG(ILX_ICONPACK, " -> file: %s\n", iconsFile);

    std::string cacheFile = PrintF("%s%u.sxml", FileSystem::ilxDirectory().c_str(), createHash(iconsFile));
    ILOG_DEBUG(ILX_ICONPACK, " -> cache file: %s\n", cacheFile.c_str());
    if (difftime(FileSystem::getModificationTime(cacheFile), FileSystem::getModificationTime(iconsFile)) > 0)
    {
        ILOG_DEBUG(ILX_ICONPACK, " -> Parsing cached icons file.\n");
        std::ifstream ifs(cacheFile.c_str(), std::ios::in);
        ifs >> *this;
        ifs.close();
        ILOG_INFO(ILX_ICONPACK, "Parsed cached icons file: %s\n", cacheFile.c_str());
    } else
    {
        XMLReader xml;
        if (xml.loadFile(iconsFile) == false)
        {
            ILOG_FATAL(ILX_ICONPACK, "Could not parse fonts!\n");
            return false;
        }

        xmlNodePtr root = xml.root();
        xmlNodePtr node = xml.currentNode();

        release();

        xmlChar* imgFile = xmlGetProp(root, (xmlChar*) "resource");
        xmlChar* imgDefSize = xmlGetProp(root, (xmlChar*) "defaultSize");
        std::string path = (char*) imgFile;
        std::string file;
        size_t found = path.find("@IMGDIR:");
        if (found != std::string::npos)
        {
            file = ILIXI_DATADIR"images/";
            file.append(path.substr(found + 8, std::string::npos));
        } else
            file = path;

        _iconPack = new Image(file);
        _iconSize = atoi((char*) imgDefSize);
        xmlFree(imgDefSize);
        xmlFree(imgFile);

        while (node != NULL)
        {
            xmlChar* iconName = xmlGetProp(node, (xmlChar*) "name");
            xmlChar* iconRow = xmlGetProp(node, (xmlChar*) "row");
            xmlChar* iconCol = xmlGetProp(node, (xmlChar*) "col");
            int x = (atoi((char*) iconCol) - 1) * _iconSize;
            int y = (atoi((char*) iconRow) - 1) * _iconSize;
            std::pair<IconMap::iterator, bool> res = _iconMap.insert(std::make_pair((char*) iconName, Point(x, y)));
            if (!res.second)
                ILOG_WARNING(ILX_ICONPACK, "Icon %s already exists!\n", iconName);
            else
                ILOG_DEBUG(ILX_ICONPACK, " -> %s - %d, %d\n", iconName, res.first->second.x(), res.first->second.y());

            xmlFree(iconCol);
            xmlFree(iconRow);
            xmlFree(iconName);
            node = node->next;
        }

        ILOG_INFO(ILX_ICONPACK, "Parsed icons file: %s\n", iconsFile);

        std::ofstream ofs(cacheFile.c_str(), std::ios::out);
        ofs.seekp(0, std::ios::beg);
        ofs << *this;
        ofs.close();
        ILOG_INFO(ILX_ICONPACK, "Created cached icons file: %s\n", cacheFile.c_str());
    }

    return true;
}

void
IconPack::release()
{
    _iconMap.clear();
    delete _iconPack;
    _iconPack = NULL;
}

std::istream&
operator>>(std::istream& is, IconPack& obj)
{
    obj.release();

    int mapSize = 0;
    std::string iconName;
    Point p;
    obj._iconPack = new Image();

    is >> obj._iconSize;
    is.ignore(1);
    is >> *obj._iconPack;
    is >> mapSize;
    for (int i = 0; i < mapSize; ++i)
    {
        is >> iconName;
        is.ignore(1);
        is >> p;
        obj._iconMap.insert(std::make_pair(iconName, p));
    }
    is.ignore(1);

    return is;
}

std::ostream&
operator<<(std::ostream& os, const IconPack& obj)
{
    os << obj._iconSize << std::endl;
    os << *obj._iconPack << std::endl;
    os << obj._iconMap.size() << std::endl;
    for (IconPack::IconMap::const_iterator it = obj._iconMap.begin(); it != obj._iconMap.end(); ++it)
        os << it->first << "\t" << it->second << std::endl;

    return os;
}

} /* namespace ilixi */
