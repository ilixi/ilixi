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
}

Keyboard::~Keyboard()
{
    ILOG_TRACE_W(ILX_KEYBOARD);
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
            _rows.push_back(getRow(group));

        group = group->next;
    } // end while(group)

    xmlFreeDoc(doc);
    xmlFreeParserCtxt(ctxt);
    ILOG_INFO(ILX_KEYBOARD, "Parsed layout file: %s\n", file);
    return true;
}

void
Keyboard::compose(const PaintEvent& event)
{
}

Key*
Keyboard::getKey(xmlNodePtr node)
{
    Key* key = NULL;
    return key;
}

Row*
Keyboard::getRow(xmlNodePtr node)
{
    Row* row = NULL;
    xmlNodePtr key = node->children;
    while (key != NULL)
    {
        xmlChar* id = xmlGetProp(key, (xmlChar*) "id");
        ILOG_DEBUG(ILX_KEYBOARD, " node: %s\n", (char*) id);
        key = key->next;
        xmlFree(id);
    }

    return row;
}

void
Keyboard::release()
{
}

} /* namespace ilixi */
