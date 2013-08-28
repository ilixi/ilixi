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

#include <graphics/Palette.h>
#include <core/Logger.h>
#include <lib/XMLReader.h>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_PALETTE, "ilixi/graphics/Palette", "Palette Parser");

ColorGroup::ColorGroup()
        : base(1, 1, 1),
          baseText(0, 0, 0),
          baseAlt(0.9, 0.9, 0.9),
          baseAltText(0, 0, 0),
          bg(0.43, 0.43, 0.43),
          border(0.3, 0.3, 0.3),
          fill(0, 0, 1),
          text(1, 1, 1)
{
}

ColorGroup::ColorGroup(const ColorGroup& c)
        : base(c.base),
          baseText(c.baseText),
          baseAlt(c.baseAlt),
          baseAltText(c.baseAltText),
          bg(c.bg),
          border(c.border),
          fill(c.fill),
          text(c.text)
{
}

ColorGroup::ColorGroup(Color c1, Color c2, Color c3, Color c4, Color c5, Color c6)
        : base(c1),
          baseAlt(c2),
          bg(c3),
          border(c4),
          fill(c5),
          text(c6)
{
}

void
ColorGroup::setColors(Color c1, Color c2, Color c3, Color c4, Color c5, Color c6)
{
    base = c1;
    baseAlt = c2;
    bg = c3;
    border = c4;
    fill = c5;
    text = c6;
}

Palette::Palette()
        : bg(0, 0, 0),
          focus(0, 0, 0),
          text(1, 1, 1),
          textDisabled(.9, .9, .9),
          _default(),
          _exposed(),
          _pressed(),
          _disabled()
{
}

ColorGroup&
Palette::getGroup(WidgetState state)
{
    if (state & DisabledState)
        return _disabled;
    else if (state & PressedState)
        return _pressed;
    else if (state & ExposedState)
        return _exposed;
    else
        return _default;
}

