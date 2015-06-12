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

#include <graphics/FontPack.h>
#include <lib/FileSystem.h>
#include <lib/XMLReader.h>
#include <core/Logger.h>
#include <types/FontCache.h>
#include <fstream>

namespace ilixi
{

D_DEBUG_DOMAIN(ILX_FONTPACK, "ilixi/graphics/FontPack", "FontPack");

FontPack::FontPack()
        : _buttonFont(NULL),
          _defaultFont(NULL),
          _inputFont(NULL),
          _titleFont(NULL),
          _microFont(NULL)
{
}

FontPack::~FontPack()
{
    ILOG_TRACE(ILX_FONTPACK);
    release();
}

Font*
FontPack::getFont(StyleHint::FontHint font) const
{
    ILOG_TRACE(ILX_FONTPACK);
    switch (font)
    {
    case StyleHint::ButtonFont:
        return _buttonFont;
    case StyleHint::TitleFont:
        return _titleFont;
    case StyleHint::InputFont:
        return _inputFont;
    case StyleHint::MicroFont:
        return _microFont;
    default:
        return _defaultFont;
    }
}

Font*
FontPack::getCustomFont(const std::string& name) const
{
    ILOG_TRACE(ILX_FONTPACK);
    ILOG_DEBUG(ILX_FONTPACK, " -> name: %s\n", name.c_str());
    FontMap::const_iterator it = _fontMap.find(name);
    if (it != _fontMap.end())
        return it->second;
    ILOG_WARNING(ILX_FONTPACK, " -> Cannot find font: %s\n", name.c_str());
    return getFont(StyleHint::DefaultFont);
}

bool
FontPack::parseFonts(const char* fontsFile)
{
    ILOG_TRACE(ILX_FONTPACK);
    ILOG_DEBUG(ILX_FONTPACK, " -> file: %s\n", fontsFile);

    std::string cacheFile = PrintF("%s%u.sxml", FileSystem::ilxDirectory().c_str(), createHash(fontsFile));
    if (difftime(FileSystem::getModificationTime(cacheFile), FileSystem::getModificationTime(fontsFile)) > 0)
    {
        ILOG_DEBUG(ILX_FONTPACK, " -> Parsing cached fonts file: %s\n", cacheFile.c_str());
        std::ifstream ifs(cacheFile.c_str(), std::ios::in);
        ifs >> *this;
        ifs.close();
        ILOG_INFO(ILX_FONTPACK, "Parsed cached fonts file: %s\n", cacheFile.c_str());
    } else
    {
        XMLReader xml;
        if (xml.loadFile(fontsFile) == false)
        {
            ILOG_FATAL(ILX_FONTPACK, "Could not parse fonts!\n");
            return false;
        }

        xmlNodePtr node = xml.currentNode();

        release();

        while (node != NULL)
        {
            ILOG_DEBUG(ILX_FONTPACK, " -> font: %s\n", node->name);
            xmlChar* fileC = xmlNodeGetContent(node->children);
            xmlChar* sizeC = xmlNodeGetContent(node->children->next);
            xmlChar* styleC = xmlNodeGetContent(node->children->next->next);

            Font::Style fontStyle = Font::Plain;
            if (styleC)
            {
                if (xmlStrcmp(styleC, (xmlChar *) "italic") == 0)
                    fontStyle = Font::Italic;
                else if (xmlStrcmp(styleC, (xmlChar *) "bold") == 0)
                    fontStyle = Font::Bold;
                xmlFree(styleC);
            }

            if (xmlStrcmp(node->name, (xmlChar*) "DefaultFont") == 0)
            {
                _defaultFont = new Font((char*) fileC, atoi((char*) sizeC));
                _defaultFont->setStyle(fontStyle);
                _defaultFont->dfbFont();
            } else if (xmlStrcmp(node->name, (xmlChar*) "ButtonFont") == 0)
            {
                _buttonFont = new Font((char*) fileC, atoi((char*) sizeC));
                _buttonFont->setStyle(fontStyle);
                _buttonFont->dfbFont();
            }

            else if (xmlStrcmp(node->name, (xmlChar*) "InputFont") == 0)
            {
                _inputFont = new Font((char*) fileC, atoi((char*) sizeC));
                _inputFont->setStyle(fontStyle);
                _inputFont->dfbFont();
            }

            else if (xmlStrcmp(node->name, (xmlChar*) "TitleFont") == 0)
            {
                _titleFont = new Font((char*) fileC, atoi((char*) sizeC));
                _titleFont->setStyle(fontStyle);
                _titleFont->dfbFont();
            }

            else if (xmlStrcmp(node->name, (xmlChar*) "MicroFont") == 0)
            {
                _microFont = new Font((char*) fileC, atoi((char*) sizeC));
                _microFont->setStyle(fontStyle);
                _microFont->dfbFont();
            }

            else if (xmlStrcmp(node->name, (xmlChar*) "CustomFont") == 0)
            {
                xmlChar* fontName = xmlGetProp(node, (xmlChar*) "name");
                FontMap::const_iterator it = _fontMap.find((char*) fontName);
                if (it == _fontMap.end())
                {
                    Font* cFont = new Font((char*) fileC, atoi((char*) sizeC));
                    cFont->setStyle(fontStyle);
                    cFont->dfbFont();
                    std::pair<FontMap::iterator, bool> res = _fontMap.insert(std::make_pair((char*) fontName, cFont));
                } else
                    ILOG_WARNING(ILX_FONTPACK, "CustomFont '%s' already exists!\n", fontName);
                xmlFree(fontName);
            }

            xmlFree(fileC);
            xmlFree(sizeC);
            node = node->next;
        }
        ILOG_INFO(ILX_FONTPACK, "Parsed fonts file: %s\n", fontsFile);

        std::ofstream ofs(cacheFile.c_str(), std::ios::out);
        ofs.seekp(0, std::ios::beg);
        ofs << *this;
        ofs.close();
        ILOG_INFO(ILX_FONTPACK, "Created cached fonts file: %s\n", cacheFile.c_str());
    }

    return true;
}

void
FontPack::release()
{
    ILOG_TRACE(ILX_FONTPACK);
    delete _buttonFont;
    delete _defaultFont;
    delete _inputFont;
    delete _titleFont;
    delete _microFont;

    for (FontPack::FontMap::iterator it = _fontMap.begin(); it != _fontMap.end(); ++it)
        delete it->second;
    _fontMap.clear();

    _buttonFont = NULL;
    _defaultFont = NULL;
    _inputFont = NULL;
    _titleFont = NULL;
    _microFont = NULL;

    FontCache::Instance()->logEntries();
}

std::istream&
operator>>(std::istream& is, FontPack& obj)
{
    ILOG_TRACE_F(ILX_FONTPACK);
    obj.release();

    obj._buttonFont = new Font();
    obj._defaultFont = new Font();
    obj._inputFont = new Font();
    obj._titleFont = new Font();
    obj._microFont = new Font();

    is >> *obj._buttonFont;
    is.ignore(1);
    is >> *obj._defaultFont;
    is.ignore(1);
    is >> *obj._inputFont;
    is.ignore(1);
    is >> *obj._titleFont;
    is.ignore(1);
    is >> *obj._microFont;
    is.ignore(1);

    int mapSize = 0;
    std::string fontName;
    is >> mapSize;
    for (int i = 0; i < mapSize; ++i)
    {
        is >> fontName;
        is.ignore(1);
        Font* font = new Font();
        is >> *font;
        font->dfbFont();
        obj._fontMap.insert(std::make_pair(fontName, font));
    }
    is.ignore(1);

    *obj._buttonFont->dfbFont();
    *obj._defaultFont->dfbFont();
    *obj._inputFont->dfbFont();
    *obj._titleFont->dfbFont();
    *obj._microFont->dfbFont();

    return is;
}

std::ostream&
operator<<(std::ostream& os, const FontPack& obj)
{
    ILOG_TRACE_F(ILX_FONTPACK);
    os << *obj._buttonFont << std::endl;
    os << *obj._defaultFont << std::endl;
    os << *obj._inputFont << std::endl;
    os << *obj._titleFont << std::endl;
    os << *obj._microFont << std::endl;

    os << obj._fontMap.size() << std::endl;
    for (FontPack::FontMap::const_iterator it = obj._fontMap.begin(); it != obj._fontMap.end(); ++it)
        os << it->first << std::endl << *((Font*) it->second) << std::endl;

    return os;
}

} /* namespace ilixi */
