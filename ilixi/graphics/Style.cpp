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

#include <graphics/Style.h>
#include <core/Logger.h>
#include <libxml/parser.h>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_STYLE, "ilixi/graphics/Style", "Style Parser");

Style::Style()
        : _buttonFont(NULL),
          _defaultFont(NULL),
          _inputFont(NULL),
          _titleFont(NULL),
          _iconPack(NULL),
          _defaultIconSize(48),
          _pack(NULL)
{
    parseStyle(ILIXI_DATADIR"def_style.xml");
}

Style::~Style()
{
    release();
}

Image*
Style::getIcon(std::string name)
{
    ILOG_TRACE(ILX_STYLE);
    IconMap::iterator it = _iconMap.find(name);
    if (it != _iconMap.end())
    {
        ILOG_DEBUG(
                ILX_STYLE,
                " -> %s @ (%d, %d, %d, %d)\n", name.c_str(), it->second.x(), it->second.y(), _defaultIconSize, _defaultIconSize);
        return new Image(
                _iconPack,
                Rectangle(it->second.x(), it->second.y(), _defaultIconSize, _defaultIconSize));
    }
    ILOG_WARNING(ILX_STYLE, " -> Cannot find icon: %s\n", name.c_str());
    return NULL;
}

void
Style::release()
{
    // fonts
    delete _buttonFont;
    delete _defaultFont;
    delete _inputFont;
    delete _titleFont;

    // images
    delete _iconPack;
    delete _pack;
}

bool
Style::parseStyle(const char* style)
{
    ILOG_TRACE(ILX_STYLE);
    ILOG_DEBUG(ILX_STYLE, " -> file: %s\n", style);
    xmlParserCtxtPtr ctxt;
    xmlDocPtr doc;

    ctxt = xmlNewParserCtxt();
    if (ctxt == NULL)
    {
        ILOG_ERROR(ILX_STYLE, "Failed to allocate parser context\n");
        return false;
    }

    doc = xmlCtxtReadFile(
            ctxt,
            style,
            NULL,
            XML_PARSE_DTDATTR | XML_PARSE_NOENT | XML_PARSE_DTDVALID | XML_PARSE_NOBLANKS);

    if (doc == NULL)
    {
        xmlFreeParserCtxt(ctxt);
        ILOG_ERROR(ILX_STYLE, "Failed to parse style: %s\n", style);
        return false;
    }

    if (ctxt->valid == 0)
    {
        xmlFreeDoc(doc);
        xmlFreeParserCtxt(ctxt);
        ILOG_ERROR(ILX_STYLE, "Failed to validate style: %s\n", style);
        return false;
    }

    release();

    xmlNodePtr root = xmlDocGetRootElement(doc);
    xmlNodePtr group = root->xmlChildrenNode;

    while (group != NULL)
    {
        if (xmlStrcmp(group->name, (xmlChar*) "fonts") == 0)
        {
            ILOG_DEBUG(ILX_STYLE, " -> parsing fonts...\n");
            parseFonts(group->children);
        } else if (xmlStrcmp(group->name, (xmlChar*) "icons") == 0)
        {
            ILOG_DEBUG(ILX_STYLE, " -> parsing icons...\n");
            xmlChar* imgFile = xmlGetProp(group, (xmlChar*) "resource");
            xmlChar* imgDefSize = xmlGetProp(group, (xmlChar*) "defaultSize");
            _iconPack = new Image(
                    std::string(
                            ILIXI_DATADIR"" + std::string((char*) imgFile)));
            _defaultIconSize = atoi((char*) imgDefSize);
            parseIcons(group->children);
            xmlFree(imgDefSize);
            xmlFree(imgFile);
        } else if (xmlStrcmp(group->name, (xmlChar*) "pack") == 0)
        {
            ILOG_DEBUG(ILX_STYLE, " -> parsing theme...\n");
            xmlChar* imgFile = xmlGetProp(group, (xmlChar*) "resource");
            _pack = new Image(
                    std::string(ILIXI_DATADIR + std::string((char*) imgFile)));
            parseTheme(group->children);
            xmlFree(imgFile);
        }

        group = group->next;
    } // end while(group)

    xmlFreeDoc(doc);
    xmlFreeParserCtxt(ctxt);
    ILOG_INFO(ILX_STYLE, "Parsed style file: %s\n", style);
    return true;
}