bool
Palette::parsePalette(const char* palette)
{
    XMLReader xml;
    if (xml.loadFile(palette) == false)
    {
        ILOG_FATAL(ILX_PALETTE, "Could not parse palette!\n");
        return false;
    }

    xmlNodePtr group = xml.currentNode();
    xmlNodePtr element, colorNode, hexNode;
    Color c;

    while (group != NULL)
    {
        element = group->children;
        while (element != NULL)
        {
            colorNode = element->children;
            xmlChar* alpha = xmlGetProp(colorNode, (xmlChar*) "alpha");

            hexNode = colorNode->children;
            if (xmlStrcmp(hexNode->name, (xmlChar*) "hex") == 0)
            {
                xmlChar* hex = xmlNodeGetContent(hexNode);
                c.setFromHex((char*) hex, atof((char*) alpha));
                xmlFree(hex);
            } else
            {
                xmlChar* r = xmlNodeGetContent(hexNode);
                xmlChar* g = xmlNodeGetContent(hexNode->next);
                xmlChar* b = xmlNodeGetContent(hexNode->next->next);
                c.setRGBA(atoi((char*) r), atoi((char*) g), atoi((char*) b),
                          atof((char*) alpha));
                xmlFree(r);
                xmlFree(g);
                xmlFree(b);
            }

            // check group name
            if (xmlStrcmp(group->name, (xmlChar*) "application") == 0)
            {
                if (xmlStrcmp(element->name, (xmlChar*) "bg") == 0)
                    bg = c;
                else if (xmlStrcmp(element->name, (xmlChar*) "focus") == 0)
                    focus = c;
                else if (xmlStrcmp(element->name, (xmlChar*) "text") == 0)
                    text = c;
                else if (xmlStrcmp(element->name, (xmlChar*) "textDisabled") == 0)
                    textDisabled = c;
            }

            else if (xmlStrcmp(group->name, (xmlChar*) "default") == 0)
            {
                if (xmlStrcmp(element->name, (xmlChar*) "base") == 0)
                    _default.base = c;
                else if (xmlStrcmp(element->name, (xmlChar*) "baseText") == 0)
                    _default.baseText = c;
                else if (xmlStrcmp(element->name, (xmlChar*) "baseAlt") == 0)
                    _default.baseAlt = c;
                else if (xmlStrcmp(element->name, (xmlChar*) "baseAltText") == 0)
                    _default.baseAltText = c;
                else if (xmlStrcmp(element->name, (xmlChar*) "bg") == 0)
                    _default.bg = c;
                else if (xmlStrcmp(element->name, (xmlChar*) "border") == 0)
                    _default.border = c;
                else if (xmlStrcmp(element->name, (xmlChar*) "fill") == 0)
                    _default.fill = c;
                else if (xmlStrcmp(element->name, (xmlChar*) "text") == 0)
                    _default.text = c;
            }

            else if (xmlStrcmp(group->name, (xmlChar*) "disabled") == 0)
            {
                if (xmlStrcmp(element->name, (xmlChar*) "base") == 0)
                    _disabled.base = c;
                else if (xmlStrcmp(element->name, (xmlChar*) "baseText") == 0)
                    _disabled.baseText = c;
                else if (xmlStrcmp(element->name, (xmlChar*) "baseAlt") == 0)
                    _disabled.baseAlt = c;
                else if (xmlStrcmp(element->name, (xmlChar*) "baseAltText") == 0)
                    _disabled.baseAltText = c;
                else if (xmlStrcmp(element->name, (xmlChar*) "bg") == 0)
                    _disabled.bg = c;
                else if (xmlStrcmp(element->name, (xmlChar*) "border") == 0)
                    _disabled.border = c;
                else if (xmlStrcmp(element->name, (xmlChar*) "fill") == 0)
                    _disabled.fill = c;
                else if (xmlStrcmp(element->name, (xmlChar*) "text") == 0)
                    _disabled.text = c;
            }

            else if (xmlStrcmp(group->name, (xmlChar*) "exposed") == 0)
            {
                if (xmlStrcmp(element->name, (xmlChar*) "base") == 0)
                    _exposed.base = c;
                else if (xmlStrcmp(element->name, (xmlChar*) "baseText") == 0)
                    _exposed.baseText = c;
                else if (xmlStrcmp(element->name, (xmlChar*) "baseAlt") == 0)
                    _exposed.baseAlt = c;
                else if (xmlStrcmp(element->name, (xmlChar*) "baseAltText") == 0)
                    _exposed.baseAltText = c;
                else if (xmlStrcmp(element->name, (xmlChar*) "bg") == 0)
                    _exposed.bg = c;
                else if (xmlStrcmp(element->name, (xmlChar*) "border") == 0)
                    _exposed.border = c;
                else if (xmlStrcmp(element->name, (xmlChar*) "fill") == 0)
                    _exposed.fill = c;
                else if (xmlStrcmp(element->name, (xmlChar*) "text") == 0)
                    _exposed.text = c;
            }

            else if (xmlStrcmp(group->name, (xmlChar*) "pressed") == 0)
            {
                if (xmlStrcmp(element->name, (xmlChar*) "base") == 0)
                    _pressed.base = c;
                else if (xmlStrcmp(element->name, (xmlChar*) "baseText") == 0)
                    _pressed.baseText = c;
                else if (xmlStrcmp(element->name, (xmlChar*) "baseAlt") == 0)
                    _pressed.baseAlt = c;
                else if (xmlStrcmp(element->name, (xmlChar*) "baseAltText") == 0)
                    _pressed.baseAltText = c;
                else if (xmlStrcmp(element->name, (xmlChar*) "bg") == 0)
                    _pressed.bg = c;
                else if (xmlStrcmp(element->name, (xmlChar*) "border") == 0)
                    _pressed.border = c;
                else if (xmlStrcmp(element->name, (xmlChar*) "fill") == 0)
                    _pressed.fill = c;
                else if (xmlStrcmp(element->name, (xmlChar*) "text") == 0)
                    _pressed.text = c;
            }

            element = element->next;
            xmlFree(alpha);
        }
        group = group->next;
    }

    ILOG_INFO(ILX_PALETTE, "Parsed palette file: %s\n", palette);
    return true;
}

} /* namespace ilixi */
