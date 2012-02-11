/*
 Copyright 2010, 2011 Tarik Sekmen.

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

#include "graphics/Style.h"
#include "core/Logger.h"
#include <libxml/parser.h>
#include <libxml/tree.h>

using namespace ilixi;

Style::Style() :
  _buttonFont(NULL), _defaultFont(NULL), _inputFont(NULL), _titleFont(NULL),
      _plusSign(NULL), _minusSign(NULL), _grid(NULL), _arrowUp(NULL),
      _arrowDown(NULL), _arrowLeft(NULL), _arrowRight(NULL), _checkEmpty(NULL),
      _checkPartial(NULL), _checkFull(NULL), _critical(NULL), _info(NULL),
      _question(NULL), _warning(NULL)
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
  delete _plusSign;
  delete _minusSign;
  delete _grid;

  delete _arrowUp;
  delete _arrowDown;
  delete _arrowLeft;
  delete _arrowRight;

  delete _checkEmpty;
  delete _checkFull;
  delete _checkPartial;

  delete _info;
  delete _warning;
  delete _critical;
  delete _question;
}

bool
Style::parseStyle(const char* style)
{
  xmlParserCtxtPtr ctxt;
  xmlDocPtr doc;

  ctxt = xmlNewParserCtxt();
  if (ctxt == NULL)
    {
      ILOG_ERROR("Failed to allocate parser context");
      return false;
    }

  doc = xmlCtxtReadFile(
      ctxt,
      style,
      NULL,
      XML_PARSE_DTDATTR | XML_PARSE_NOENT | XML_PARSE_DTDVALID
          | XML_PARSE_NOBLANKS);

  if (doc == NULL)
    {
      xmlFreeParserCtxt(ctxt);
      ILOG_ERROR("Failed to parse style: %s", style);
      return false;
    }

  if (ctxt->valid == 0)
    {
      xmlFreeDoc(doc);
      xmlFreeParserCtxt(ctxt);
      ILOG_ERROR("Failed to validate style: %s", style);
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
          xmlChar* fPath = xmlGetProp(group, (xmlChar*) "path");
          std::string path = ILIXI_DATADIR"" + std::string((char*) fPath);

          while (element != NULL)
            {
              xmlChar* fileC = xmlNodeGetContent(element->children);
              xmlChar* sizeC = xmlNodeGetContent(element->children->next);
              xmlChar* styleC =
                  xmlNodeGetContent(element->children->next->next);

              Font::Style fontStyle = Font::Plain;
              if (styleC)
                {
                  if (xmlStrcmp(styleC, (xmlChar *) "italic") == 0)
                    fontStyle = Font::Italic;
                  else if (xmlStrcmp(styleC, (xmlChar *) "bold") == 0)
                    fontStyle = Font::Bold;
                }

              if (xmlStrcmp(element->name, (xmlChar*) "defaultFont") == 0)
                {
                  _defaultFont = new Font(path + (char*) fileC);
                  _defaultFont->setSize(atoi((char*) sizeC));
                  _defaultFont->setStyle(fontStyle);
                }
              else if (xmlStrcmp(element->name, (xmlChar*) "buttonFont") == 0)
                {
                  _buttonFont = new Font(path + (char*) fileC);
                  _buttonFont->setSize(atoi((char*) sizeC));
                  _buttonFont->setStyle(fontStyle);
                }

              else if (xmlStrcmp(element->name, (xmlChar*) "inputFont") == 0)
                {
                  _inputFont = new Font(path + (char*) fileC);
                  _inputFont->setSize(atoi((char*) sizeC));
                  _inputFont->setStyle(fontStyle);
                }

              else if (xmlStrcmp(element->name, (xmlChar*) "titleFont") == 0)
                {
                  _titleFont = new Font(path + (char*) fileC);
                  _titleFont->setSize(atoi((char*) sizeC));
                  _titleFont->setStyle(fontStyle);
                }

              xmlFree(fileC);
              xmlFree(sizeC);
              element = element->next;
            } // end while(element)
        } // end fonts


      else if (xmlStrcmp(group->name, (xmlChar*) "hints") == 0)
        {
          while (element != NULL)
            {
              if (xmlStrcmp(element->name, (xmlChar*) "button") == 0)
                {
                  xmlChar* width = xmlNodeGetContent(element->children);
                  xmlChar* height = xmlNodeGetContent(element->children->next);
                  xmlChar* radius = xmlNodeGetContent(
                      element->children->next->next);
                  xmlChar* offset = xmlNodeGetContent(
                      element->children->next->next->next);
                  xmlChar* indicator = xmlNodeGetContent(
                      element->children->next->next->next->next);

                  _buttonSize = Size(atoi((char*) width), atoi((char*) height));
                  _buttonRadius = atoi((char*) radius);
                  _buttonOffset = atoi((char*) offset);
                  _buttonIndicator = atoi((char*) indicator);

                  xmlFree(width);
                  xmlFree(height);
                  xmlFree(radius);
                  xmlFree(offset);
                  xmlFree(indicator);
                }

              else if (xmlStrcmp(element->name, (xmlChar*) "checkbox") == 0)
                {
                  xmlChar* width = xmlNodeGetContent(element->children);
                  xmlChar* height = xmlNodeGetContent(element->children->next);
                  xmlChar* radius = xmlNodeGetContent(
                      element->children->next->next);
                  xmlChar* offset = xmlNodeGetContent(
                      element->children->next->next->next);

                  _checkboxSize = Size(atoi((char*) width),
                      atoi((char*) height));
                  _checkboxRadius = atoi((char*) radius);
                  _checkboxOffset = atoi((char*) offset);

                  xmlFree(width);
                  xmlFree(height);
                  xmlFree(radius);
                  xmlFree(offset);
                }

              else if (xmlStrcmp(element->name, (xmlChar*) "combobox") == 0)
                {
                  xmlChar* width = xmlNodeGetContent(element->children);
                  xmlChar* radius = xmlNodeGetContent(element->children->next);

                  _comboboxRadius = atoi((char*) radius);
                  _comboboxButtonWidth = atoi((char*) width);

                  xmlFree(width);
                  xmlFree(radius);
                }

              else if (xmlStrcmp(element->name, (xmlChar*) "frame") == 0)
                {
                  xmlChar* radius = xmlNodeGetContent(element->children);

                  _frameRadius = atoi((char*) radius);

                  xmlFree(radius);
                }

              else if (xmlStrcmp(element->name, (xmlChar*) "icon") == 0)
                {
                  xmlChar* width = xmlNodeGetContent(element->children);
                  xmlChar* height = xmlNodeGetContent(element->children->next);

                  _iconSize = Size(atoi((char*) width), atoi((char*) height));

                  xmlFree(width);
                  xmlFree(height);
                }

              else if (xmlStrcmp(element->name, (xmlChar*) "progressbar") == 0)
                {
                  xmlChar* width = xmlNodeGetContent(element->children);
                  xmlChar* height = xmlNodeGetContent(element->children->next);
                  xmlChar* radius = xmlNodeGetContent(
                      element->children->next->next);

                  _progressbarSize = Size(atoi((char*) width),
                      atoi((char*) height));
                  _progressbarRadius = atoi((char*) radius);

                  xmlFree(width);
                  xmlFree(height);
                  xmlFree(radius);
                }

              else if (xmlStrcmp(element->name, (xmlChar*) "radiobutton") == 0)
                {
                  xmlChar* width = xmlNodeGetContent(element->children);
                  xmlChar* height = xmlNodeGetContent(element->children->next);
                  xmlChar* offset = xmlNodeGetContent(
                      element->children->next->next);

                  _radiobuttonSize = Size(atoi((char*) width),
                      atoi((char*) height));
                  _radiobuttonOffset = atoi((char*) offset);

                  xmlFree(width);
                  xmlFree(height);
                  xmlFree(offset);
                }

              else if (xmlStrcmp(element->name, (xmlChar*) "slider") == 0)
                {
                  xmlChar* width = xmlNodeGetContent(element->children);
                  xmlChar* height = xmlNodeGetContent(element->children->next);
                  xmlChar* radius = xmlNodeGetContent(
                      element->children->next->next);

                  _sliderSize = Size(atoi((char*) width), atoi((char*) height));
                  _sliderRadius = atoi((char*) radius);

                  xmlFree(width);
                  xmlFree(height);
                  xmlFree(radius);
                }

              else if (xmlStrcmp(element->name, (xmlChar*) "scrollbar") == 0)
                {
                  xmlChar* width = xmlNodeGetContent(element->children);
                  xmlChar* height = xmlNodeGetContent(element->children->next);
                  xmlChar* radius = xmlNodeGetContent(
                      element->children->next->next);

                  _scrollbarSize = Size(atoi((char*) width),
                      atoi((char*) height));
                  _scrollbarRadius = atoi((char*) radius);

                  xmlFree(width);
                  xmlFree(height);
                  xmlFree(radius);
                }

              else if (xmlStrcmp(element->name, (xmlChar*) "tabpanel") == 0)
                {
                  xmlChar* width = xmlNodeGetContent(element->children);
                  xmlChar* height = xmlNodeGetContent(element->children->next);
                  xmlChar* offset = xmlNodeGetContent(
                      element->children->next->next);

                  _tabPanelSize = Size(atoi((char*) width),
                      atoi((char*) height));

                  _tabPanelButtonOffset = atoi((char*) offset);

                  xmlFree(width);
                  xmlFree(height);
                  xmlFree(offset);
                }

              else if (xmlStrcmp(element->name, (xmlChar*) "tabbutton") == 0)
                {
                  xmlChar* width = xmlNodeGetContent(element->children);
                  xmlChar* height = xmlNodeGetContent(element->children->next);

                  _tabPanelButtonSize = Size(atoi((char*) width),
                      atoi((char*) height));

                  xmlFree(width);
                  xmlFree(height);
                }

              else if (xmlStrcmp(element->name, (xmlChar*) "textinput") == 0)
                {
                  xmlChar* radius = xmlNodeGetContent(element->children);

                  _textInputRadius = atoi((char*) radius);

                  xmlFree(radius);
                }

              else if (xmlStrcmp(element->name, (xmlChar*) "toolbar") == 0)
                {
                  xmlChar* height = xmlNodeGetContent(element->children);

                  _toolbarHeight = atoi((char*) height);

                  xmlFree(height);
                }

              element = element->next;
            } // end while(element)
        } // end hints

      else if (xmlStrcmp(group->name, (xmlChar*) "images") == 0)
        {
          xmlChar* iPath = xmlGetProp(group, (xmlChar*) "path");
          std::string path = ILIXI_DATADIR"" + std::string((char*) iPath);

          while (element != NULL)
            {
              xmlChar* img = xmlNodeGetContent(element);
              //              path.append(;


              if (xmlStrcmp(element->name, (xmlChar*) "plus") == 0)
                _plusSign = new Image(path + (char*) img, _iconSize);

              else if (xmlStrcmp(element->name, (xmlChar*) "minus") == 0)
                _minusSign = new Image(path + (char*) img, _iconSize);

              else if (xmlStrcmp(element->name, (xmlChar*) "grid") == 0)
                _grid = new Image(path + (char*) img, _iconSize);

              else if (xmlStrcmp(element->name, (xmlChar*) "critical") == 0)
                _critical = new Image(path + (char*) img, _iconSize);

              else if (xmlStrcmp(element->name, (xmlChar*) "information") == 0)
                _info = new Image(path + (char*) img, _iconSize);

              else if (xmlStrcmp(element->name, (xmlChar*) "question") == 0)
                _question = new Image(path + (char*) img, _iconSize);

              else if (xmlStrcmp(element->name, (xmlChar*) "warning") == 0)
                _warning = new Image(path + (char*) img, _iconSize);

              else if (xmlStrcmp(element->name, (xmlChar*) "up") == 0)
                _arrowUp = new Image(path + (char*) img, _iconSize);
              else if (xmlStrcmp(element->name, (xmlChar*) "down") == 0)
                _arrowDown = new Image(path + (char*) img, _iconSize);
              else if (xmlStrcmp(element->name, (xmlChar*) "left") == 0)
                _arrowLeft = new Image(path + (char*) img, _iconSize);
              else if (xmlStrcmp(element->name, (xmlChar*) "right") == 0)
                _arrowRight = new Image(path + (char*) img, _iconSize);

              else if (xmlStrcmp(element->name, (xmlChar*) "empty") == 0)
                _checkEmpty = new Image(path + (char*) img, _iconSize);
              else if (xmlStrcmp(element->name, (xmlChar*) "partial") == 0)
                _checkPartial = new Image(path + (char*) img, _iconSize);
              else if (xmlStrcmp(element->name, (xmlChar*) "full") == 0)
                _checkFull = new Image(path + (char*) img, _iconSize);

              xmlFree(img);
              element = element->next;
            } // end while(element)
          xmlFree(iPath);
        } // end images

      group = group->next;
    } // end while(group)

  xmlFreeDoc(doc);
  xmlFreeParserCtxt(ctxt);
  ILOG_INFO("Parsed style file: %s", style);
  return true;
}
