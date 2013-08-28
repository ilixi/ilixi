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

#include <lib/XMLReader.h>
#include <core/Logger.h>

namespace ilixi
{

D_DEBUG_DOMAIN(ILX_XMLREADER, "ilixi/lib/XMLReader", "XMLReader");

XMLReader::XMLReader()
        : _currentNode(NULL),
          _doc(NULL),
          _ctxt(NULL)
{
    ILOG_TRACE_F(ILX_XMLREADER);
}

XMLReader::XMLReader(const std::string& file)
        : _currentNode(NULL),
          _doc(NULL),
          _ctxt(NULL)
{
    ILOG_TRACE_F(ILX_XMLREADER);
    loadFile(file);
}

XMLReader::~XMLReader()
{
    ILOG_TRACE_F(ILX_XMLREADER);
    release();
}

bool
XMLReader::loadFile(const std::string& file, int parserOptions)
{
    ILOG_TRACE_F(ILX_XMLREADER);
    release();
    ILOG_DEBUG(ILX_XMLREADER, " -> file: %s\n", file.c_str());
    _ctxt = xmlNewParserCtxt();
    if (_ctxt == NULL)
    {
        ILOG_ERROR(ILX_XMLREADER, "Failed to allocate parser context!\n");
        return false;
    }
#ifdef ANDROID_NDK
    _doc = xmlCtxtReadFile(_ctxt, file.c_str(), NULL, XML_PARSE_RECOVER | XML_PARSE_NOENT | XML_PARSE_NOBLANKS);
#else
    _doc = xmlCtxtReadFile(_ctxt, file.c_str(), NULL, XML_PARSE_DTDATTR | XML_PARSE_NOENT | XML_PARSE_DTDVALID | XML_PARSE_NOBLANKS);
#endif
    if (_doc == NULL)
    {
        xmlFreeParserCtxt(_ctxt);
        _ctxt = NULL;
        ILOG_ERROR(ILX_XMLREADER, "Failed to parse %s!\n", file.c_str());
        return false;
    }

    if ((parserOptions & XML_PARSE_DTDVALID) && (_ctxt->valid == 0))
    {
        xmlFreeDoc(_doc);
        xmlFreeParserCtxt(_ctxt);
        _doc = NULL;
        _ctxt = NULL;
        ILOG_ERROR(ILX_XMLREADER, "Failed to validate %s!\n", file.c_str());
        return false;
    }

    _currentNode = root()->xmlChildrenNode;
    ILOG_DEBUG(ILX_XMLREADER, " -> done.\n");
    return true;
}

void
XMLReader::release()
{
    ILOG_TRACE_F(ILX_XMLREADER);
    if (_doc)
        xmlFreeDoc(_doc);
    if (_ctxt)
        xmlFreeParserCtxt(_ctxt);

    _currentNode = NULL;
    _doc = NULL;
    _ctxt = NULL;
}

xmlNodePtr
XMLReader::root()
{
    if (_doc != NULL)
        return xmlDocGetRootElement(_doc);
    return NULL;
}

xmlNodePtr
XMLReader::currentNode()
{
    return _currentNode;
}

bool
XMLReader::readNext()
{
    if (_currentNode)
    {
        _currentNode = _currentNode->next;
        return true;
    }
    return false;
}

} /* namespace ilixi */
