/*
 Copyright 2010-2015 Tarik Sekmen.

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

#include <graphics/ImagePack.h>
#include <core/PlatformManager.h>
#include <lib/FileSystem.h>
#include <lib/XMLReader.h>
#include <core/Logger.h>
#include <fstream>

namespace ilixi
{

D_DEBUG_DOMAIN(ILX_IMAGEPACK, "ilixi/graphics/ImagePack", "ImagePack");

ImagePack::ImagePack(const char* file)
        : _pack(NULL)
{
    parsePack(file);
}

ImagePack::~ImagePack()
{
    release();
}

Image*
ImagePack::getImage(const std::string& name) const
{
    ILOG_TRACE(ILX_IMAGEPACK);
    ILOG_DEBUG(ILX_IMAGEPACK, " -> name: %s\n", name.c_str());
    ImageMap::const_iterator it = _map.find(name);
    if (it != _map.end())
    {
        ILOG_DEBUG(ILX_IMAGEPACK, " -> %s @ (%d, %d, %d, %d)\n", name.c_str(), it->second.x(), it->second.y(), it->second.width(), it->second.height());
        return new Image(_pack, it->second);
    }
    ILOG_WARNING(ILX_IMAGEPACK, " -> Cannot find image: %s\n", name.c_str());
    return NULL;
}

bool
ImagePack::parsePack(const char* packFile)
{
    ILOG_TRACE(ILX_IMAGEPACK);
    ILOG_DEBUG(ILX_IMAGEPACK, " -> file: %s\n", packFile);

    std::string cacheFile = PrintF("%s%u.sxml", FileSystem::ilxDirectory().c_str(), createHash(packFile));
    ILOG_DEBUG(ILX_IMAGEPACK, " -> cache file: %s\n", cacheFile.c_str());
    if (difftime(FileSystem::getModificationTime(cacheFile), FileSystem::getModificationTime(packFile)) > 0)
    {
        ILOG_DEBUG(ILX_IMAGEPACK, " -> Parsing cached image pack file.\n");
        std::ifstream ifs(cacheFile.c_str(), std::ios::in);
        ifs >> *this;
        ifs.close();
        ILOG_INFO(ILX_IMAGEPACK, "Parsed cached image pack file: %s\n", cacheFile.c_str());
    } else
    {
        XMLReader xml;
        if (xml.loadFile(packFile) == false)
        {
            ILOG_FATAL(ILX_IMAGEPACK, "Could not parse image pack!\n");
            return false;
        }

        xmlNodePtr root = xml.root();
        xmlNodePtr node = xml.currentNode();

        release();

        xmlChar* imgFile = xmlGetProp(root, (xmlChar*) "resource");
        std::string path = (char*) imgFile;
        std::string file;
        size_t found = path.find("@IMGDIR:");
        if (found != std::string::npos)
        {
            file = ILIXI_DATADIR"images/";
            file.append(path.substr(found + 8, std::string::npos));
            ILOG_DEBUG(ILX_IMAGEPACK, " -> image file: %s\n", file.c_str());
        } else
        {
            found = path.find("@ILX_THEMEDIR:");
            if (found != std::string::npos)
            {
                file.append(PlatformManager::instance().getThemeDirectory());
                file.append(path.substr(found + 14, std::string::npos));
                ILOG_DEBUG(ILX_IMAGEPACK, " -> image file: %s\n", file.c_str());
            } else
                file = path;
        }

        _pack = new Image(file);
        xmlFree(imgFile);

        while (node != NULL)
        {
            xmlChar* imgName = xmlGetProp(node, (xmlChar*) "name");
            xmlChar* imgX = xmlGetProp(node, (xmlChar*) "x");
            xmlChar* imgY = xmlGetProp(node, (xmlChar*) "y");
            xmlChar* imgW = xmlGetProp(node, (xmlChar*) "w");
            xmlChar* imgH = xmlGetProp(node, (xmlChar*) "h");
            std::pair<ImageMap::iterator, bool> res = _map.insert(std::make_pair((char*) imgName, Rectangle(atoi((char*) imgX), atoi((char*) imgY), atoi((char*) imgW), atoi((char*) imgH))));
            if (!res.second)
                ILOG_WARNING(ILX_IMAGEPACK, "Image %s already exists!\n", imgName);
            else
                ILOG_DEBUG(ILX_IMAGEPACK, " -> %s - %d, %d, %d, %d\n", imgName, res.first->second.x(), res.first->second.y(), res.first->second.width(), res.first->second.height());

            xmlFree(imgH);
            xmlFree(imgW);
            xmlFree(imgY);
            xmlFree(imgX);
            xmlFree(imgName);
            node = node->next;
        }

        ILOG_INFO(ILX_IMAGEPACK, "Parsed image pack file: %s\n", packFile);

        std::ofstream ofs(cacheFile.c_str(), std::ios::out);
        ofs.seekp(0, std::ios::beg);
        ofs << *this;
        ofs.close();
        ILOG_INFO(ILX_IMAGEPACK, "Created cached image pack file: %s\n", cacheFile.c_str());
    }

    return true;
}

void
ImagePack::release()
{
    _map.clear();
    delete _pack;
    _pack = NULL;
}

std::istream&
operator>>(std::istream& is, ImagePack& obj)
{
    obj.release();

    int mapSize = 0;
    std::string imgName;
    Rectangle r;
    obj._pack = new Image();

    is >> *obj._pack;
    is >> mapSize;
    for (int i = 0; i < mapSize; ++i)
    {
        is >> imgName;
        is.ignore(1);
        is >> r;
        obj._map.insert(std::make_pair(imgName, r));
    }
    is.ignore(1);

    return is;
}

std::ostream&
operator<<(std::ostream& os, const ImagePack& obj)
{
    os << *obj._pack << std::endl;
    os << obj._map.size() << std::endl;
    for (ImagePack::ImageMap::const_iterator it = obj._map.begin(); it != obj._map.end(); ++it)
        os << it->first << "\t" << it->second << std::endl;

    return os;
}

} /* namespace ilixi */
