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

#include "Keyboard.h"
#include <ui/Label.h>
#include <core/ComponentData.h>
#include <core/Logger.h>
#include <libxml/parser.h>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_KEYBOARD, "ilixi/osk/Keyboard", "Keyboard");

Keyboard::Keyboard(OSKHelper* helper, Widget* parent)
        : Widget(parent),
          _helper(helper),
          _buttonFont(NULL),
          _oskComponent(NULL),
          _modifier(NULL),
          _cycleKey(NULL)
{
    ILOG_TRACE_W(ILX_KEYBOARD);
    setConstraints(ExpandingConstraint, ExpandingConstraint);
    setInputMethod(PointerInput);
    sigGeometryUpdated.connect(sigc::mem_fun(this, &Keyboard::updateKeyboardGeometry));

    DaleDFB::comaGetComponent("OSK", &_oskComponent);

    _cycleTimer.sigExec.connect(sigc::mem_fun(this, &Keyboard::handleCycleTimer));
}

Keyboard::~Keyboard()
{
    ILOG_TRACE_W(ILX_KEYBOARD);
    delete _buttonFont;
    if (_oskComponent)
        _oskComponent->Release(_oskComponent);
}

void
Keyboard::toggleHelper()
{
    ILOG_TRACE_W(ILX_KEYBOARD);
    if (_helper->visible())
        _helper->setVisible(false);
    else
        _helper->setVisible(true);
}

void
Keyboard::setSymbolState(unsigned char state)
{
    ILOG_TRACE_W(ILX_KEYBOARD);
    ILOG_DEBUG(ILX_KEYBOARD, " -> State: %d\n", state);
    for (unsigned int i = 0; i < _rows.size(); ++i)
        _rows[i]->setSymbolState(state);
}

