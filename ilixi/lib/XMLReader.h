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

#ifndef ILIXI_XMLREADER_H_
#define ILIXI_XMLREADER_H_

#include <libxml/tree.h>
#include <libxml/parser.h>
#include <string>

namespace ilixi
{
//! Basic wrapper class for the libxml parser.
class XMLReader
{
public:
    /*!
     * Constructor.
     */
    XMLReader();

    /*!
     * Constructor, loads given file with default parser options.
     *
     * @param file
     */
    XMLReader(const std::string& file);

    /*!
     * Destructor, releases any allocated parser resources.
     */
    virtual
    ~XMLReader();

    /*!
     * Loads given file with given parser options.
     *
     * @param file
     * @param parserOptions
     *
     * @return true on success.
     */
    bool
    loadFile(const std::string& file, int parserOptions = XML_PARSE_DTDATTR | XML_PARSE_NOENT | XML_PARSE_DTDVALID | XML_PARSE_NOBLANKS);

    /*!
     * Releases any allocated parser resources.
     */
    void
    release();

    /*!
     * Returns root element of document.
     */
    xmlNodePtr
    root();

    /*!
     * Returns current node.
     */
    xmlNodePtr
    currentNode();

    /*!
     * Reads the next node and sets current node.
     *
     * @return true if next node is available.
     */
    bool
    readNext();

private:
    //! current node
    xmlNodePtr _currentNode;

    //! libxml document
    xmlDocPtr _doc;

    //! libxml parser context
    xmlParserCtxtPtr _ctxt;
};

} /* namespace ilixi */
#endif /* ILIXI_XMLREADER_H_ */
