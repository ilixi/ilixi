/*
 * Keyboard.cpp
 *
 *  Created on: Aug 6, 2012
 *      Author: tarik
 */

#include "Keyboard.h"
#include <core/Logger.h>
#include <libxml/parser.h>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_KEYBOARD, "ilixi/osk/Keyboard", "Keyboard");

Keyboard::Keyboard(Widget* parent)
        : Widget(parent)
{
    ILOG_TRACE_W(ILX_KEYBOARD);
    setInputMethod(PointerInput);
    sigGeometryUpdated.connect(
            sigc::mem_fun(this, &Keyboard::updateKeyboardGeometry));
}

Keyboard::~Keyboard()
{
    ILOG_TRACE_W(ILX_KEYBOARD);
}

void
Keyboard::setSymbolState(unsigned char state)
{
    ILOG_DEBUG(ILX_KEYBOARD, "State: %d\n", state);
    for (unsigned int i = 0; i < _rows.size(); ++i)
        _rows[i]->setSymbolState(state);
}

bool
Keyboard::parseLayoutFile(const char* file)
{
    ILOG_DEBUG(ILX_KEYBOARD, "Parsing layout file: %s\n", file);
    xmlParserCtxtPtr ctxt;
    xmlDocPtr doc;

    ctxt = xmlNewParserCtxt();
    if (ctxt == NULL)
    {
        ILOG_ERROR(ILX_KEYBOARD, "Failed to allocate parser context\n");
        return false;
    }

    doc = xmlCtxtReadFile(
            ctxt,
            file,
            NULL,
            XML_PARSE_DTDATTR | XML_PARSE_NOENT | XML_PARSE_DTDVALID
                    | XML_PARSE_NOBLANKS);

    if (doc == NULL)
    {
        xmlFreeParserCtxt(ctxt);
        ILOG_ERROR(ILX_KEYBOARD, "Failed to parse layout: %s\n", file);
        return false;
    }

    if (ctxt->valid == 0)
    {
        xmlFreeDoc(doc);
        xmlFreeParserCtxt(ctxt);
        ILOG_ERROR(ILX_KEYBOARD, "Failed to validate layout: %s\n", file);
        return false;
    }

    release();

    xmlNodePtr root = xmlDocGetRootElement(doc);
    xmlNodePtr group = root->xmlChildrenNode;

    while (group != NULL)
    {
        if (xmlStrcmp(group->name, (xmlChar*) "row") == 0)
        {
            Row* row = getRow(group);
            _rows.push_back(row);
            addChild(row);
        }

        group = group->next;
    } // end while(group)

    xmlFreeDoc(doc);
    xmlFreeParserCtxt(ctxt);
    ILOG_INFO(ILX_KEYBOARD, "Parsed layout file: %s\n", file);
    setSymbolState(1);
    return true;
}

void
Keyboard::compose(const PaintEvent& event)
{
}

Key*
Keyboard::getKey(xmlNodePtr node)
{
    xmlChar* id = xmlGetProp(node, (xmlChar*) "id");
    xmlChar* modifier = xmlGetProp(node, (xmlChar*) "modifier");
    xmlChar* constraint = xmlGetProp(node, (xmlChar*) "constraint");
    xmlChar* repeatable = xmlGetProp(node, (xmlChar*) "repeatable");
    xmlChar* sticky = xmlGetProp(node, (xmlChar*) "sticky");

    ILOG_DEBUG(ILX_KEYBOARD, "Key: %s\n", (char*) id);

    Key* key = new Key((char*) id, this);

    if (xmlStrcmp(modifier, (xmlChar*) "yes") == 0)
        key->setKeyMode(Key::Modifier);

    if (xmlStrcmp(constraint, (xmlChar*) "expand") == 0)
        key->setXConstraint(ExpandingConstraint);

    if (xmlStrcmp(repeatable, (xmlChar*) "yes") == 0)
        key->setKeyMode(Key::Repeatable);

    if (xmlStrcmp(sticky, (xmlChar*) "yes") == 0)
        key->setKeyMode(Key::Sticky);

    xmlFree(id);
    xmlFree(modifier);
    xmlFree(constraint);
    xmlFree(repeatable);

    xmlNodePtr element = node->children;
    while (element != NULL)
    {
        if (xmlStrcmp(element->name, (xmlChar*) "symbol") == 0)
        {
            xmlChar* states = xmlGetProp(element, (xmlChar*) "states");
            xmlChar* symbol = xmlNodeGetContent(element->children);
            key->addSymbol((char*) states, (char*) symbol);
            xmlFree(states);
            xmlFree(symbol);
        } else if (xmlStrcmp(element->name, (xmlChar*) "icon") == 0)
        {
            std::string iconPath;
            xmlChar* icon = xmlNodeGetContent(element->children);
            std::string iconStr = (char*) icon;
            xmlFree(icon);

            size_t found = iconStr.find("@DATADIR:");
            if (found != std::string::npos)
            {
                iconPath = ILIXI_IMAGEDIR;
                iconPath.append(iconStr.substr(found + 9, std::string::npos));
            } else
                iconPath = iconStr;
            ILOG_DEBUG(ILX_KEYBOARD, "ICON: %s\n", iconPath.c_str());

            key->setIcon(iconPath, Size(48,48));
            key->setToolButtonStyle(ToolButton::IconOnly);
        } else if (xmlStrcmp(element->name, (xmlChar*) "rollStates") == 0)
        {
            xmlChar* rollStates = xmlNodeGetContent(element->children);
            key->setRollStates((char*) rollStates);
            xmlFree(rollStates);
        }
        // TODO icon etc..
        element = element->next;
    }

    return key;
}

Row*
Keyboard::getRow(xmlNodePtr node)
{
    xmlChar* id = xmlGetProp(node, (xmlChar*) "id");
    xmlChar* gap = xmlGetProp(node, (xmlChar*) "gap");
    xmlChar* height = xmlGetProp(node, (xmlChar*) "height");

    Row* row = new Row((char*) id);
    if (gap)
        row->setGap(atoi((char*) gap));
    if (height)
    {
        char* h = (char*) height;
        h++;
        row->setKeyHeight(atoi((char*) h));
    }

    ILOG_DEBUG(
            ILX_KEYBOARD,
            "Row: %s Gap: %u Height: %d\n", (char*) id, row->gap(), row->keyHeight());

    xmlFree(id);
    xmlFree(gap);
    xmlFree(height);

    xmlNodePtr key = node->children;
    while (key != NULL)
    {
        row->addKey(getKey(key));
        key = key->next;
    }

    return row;
}

void
Keyboard::release()
{
}

void
Keyboard::updateKeyboardGeometry()
{
    // TODO calculate row heights.

    int y = 0;
    unsigned int h;
    for (unsigned int i = 0; i < _rows.size(); ++i)
    {
        h = height() * _rows[i]->keyHeight() / 100;
        _rows[i]->setGeometry(_rows[i]->gap(), y, width() - 2 * _rows[i]->gap(),
                              h);
        y += h;
    }
}

} /* namespace ilixi */