bool
Keyboard::parseLayoutFile(const char* file)
{
    ILOG_TRACE_W(ILX_KEYBOARD);
    ILOG_DEBUG(ILX_KEYBOARD, " -> Parsing layout file: %s\n", file);
    xmlParserCtxtPtr ctxt;
    xmlDocPtr doc;

    ctxt = xmlNewParserCtxt();
    if (ctxt == NULL)
    {
        ILOG_ERROR(ILX_KEYBOARD, "Failed to allocate parser context\n");
        return false;
    }

    doc = xmlCtxtReadFile(ctxt, file, NULL, XML_PARSE_DTDATTR | XML_PARSE_NOENT | XML_PARSE_DTDVALID | XML_PARSE_NOBLANKS);

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
Keyboard::forwardKeyData(const uint32_t& ucs32, unsigned int modifiers)
{
    ILOG_TRACE_W(ILX_KEYBOARD);
    ILOG_DEBUG(ILX_KEYBOARD, " -> U+%04X\n", ucs32);

    void *ptr;
    DaleDFB::comaGetLocal(sizeof(uint32_t) * 2, &ptr);
    uint32_t* key = (uint32_t*) ptr;
    key[0] = ucs32;
    key[1] = modifiers;

    DaleDFB::comaCallComponent(_oskComponent, OSK::ConsumeKey, (void*) key);

    if (_modifier && _modifier->symbolState() == 2 && !_cycleKey)
    {
        ILOG_DEBUG(ILX_KEYBOARD, " -> unset modifier\n");
        setSymbolState(_modifier->getNextState());
        _modifier = NULL;
    }
}

void
Keyboard::forwardKeyData(const std::vector<uint32_t>& ucs32, unsigned int modifiers, bool force)
{
    for (unsigned int i = 0; i < ucs32.size(); ++i)
    {
        if (!force && _helper->visible())
            _helper->handleInput(ucs32[i]);
        else
        {
            ILOG_TRACE_W(ILX_KEYBOARD);
            ILOG_DEBUG(ILX_KEYBOARD, " -> U+%04X\n", ucs32[i]);

            void *ptr;
            DaleDFB::comaGetLocal(sizeof(uint32_t) * 2, &ptr);
            uint32_t* key = (uint32_t*) ptr;
            key[0] = ucs32[i];
            key[1] = modifiers;

            DaleDFB::comaCallComponent(_oskComponent, OSK::ConsumeKey, (void*) key);
        }
    }

    if (_modifier && _modifier->symbolState() == 2 && !_cycleKey)
    {
        ILOG_DEBUG(ILX_KEYBOARD, " -> unset modifier\n");
        setSymbolState(_modifier->getNextState());
        _modifier = NULL;
    }
}

void
Keyboard::setModifier(Key* modifier)
{
    ILOG_TRACE_W(ILX_KEYBOARD);
    if (_cycleKey)
    {
        /* send cursor right to remove selection and continue with next letter */
        std::vector<uint32_t> c;
        c.push_back(DIKS_CURSOR_RIGHT);
        forwardKeyData(c, DIMM_SHIFT);

        _cycleKey = NULL;

        _cycleTimer.stop();
    }

    _modifier = modifier;

    ILOG_DEBUG(ILX_KEYBOARD, " -> modifier = %s\n", _modifier->xmlID().c_str());
}

void
Keyboard::handleCycleKey(Key* key)
{
    std::vector<uint32_t> ucs32 = key->_symbols[key->_keyState].ucs32;

    if (_cycleKey && _cycleKey == key)
    {
        uint32_t nextCharacter = _cycleCharacter;

        for (int i = 0; i < ucs32.size(); i++)
        {
            if (ucs32[i] == _cycleCharacter)
            {
                if (i == ucs32.size() - 1)
                    nextCharacter = ucs32[0];
                else
                    nextCharacter = ucs32[i + 1];

                break;
            }
        }

        _cycleCharacter = nextCharacter;
    } else
    {
        if (_cycleKey)
        {
            /* send cursor right to remove selection and continue with next letter */
            std::vector<uint32_t> c;
            c.push_back(DIKS_CURSOR_RIGHT);
            forwardKeyData(c, DIMM_SHIFT);
        }

        _cycleKey = key;
        _cycleCharacter = ucs32[0];
    }

    /* send new character replacing the old selection (if any) */
    std::vector<uint32_t> c;
    c.push_back(_cycleCharacter);
    forwardKeyData(c);

    _cycleTimer.stop();

    if (!(key->_keyMode & Key::Special))
    {
        /* send shift left to make new selection */
        std::vector<uint32_t> c2;
        c2.push_back(DIKS_CURSOR_LEFT);
        forwardKeyData(c2, DIMM_SHIFT);

        _cycleTimer.start(800);
    } else
        _cycleKey = NULL;
}

bool
Keyboard::handleKeyPress(uint32_t symbol)
{
    if (_helper->visible())
    {
        _helper->handleInput(symbol);
        return true;
    }

    std::map<uint32_t, Key*>::iterator it = _cycleMap.find(symbol);

    if (it != _cycleMap.end())
    {
        Key *key = (*it).second;
        key->click();
        return true;
    }

    return false;
}

void
Keyboard::compose(const PaintEvent& event)
{
}

Key*
Keyboard::getKey(xmlNodePtr node)
{
    ILOG_TRACE_W(ILX_KEYBOARD);
    xmlChar* helper = xmlGetProp(node, (xmlChar*) "helper");
#ifndef ILIXI_HAVE_LIBWNN
    if (xmlStrcmp(helper, (xmlChar*) "yes") == 0)
    {
        xmlFree(helper);
        return NULL;
    }
#endif

    xmlChar* id = xmlGetProp(node, (xmlChar*) "id");
    xmlChar* modifier = xmlGetProp(node, (xmlChar*) "modifier");
    xmlChar* constraint = xmlGetProp(node, (xmlChar*) "constraint");
    xmlChar* repeatable = xmlGetProp(node, (xmlChar*) "repeatable");
    xmlChar* sticky = xmlGetProp(node, (xmlChar*) "sticky");
    xmlChar* special = xmlGetProp(node, (xmlChar*) "special");
    xmlChar* cycle = xmlGetProp(node, (xmlChar*) "cycle");

    ILOG_DEBUG(ILX_KEYBOARD, "Key: %s\n", (char*) id);

    Key* key = new Key((char*) id, this);

    if (xmlStrcmp(helper, (xmlChar*) "yes") == 0)
        key->setKeyMode(Key::Helper);

    if (xmlStrcmp(modifier, (xmlChar*) "yes") == 0)
        key->setKeyMode(Key::Modifier);

    if (xmlStrcmp(constraint, (xmlChar*) "extend") == 0)
        key->setXConstraint(MinimumExpandingConstraint);
    else if (xmlStrcmp(constraint, (xmlChar*) "minimum") == 0)
        key->setXConstraint(MinimumConstraint);

    if (xmlStrcmp(repeatable, (xmlChar*) "yes") == 0)
        key->setRepeatable(true);

    if (xmlStrcmp(sticky, (xmlChar*) "yes") == 0)
        key->setKeyMode(Key::Sticky);

    if (xmlStrcmp(special, (xmlChar*) "yes") == 0)
        key->setKeyMode(Key::Special);

    if (xmlStrcmp(cycle, (xmlChar*) "yes") == 0)
        key->setKeyMode(Key::Cycle);

    xmlFree(helper);
    xmlFree(id);
    xmlFree(modifier);
    xmlFree(constraint);
    xmlFree(repeatable);
    xmlFree(cycle);

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
                iconPath = ILIXI_DATADIR;
                iconPath.append(iconStr.substr(found + 9, std::string::npos));
            } else
                iconPath = iconStr;
            ILOG_DEBUG(ILX_KEYBOARD, "ICON: %s\n", iconPath.c_str());

            key->setIcon(iconPath, Size(48, 48));
            key->icon()->setColorize(true);
            key->setToolButtonStyle(ToolButton::IconOnly);
        } else if (xmlStrcmp(element->name, (xmlChar*) "rollStates") == 0)
        {
            xmlChar* rollStates = xmlNodeGetContent(element->children);
            key->setRollStates((char*) rollStates);
            xmlFree(rollStates);
        }
        element = element->next;
    }

    if (key->_keyMode & Key::Cycle)
        _cycleMap.insert(std::pair<uint32_t, Key*>(key->_cycleUCS, key));

    return key;
}