void
Style::parseFonts(xmlNodePtr node)
{
    while (node != NULL)
    {
        ILOG_DEBUG(ILX_STYLE, " -> font: %s...\n", node->name);
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
        }

        if (xmlStrcmp(node->name, (xmlChar*) "defaultFont") == 0)
        {
            _defaultFont = new Font((char*) fileC, atoi((char*) sizeC));
            _defaultFont->setStyle(fontStyle);
        } else if (xmlStrcmp(node->name, (xmlChar*) "buttonFont") == 0)
        {
            _buttonFont = new Font((char*) fileC, atoi((char*) sizeC));
            _buttonFont->setStyle(fontStyle);
        }

        else if (xmlStrcmp(node->name, (xmlChar*) "inputFont") == 0)
        {
            _inputFont = new Font((char*) fileC, atoi((char*) sizeC));
            _inputFont->setStyle(fontStyle);
        }

        else if (xmlStrcmp(node->name, (xmlChar*) "titleFont") == 0)
        {
            _titleFont = new Font((char*) fileC, atoi((char*) sizeC));
            _titleFont->setStyle(fontStyle);
        }

        xmlFree(fileC);
        xmlFree(sizeC);
        node = node->next;
    }
    ILOG_DEBUG(ILX_STYLE, "Parsed fonts.\n");
}

void
Style::parseIcons(xmlNodePtr node)
{
    _iconMap.clear();
    while (node != NULL)
    {
        xmlChar* iconName = xmlGetProp(node, (xmlChar*) "name");
        xmlChar* iconRow = xmlGetProp(node, (xmlChar*) "row");
        xmlChar* iconCol = xmlGetProp(node, (xmlChar*) "col");
        int x = (atoi((char*) iconCol) - 1) * _defaultIconSize;
        int y = (atoi((char*) iconRow) - 1) * _defaultIconSize;
        std::pair<IconMap::iterator, bool> res = _iconMap.insert(
                std::make_pair((char*) iconName, Point(x, y)));
        if (!res.second)
            ILOG_WARNING(ILX_STYLE, "Icon %s already exists!\n", iconName);
        else
            ILOG_DEBUG(ILX_STYLE, " -> %s - %d, %d\n", iconName, res.first->second.x(), res.first->second.y());

        xmlFree(iconCol);
        xmlFree(iconRow);
        xmlFree(iconName);
        node = node->next;
    }
}

