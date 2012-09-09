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
          _icons(NULL),
          _pack(NULL)
{
}

Style::~Style()
{
    release();
}

void
Style::release()
{
    // clear fonts
    delete _buttonFont;
    delete _defaultFont;
    delete _inputFont;
    delete _titleFont;

    // clear images
    delete _icons;
    delete _pack;
}

bool
Style::parseStyle(const char* style)
{
    ILOG_DEBUG(ILX_STYLE, "Parsing style file: %s\n", style);
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
    xmlNodePtr element;

    while (group != NULL)
    {
        element = group->children;
        if (xmlStrcmp(group->name, (xmlChar*) "fonts") == 0)
        {
            while (element != NULL)
            {
                xmlChar* fileC = xmlNodeGetContent(element->children);
                xmlChar* sizeC = xmlNodeGetContent(element->children->next);
                xmlChar* styleC = xmlNodeGetContent(
                        element->children->next->next);

                Font::Style fontStyle = Font::Plain;
                if (styleC)
                {
                    if (xmlStrcmp(styleC, (xmlChar *) "italic") == 0)
                        fontStyle = Font::Italic;
                    else if (xmlStrcmp(styleC, (xmlChar *) "bold") == 0)
                        fontStyle = Font::Bold;
                }

                ILOG_DEBUG(ILX_STYLE, " Parsing %s...\n", element->name);

                if (xmlStrcmp(element->name, (xmlChar*) "defaultFont") == 0)
                {
                    _defaultFont = new Font((char*) fileC, atoi((char*) sizeC));
                    _defaultFont->setStyle(fontStyle);
                } else if (xmlStrcmp(element->name, (xmlChar*) "buttonFont") == 0)
                {
                    _buttonFont = new Font((char*) fileC, atoi((char*) sizeC));
                    _buttonFont->setStyle(fontStyle);
                }

                else if (xmlStrcmp(element->name, (xmlChar*) "inputFont") == 0)
                {
                    _inputFont = new Font((char*) fileC, atoi((char*) sizeC));
                    _inputFont->setStyle(fontStyle);
                }

                else if (xmlStrcmp(element->name, (xmlChar*) "titleFont") == 0)
                {
                    _titleFont = new Font((char*) fileC, atoi((char*) sizeC));
                    _titleFont->setStyle(fontStyle);
                }

                xmlFree(fileC);
                xmlFree(sizeC);
                element = element->next;
            } // end while(element)
            ILOG_DEBUG(ILX_STYLE, "Parsed fonts.\n");
        } // end fonts

        else if (xmlStrcmp(group->name, (xmlChar*) "icons") == 0)
        {
            ILOG_DEBUG(ILX_STYLE, "Parsing icons...\n");
            xmlChar* iPath = xmlGetProp(group, (xmlChar*) "resource");
            std::string path = ILIXI_DATADIR"" + std::string((char*) iPath);

            _icons = new Image(path);
            while (element != NULL)
            {
                ILOG_DEBUG(ILX_STYLE, " Parsing %s...\n", element->name);
                if (xmlStrcmp(element->name, (xmlChar*) "plus") == 0)
                    getRectangle(element, plus);

                else if (xmlStrcmp(element->name, (xmlChar*) "minus") == 0)
                    getRectangle(element, minus);

                element = element->next;
            } // end while(element)

            xmlFree(iPath);
        } // end images

        else if (xmlStrcmp(group->name, (xmlChar*) "pack") == 0)
        {
            ILOG_DEBUG(ILX_STYLE, "Parsed pack...\n");
            xmlChar* iPath = xmlGetProp(group, (xmlChar*) "resource");
            std::string path = ILIXI_DATADIR"" + std::string((char*) iPath);

            _pack = new Image(path);

            xmlNodePtr state;
            while (element != NULL)
            {
                ILOG_DEBUG(ILX_STYLE, " Parsing %s...\n", element->name);
                if (xmlStrcmp(element->name, (xmlChar*) "PushButton") == 0)
                {
                    state = element->children;
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
                } else if (xmlStrcmp(element->name, (xmlChar*) "CheckBox") == 0)
                {
                    state = element->children;
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
                } else if (xmlStrcmp(element->name, (xmlChar*) "RadioButton") == 0)
                {
                    state = element->children;
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

                else if (xmlStrcmp(element->name, (xmlChar*) "ProgressBar") == 0)
                {
                    state = element->children;
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

                                else if (xmlStrcmp(state->name,
                                                   (xmlChar*) "dis3") == 0)
                                    get3Rectangle(iState->children, prI.dis);

                                iState = iState->next;
                            }
                        }

                        state = state->next;
                    }
                }

                else if (xmlStrcmp(element->name, (xmlChar*) "Slider") == 0)
                {
                    state = element->children;
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

                                else if (xmlStrcmp(iState->name,
                                                   (xmlChar*) "pre1") == 0)
                                    getRectangle(iState->children, slI.pre);
                                else if (xmlStrcmp(iState->name,
                                                   (xmlChar*) "exp1") == 0)
                                    getRectangle(iState->children, slI.exp);

                                else if (xmlStrcmp(iState->name,
                                                   (xmlChar*) "dis1") == 0)
                                    getRectangle(iState->children, slI.dis);

                                else if (xmlStrcmp(iState->name,
                                                   (xmlChar*) "foc1") == 0)
                                    getRectangle(iState->children, slI.foc);

                                iState = iState->next;
                            }
                        }

                        state = state->next;
                    }
                } else if (xmlStrcmp(element->name, (xmlChar*) "Frame") == 0)
                {
                    state = element->children;
                    while (state != NULL)
                    {
                        ILOG_DEBUG(ILX_STYLE, "  state: %s...\n", state->name);

                        if (xmlStrcmp(state->name, (xmlChar*) "def9") == 0)
                            get9Rectangle(state->children, fr.def);

                        else if (xmlStrcmp(state->name, (xmlChar*) "dis9") == 0)
                            get9Rectangle(state->children, fr.dis);

                        state = state->next;
                    }
                } else if (xmlStrcmp(element->name, (xmlChar*) "LineInput") == 0)
                {
                    state = element->children;
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
                } else if (xmlStrcmp(element->name, (xmlChar*) "Tab") == 0)
                {
                    state = element->children;
                    while (state != NULL)
                    {
                        ILOG_DEBUG(ILX_STYLE, "  state: %s...\n", state->name);

                        if (xmlStrcmp(state->name, (xmlChar*) "def9") == 0)
                            get9Rectangle(state->children, tab.def);

                        else if (xmlStrcmp(state->name, (xmlChar*) "dis9") == 0)
                            get9Rectangle(state->children, tab.dis);

                        state = state->next;
                    }
                } else if (xmlStrcmp(element->name, (xmlChar*) "ToolButton") == 0)
                {
                    state = element->children;
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

                element = element->next;
            } // end while(element)

            xmlFree(iPath);
        } // end images

        group = group->next;
    } // end while(group)

    xmlFreeDoc(doc);
    xmlFreeParserCtxt(ctxt);
    ILOG_INFO(ILX_STYLE, "Parsed style file: %s\n", style);
    return true;
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
