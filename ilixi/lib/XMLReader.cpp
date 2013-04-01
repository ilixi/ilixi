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
}

XMLReader::XMLReader(const std::string& file)
        : _currentNode(NULL),
          _doc(NULL),
          _ctxt(NULL)
{
    loadFile(file);
}

XMLReader::~XMLReader()
{
    release();
}

bool
XMLReader::loadFile(const std::string& file, int parserOptions)
{
    release();
    _ctxt = xmlNewParserCtxt();
    if (_ctxt == NULL)
    {
        ILOG_ERROR(ILX_XMLREADER, "Failed to allocate parser context!\n");
        return false;
    }

    _doc = xmlCtxtReadFile(_ctxt, ILIXI_DATADIR"compositor.xml", NULL, XML_PARSE_DTDATTR | XML_PARSE_NOENT | XML_PARSE_DTDVALID | XML_PARSE_NOBLANKS);

    if (_doc == NULL)
    {
        xmlFreeParserCtxt(_ctxt);
        _ctxt = NULL;
        ILOG_ERROR(ILX_XMLREADER, "Failed to parse %s!\n", file.c_str());
        return false;
    }

    if (_ctxt->valid == 0)
    {
        xmlFreeDoc(_doc);
        xmlFreeParserCtxt(_ctxt);
        _doc = NULL;
        _ctxt = NULL;
        ILOG_ERROR(ILX_XMLREADER, "Failed to validate %s!\n", file.c_str());
        return false;
    }

    _currentNode = root()->xmlChildrenNode;

    return true;
}

void
XMLReader::release()
{
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