void
Style::parseTheme(xmlNodePtr node)
{
    xmlNodePtr state;
    while (node != NULL)
    {
        ILOG_DEBUG(ILX_STYLE, " Parsing %s...\n", node->name);
        if (xmlStrcmp(node->name, (xmlChar*) "PushButton") == 0)
        {
            state = node->children;
            while (state != NULL)
            {
                ILOG_DEBUG(ILX_STYLE, "  state: %s...\n", state->name);

                if (xmlStrcmp(state->name, (xmlChar*) "def3") == 0)
                    get3Rectangle(state->children, pb.def);

                else if (xmlStrcmp(state->name, (xmlChar*) "pre3") == 0)
                    get3Rectangle(state->children, pb.pre);

                else if (xmlStrcmp(state->name, (xmlChar*) "exp3") == 0)
                    get3Rectangle(state->children, pb.exp);

                else if (xmlStrcmp(state->name, (xmlChar*) "dis3") == 0)
                    get3Rectangle(state->children, pb.dis);

                else if (xmlStrcmp(state->name, (xmlChar*) "foc3") == 0)
                    get3Rectangle(state->children, pb.foc);

                state = state->next;
            }
        } else if (xmlStrcmp(node->name, (xmlChar*) "CheckBox") == 0)
        {
            state = node->children;
            while (state != NULL)
            {
                ILOG_DEBUG(ILX_STYLE, "  state: %s...\n", state->name);

                if (xmlStrcmp(state->name, (xmlChar*) "def1") == 0)
                    getRectangle(state->children, cb.def);

                else if (xmlStrcmp(state->name, (xmlChar*) "pre1") == 0)
                    getRectangle(state->children, cb.pre);

                else if (xmlStrcmp(state->name, (xmlChar*) "exp1") == 0)
                    getRectangle(state->children, cb.exp);

                else if (xmlStrcmp(state->name, (xmlChar*) "dis1") == 0)
                    getRectangle(state->children, cb.dis);

                else if (xmlStrcmp(state->name, (xmlChar*) "foc1") == 0)
                    getRectangle(state->children, cb.foc);

                else if (xmlStrcmp(state->name, (xmlChar*) "check") == 0)
                    getRectangle(state, check);

                else if (xmlStrcmp(state->name, (xmlChar*) "tri-check") == 0)
                    getRectangle(state, tri_check);

                state = state->next;
            }
        } else if (xmlStrcmp(node->name, (xmlChar*) "RadioButton") == 0)
        {
            state = node->children;
            while (state != NULL)
            {
                ILOG_DEBUG(ILX_STYLE, "  state: %s...\n", state->name);

                if (xmlStrcmp(state->name, (xmlChar*) "def1") == 0)
                    getRectangle(state->children, rb.def);

                else if (xmlStrcmp(state->name, (xmlChar*) "pre1") == 0)
                    getRectangle(state->children, rb.pre);

                else if (xmlStrcmp(state->name, (xmlChar*) "exp1") == 0)
                    getRectangle(state->children, rb.exp);

                else if (xmlStrcmp(state->name, (xmlChar*) "dis1") == 0)
                    getRectangle(state->children, rb.dis);

                else if (xmlStrcmp(state->name, (xmlChar*) "foc1") == 0)
                    getRectangle(state->children, rb.foc);

                else if (xmlStrcmp(state->name, (xmlChar*) "radioOff") == 0)
                    getRectangle(state, radioOff);

                else if (xmlStrcmp(state->name, (xmlChar*) "radioOn") == 0)
                    getRectangle(state, radioOn);

                state = state->next;
            }
        }

        else if (xmlStrcmp(node->name, (xmlChar*) "ProgressBar") == 0)
        {
            state = node->children;
            while (state != NULL)
            {
                ILOG_DEBUG(ILX_STYLE, "  state: %s...\n", state->name);

                if (xmlStrcmp(state->name, (xmlChar*) "def3") == 0)
                    get3Rectangle(state->children, pr.def);

                else if (xmlStrcmp(state->name, (xmlChar*) "dis3") == 0)
                    get3Rectangle(state->children, pr.dis);

                else if (xmlStrcmp(state->name, (xmlChar*) "pIndicator") == 0)
                {
                    xmlNodePtr iState = state->children;
                    while (iState != NULL)
                    {
                        if (xmlStrcmp(iState->name, (xmlChar*) "def3") == 0)
                            get3Rectangle(iState->children, prI.def);

                        else if (xmlStrcmp(state->name, (xmlChar*) "dis3") == 0)
                            get3Rectangle(iState->children, prI.dis);

                        iState = iState->next;
                    }
                }

                state = state->next;
            }
        }

        else if (xmlStrcmp(node->name, (xmlChar*) "Slider") == 0)
        {
            state = node->children;
            while (state != NULL)
            {
                ILOG_DEBUG(ILX_STYLE, "  state: %s...\n", state->name);

                if (xmlStrcmp(state->name, (xmlChar*) "def3") == 0)
                    get3Rectangle(state->children, sl.def);
                else if (xmlStrcmp(state->name, (xmlChar*) "dis3") == 0)
                    get3Rectangle(state->children, sl.dis);
                else if (xmlStrcmp(state->name, (xmlChar*) "pre3") == 0)
                    get3Rectangle(state->children, sl.defV);
                else if (xmlStrcmp(state->name, (xmlChar*) "exp3") == 0)
                    get3Rectangle(state->children, sl.disV);
                else if (xmlStrcmp(state->name, (xmlChar*) "sIndicator") == 0)
                {
                    xmlNodePtr iState = state->children;
                    while (iState != NULL)
                    {

                        if (xmlStrcmp(iState->name, (xmlChar*) "def1") == 0)
                            getRectangle(iState->children, slI.def);

                        else if (xmlStrcmp(iState->name, (xmlChar*) "pre1") == 0)
                            getRectangle(iState->children, slI.pre);
                        else if (xmlStrcmp(iState->name, (xmlChar*) "exp1") == 0)
                            getRectangle(iState->children, slI.exp);

                        else if (xmlStrcmp(iState->name, (xmlChar*) "dis1") == 0)
                            getRectangle(iState->children, slI.dis);

                        else if (xmlStrcmp(iState->name, (xmlChar*) "foc1") == 0)
                            getRectangle(iState->children, slI.foc);

                        iState = iState->next;
                    }
                }

                state = state->next;
            }
        } else if (xmlStrcmp(node->name, (xmlChar*) "Frame") == 0)
        {
            state = node->children;
            while (state != NULL)
            {
                ILOG_DEBUG(ILX_STYLE, "  state: %s...\n", state->name);

                if (xmlStrcmp(state->name, (xmlChar*) "def9") == 0)
                    get9Rectangle(state->children, fr.def);

                else if (xmlStrcmp(state->name, (xmlChar*) "dis9") == 0)
                    get9Rectangle(state->children, fr.dis);

                state = state->next;
            }
        } else if (xmlStrcmp(node->name, (xmlChar*) "LineInput") == 0)
        {
            state = node->children;
            while (state != NULL)
            {
                ILOG_DEBUG(ILX_STYLE, "  state: %s...\n", state->name);

                if (xmlStrcmp(state->name, (xmlChar*) "def9") == 0)
                    get9Rectangle(state->children, li.def);

                else if (xmlStrcmp(state->name, (xmlChar*) "pre9") == 0)
                    get9Rectangle(state->children, li.pre);

                else if (xmlStrcmp(state->name, (xmlChar*) "exp9") == 0)
                    get9Rectangle(state->children, li.exp);

                else if (xmlStrcmp(state->name, (xmlChar*) "dis9") == 0)
                    get9Rectangle(state->children, li.dis);

                else if (xmlStrcmp(state->name, (xmlChar*) "foc9") == 0)
                    get9Rectangle(state->children, li.foc);

                state = state->next;
            }
        } else if (xmlStrcmp(node->name, (xmlChar*) "Tab") == 0)
        {
            state = node->children;
            while (state != NULL)
            {
                ILOG_DEBUG(ILX_STYLE, "  state: %s...\n", state->name);

                if (xmlStrcmp(state->name, (xmlChar*) "def9") == 0)
                    get9Rectangle(state->children, tab.def);

                else if (xmlStrcmp(state->name, (xmlChar*) "dis9") == 0)
                    get9Rectangle(state->children, tab.dis);

                state = state->next;
            }
        } else if (xmlStrcmp(node->name, (xmlChar*) "ToolButton") == 0)
        {
            state = node->children;
            while (state != NULL)
            {
                ILOG_DEBUG(ILX_STYLE, "  state: %s...\n", state->name);

                if (xmlStrcmp(state->name, (xmlChar*) "def9") == 0)
                    get9Rectangle(state->children, tb.def);

                else if (xmlStrcmp(state->name, (xmlChar*) "pre9") == 0)
                    get9Rectangle(state->children, tb.pre);

                else if (xmlStrcmp(state->name, (xmlChar*) "exp9") == 0)
                    get9Rectangle(state->children, tb.exp);

                else if (xmlStrcmp(state->name, (xmlChar*) "dis9") == 0)
                    get9Rectangle(state->children, tb.dis);

                else if (xmlStrcmp(state->name, (xmlChar*) "foc9") == 0)
                    get9Rectangle(state->children, tb.foc);

                state = state->next;
            }
        }

        node = node->next;
    } // end while(element)
}

