/*
 Copyright 2011 Tarik Sekmen.

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

#include "graphics/Palette.h"
#include "core/Logger.h"
#include <libxml/parser.h>
#include <libxml/tree.h>

using namespace ilixi;

ColorGroup::ColorGroup() :
    base(1, 1, 1), baseText(0, 0, 0), baseAlt(0.9, 0.9, 0.9), baseAltText(0, 0,
        0), bgBottom(0.28, 0.28, 0.28), bgMid(0, 0, 0), bgTop(0.43, 0.43, 0.43), borderBottom(
        0, 0, 0), borderMid(0, 0, 0), borderTop(0.3, 0.3, 0.3), fillBottom(0, 0,
        0), fillMid(0, 0, 0), fillTop(0, 0, 1), text(1, 1, 1)
{
}

ColorGroup::ColorGroup(const ColorGroup& c) :
    base(c.base), baseText(c.baseText), baseAlt(c.baseAlt), baseAltText(
        c.baseAltText), bgBottom(c.bgBottom), bgMid(c.bgMid), bgTop(c.bgTop), borderBottom(
        c.borderBottom), borderMid(c.borderMid), borderTop(c.borderTop), fillBottom(
        c.fillBottom), fillMid(c.fillMid), fillTop(c.fillTop), text(c.text)
{
}

ColorGroup::ColorGroup(Color b, Color bAlt, Color bTop, Color bMid,
    Color bBottom, Color boTop, Color boMid, Color boBottom, Color t) :
    base(b), baseAlt(bAlt), bgTop(bTop), bgMid(bMid), bgBottom(bBottom), borderTop(
        boTop), borderMid(boMid), borderBottom(boBottom), text(t)
{
}

void
ColorGroup::setColors(Color b, Color bAlt, Color bTop, Color bMid,
    Color bBottom, Color boTop, Color boMid, Color boBottom, Color t)
{
  base = b;
  baseAlt = bAlt;
  bgTop = bTop;
  bgMid = bMid;
  bgBottom = bBottom;
  borderTop = boTop;
  borderMid = boMid;
  borderBottom = boBottom;
  text = t;
}

Palette::Palette() :
    bgBottom(0, 0, 0), bgMid(0, 0, 0), bgTop(0, 0, 0), focusBottom(0, 0, 0), focusMid(
        0, 0, 0), focusTop(1, 0, 0), text(1, 1, 1), textDisabled(.9, .9, .9), _default(), _exposed(), _pressed(), _disabled()
{
}

ColorGroup&
Palette::getGroup(WidgetState state)
{
  if (state & DisabledState)
    return _disabled;
  else if (state & PressedState)
    return _pressed;
  else if (state & ExposedState)
    return _exposed;
  else
    return _default;
}

bool
Palette::parsePalette(const char* palette)
{
  xmlParserCtxtPtr ctxt;
  xmlDocPtr doc;

  ctxt = xmlNewParserCtxt();
  if (ctxt == NULL)
    {
      ILOG_ERROR(ILX_PALETTE, "Failed to allocate parser context\n");
      return false;
    }

  doc = xmlCtxtReadFile(ctxt, palette, NULL,
      XML_PARSE_DTDATTR | XML_PARSE_NOENT | XML_PARSE_DTDVALID
          | XML_PARSE_NOBLANKS);

  if (doc == NULL)
    {
      xmlFreeParserCtxt(ctxt);
      ILOG_ERROR(ILX_PALETTE, "Failed to parse palette: %s\n", palette);
      return false;
    }

  if (ctxt->valid == 0)
    {
      xmlFreeDoc(doc);
      xmlFreeParserCtxt(ctxt);
      ILOG_ERROR(ILX_PALETTE, "Failed to validate palette: %s\n", palette);
      return false;
    }

  xmlNodePtr root = xmlDocGetRootElement(doc);
  xmlNodePtr group = root->xmlChildrenNode;
  xmlNodePtr element, colorNode, hexNode;
  Color c;

  while (group != NULL)
    {
      element = group->children;
      while (element != NULL)
        {
          colorNode = element->children;
          xmlChar* alpha = xmlGetProp(colorNode, (xmlChar*) "alpha");

          hexNode = colorNode->children;
          if (xmlStrcmp(hexNode->name, (xmlChar*) "hex") == 0)
            {
              xmlChar* hex = xmlNodeGetContent(hexNode);
              c.setFromHex((char*) hex, atof((char*) alpha));
              xmlFree(hex);
            }
          else
            {
              xmlChar* r = xmlNodeGetContent(hexNode);
              xmlChar* g = xmlNodeGetContent(hexNode->next);
              xmlChar* b = xmlNodeGetContent(hexNode->next->next);
              c.setRGBA(atoi((char*) r), atoi((char*) g), atoi((char*) b),
                  atof((char*) alpha));
              xmlFree(r);
              xmlFree(g);
              xmlFree(b);
            }

          // check group name
          if (xmlStrcmp(group->name, (xmlChar*) "application") == 0)
            {
              if (xmlStrcmp(element->name, (xmlChar*) "bgBottom") == 0)
                bgBottom = c;
              else if (xmlStrcmp(element->name, (xmlChar*) "bgMid") == 0)
                bgMid = c;
              else if (xmlStrcmp(element->name, (xmlChar*) "bgTop") == 0)
                bgTop = c;

              else if (xmlStrcmp(element->name, (xmlChar*) "focusBottom") == 0)
                focusBottom = c;
              else if (xmlStrcmp(element->name, (xmlChar*) "focusMid") == 0)
                focusMid = c;
              else if (xmlStrcmp(element->name, (xmlChar*) "focusTop") == 0)
                focusTop = c;

              else if (xmlStrcmp(element->name, (xmlChar*) "text") == 0)
                text = c;
              else if (xmlStrcmp(element->name, (xmlChar*) "textDisabled") == 0)
                textDisabled = c;
            }

          else if (xmlStrcmp(group->name, (xmlChar*) "default") == 0)
            {
              if (xmlStrcmp(element->name, (xmlChar*) "base") == 0)
                _default.base = c;
              else if (xmlStrcmp(element->name, (xmlChar*) "baseText") == 0)
                _default.baseText = c;
              else if (xmlStrcmp(element->name, (xmlChar*) "baseAlt") == 0)
                _default.baseAlt = c;
              else if (xmlStrcmp(element->name, (xmlChar*) "baseAltText") == 0)
                _default.baseAltText = c;

              else if (xmlStrcmp(element->name, (xmlChar*) "bgBottom") == 0)
                _default.bgBottom = c;
              else if (xmlStrcmp(element->name, (xmlChar*) "bgMid") == 0)
                _default.bgMid = c;
              else if (xmlStrcmp(element->name, (xmlChar*) "bgTop") == 0)
                _default.bgTop = c;

              else if (xmlStrcmp(element->name, (xmlChar*) "borderBottom") == 0)
                _default.borderBottom = c;
              else if (xmlStrcmp(element->name, (xmlChar*) "borderMid") == 0)
                _default.borderMid = c;
              else if (xmlStrcmp(element->name, (xmlChar*) "borderTop") == 0)
                _default.borderTop = c;

              else if (xmlStrcmp(element->name, (xmlChar*) "fillBottom") == 0)
                _default.fillBottom = c;
              else if (xmlStrcmp(element->name, (xmlChar*) "fillMid") == 0)
                _default.fillMid = c;
              else if (xmlStrcmp(element->name, (xmlChar*) "fillTop") == 0)
                _default.fillTop = c;

              else if (xmlStrcmp(element->name, (xmlChar*) "text") == 0)
                _default.text = c;
            }

          else if (xmlStrcmp(group->name, (xmlChar*) "disabled") == 0)
            {
              if (xmlStrcmp(element->name, (xmlChar*) "base") == 0)
                _disabled.base = c;
              else if (xmlStrcmp(element->name, (xmlChar*) "baseText") == 0)
                _disabled.baseText = c;
              else if (xmlStrcmp(element->name, (xmlChar*) "baseAlt") == 0)
                _disabled.baseAlt = c;
              else if (xmlStrcmp(element->name, (xmlChar*) "baseAltText") == 0)
                _disabled.baseAltText = c;

              else if (xmlStrcmp(element->name, (xmlChar*) "bgBottom") == 0)
                _disabled.bgBottom = c;
              else if (xmlStrcmp(element->name, (xmlChar*) "bgMid") == 0)
                _disabled.bgMid = c;
              else if (xmlStrcmp(element->name, (xmlChar*) "bgTop") == 0)
                _disabled.bgTop = c;

              else if (xmlStrcmp(element->name, (xmlChar*) "borderBottom") == 0)
                _disabled.borderBottom = c;
              else if (xmlStrcmp(element->name, (xmlChar*) "borderMid") == 0)
                _disabled.borderMid = c;
              else if (xmlStrcmp(element->name, (xmlChar*) "borderTop") == 0)
                _disabled.borderTop = c;

              else if (xmlStrcmp(element->name, (xmlChar*) "fillBottom") == 0)
                _disabled.fillBottom = c;
              else if (xmlStrcmp(element->name, (xmlChar*) "fillMid") == 0)
                _disabled.fillMid = c;
              else if (xmlStrcmp(element->name, (xmlChar*) "fillTop") == 0)
                _disabled.fillTop = c;

              else if (xmlStrcmp(element->name, (xmlChar*) "text") == 0)
                _disabled.text = c;
            }

          else if (xmlStrcmp(group->name, (xmlChar*) "exposed") == 0)
            {
              if (xmlStrcmp(element->name, (xmlChar*) "base") == 0)
                _exposed.base = c;
              else if (xmlStrcmp(element->name, (xmlChar*) "baseText") == 0)
                _exposed.baseText = c;
              else if (xmlStrcmp(element->name, (xmlChar*) "baseAlt") == 0)
                _exposed.baseAlt = c;
              else if (xmlStrcmp(element->name, (xmlChar*) "baseAltText") == 0)
                _exposed.baseAltText = c;

              else if (xmlStrcmp(element->name, (xmlChar*) "bgBottom") == 0)
                _exposed.bgBottom = c;
              else if (xmlStrcmp(element->name, (xmlChar*) "bgMid") == 0)
                _exposed.bgMid = c;
              else if (xmlStrcmp(element->name, (xmlChar*) "bgTop") == 0)
                _exposed.bgTop = c;

              else if (xmlStrcmp(element->name, (xmlChar*) "borderBottom") == 0)
                _exposed.borderBottom = c;
              else if (xmlStrcmp(element->name, (xmlChar*) "borderMid") == 0)
                _exposed.borderMid = c;
              else if (xmlStrcmp(element->name, (xmlChar*) "borderTop") == 0)
                _exposed.borderTop = c;

              else if (xmlStrcmp(element->name, (xmlChar*) "fillBottom") == 0)
                _exposed.fillBottom = c;
              else if (xmlStrcmp(element->name, (xmlChar*) "fillMid") == 0)
                _exposed.fillMid = c;
              else if (xmlStrcmp(element->name, (xmlChar*) "fillTop") == 0)
                _exposed.fillTop = c;

              else if (xmlStrcmp(element->name, (xmlChar*) "text") == 0)
                _exposed.text = c;
            }

          else if (xmlStrcmp(group->name, (xmlChar*) "pressed") == 0)
            {
              if (xmlStrcmp(element->name, (xmlChar*) "base") == 0)
                _pressed.base = c;
              else if (xmlStrcmp(element->name, (xmlChar*) "baseText") == 0)
                _pressed.baseText = c;
              else if (xmlStrcmp(element->name, (xmlChar*) "baseAlt") == 0)
                _pressed.baseAlt = c;
              else if (xmlStrcmp(element->name, (xmlChar*) "baseAltText") == 0)
                _pressed.baseAltText = c;

              else if (xmlStrcmp(element->name, (xmlChar*) "bgBottom") == 0)
                _pressed.bgBottom = c;
              else if (xmlStrcmp(element->name, (xmlChar*) "bgMid") == 0)
                _pressed.bgMid = c;
              else if (xmlStrcmp(element->name, (xmlChar*) "bgTop") == 0)
                _pressed.bgTop = c;

              else if (xmlStrcmp(element->name, (xmlChar*) "borderBottom") == 0)
                _pressed.borderBottom = c;
              else if (xmlStrcmp(element->name, (xmlChar*) "borderMid") == 0)
                _pressed.borderMid = c;
              else if (xmlStrcmp(element->name, (xmlChar*) "borderTop") == 0)
                _pressed.borderTop = c;

              else if (xmlStrcmp(element->name, (xmlChar*) "fillBottom") == 0)
                _pressed.fillBottom = c;
              else if (xmlStrcmp(element->name, (xmlChar*) "fillMid") == 0)
                _pressed.fillMid = c;
              else if (xmlStrcmp(element->name, (xmlChar*) "fillTop") == 0)
                _pressed.fillTop = c;

              else if (xmlStrcmp(element->name, (xmlChar*) "text") == 0)
                _pressed.text = c;
            }

          element = element->next;
          xmlFree(alpha);
        }
      group = group->next;
    }

  xmlFreeDoc(doc);
  xmlFreeParserCtxt(ctxt);
  ILOG_INFO(ILX_PALETTE, "Parsed palette file: %s\n", palette);
  return true;
}