Row*
Keyboard::getRow(xmlNodePtr node)
{
    ILOG_TRACE_W(ILX_KEYBOARD);
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

    ILOG_DEBUG( ILX_KEYBOARD, "Row: %s Gap: %u Height: %d\n", (char*) id, row->gap(), row->keyHeight());

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
    for (unsigned int i = 0; i < _rows.size(); ++i)
        removeChild(_rows[i]);
    _rows.clear();
    _cycleMap.clear();
}

void
Keyboard::updateKeyboardGeometry()
{
    ILOG_TRACE_W(ILX_KEYBOARD);
    if (!_buttonFont)
    {
        _buttonFont = new Font("Sans", width() > 800 ? 36 : 24);
        _buttonFont->setStyle(Font::Bold);
    }

    int y = 0;
    unsigned int h;
    for (unsigned int i = 0; i < _rows.size(); ++i)
    {
        _rows[i]->setKeyFont(_buttonFont);
        h = height() * _rows[i]->keyHeight() / 100;
        _rows[i]->setGeometry(_rows[i]->gap(), y, width() - 2 * _rows[i]->gap(), h);
        y += h;

        // set neighbours
        if (i == 0)
            _rows[0]->setNeighbours(_rows[_rows.size() - 1]->_box, _rows[1]->_box, _rows[_rows.size() - 1], _rows[1]);
        else if (i < _rows.size() - 1)
            _rows[i]->setNeighbours(_rows[i - 1]->_box, _rows[i + 1]->_box, _rows[i - 1], _rows[i + 1]);
        else
            _rows[i]->setNeighbours(_rows[i - 1]->_box, _rows[0]->_box, _rows[i - 1], _rows[0]);
    }
}

void
Keyboard::handleCycleTimer()
{
    if (_cycleKey)
    {
        /* send cursor right to remove selection and continue with next letter */
        std::vector<uint32_t> c;
        c.push_back(DIKS_CURSOR_RIGHT);
        forwardKeyData(c, DIMM_SHIFT);

        if (_modifier)
        {
            ILOG_DEBUG(ILX_KEYBOARD, " -> unset modifier\n");
            setSymbolState(_modifier->getNextState());
            _modifier = NULL;
        }

        _cycleKey = NULL;
    }
}

} /* namespace ilixi */