void
Style::getRectangle(xmlNodePtr node, Rectangle& r)
{
    xmlChar* x = xmlGetProp(node, (xmlChar*) "x");
    xmlChar* y = xmlGetProp(node, (xmlChar*) "y");
    xmlChar* w = xmlGetProp(node, (xmlChar*) "w");
    xmlChar* h = xmlGetProp(node, (xmlChar*) "h");

    r.setRectangle(atoi((char*) x), atoi((char*) y), atoi((char*) w),
                   atoi((char*) h));
    ILOG_DEBUG(
            ILX_STYLE,
            "   Rectangle(%d, %d, %d, %d)\n", r.x(), r.y(), r.width(), r.height());
    xmlFree(x);
    xmlFree(y);
    xmlFree(w);
    xmlFree(h);
}

void
Style::get3Rectangle(xmlNodePtr node, r3& r)
{
    getRectangle(node, r.l);
    getRectangle(node->next, r.m);
    getRectangle(node->next->next, r.r);
}

void
Style::get9Rectangle(xmlNodePtr node, r9& r)
{
    getRectangle(node, r.tl);
    getRectangle(node->next, r.tm);
    getRectangle(node->next->next, r.tr);
    getRectangle(node->next->next->next, r.l);
    getRectangle(node->next->next->next->next, r.m);
    getRectangle(node->next->next->next->next->next, r.r);
    getRectangle(node->next->next->next->next->next->next, r.bl);
    getRectangle(node->next->next->next->next->next->next->next, r.bm);
    getRectangle(node->next->next->next->next->next->next->next->next, r.br);
}

} /* namespace ilixi */
