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
