/*
 Copyright 2010-2014 Tarik Sekmen.

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

#include <graphics/StyleUtil.h>

namespace ilixi
{

void
getCell(xmlNodePtr node, Rectangle& r)
{
    xmlChar* x = xmlGetProp(node, (xmlChar*) "x");
    xmlChar* y = xmlGetProp(node, (xmlChar*) "y");
    xmlChar* w = xmlGetProp(node, (xmlChar*) "w");
    xmlChar* h = xmlGetProp(node, (xmlChar*) "h");

    r.setRectangle(atoi((char*) x), atoi((char*) y), atoi((char*) w), atoi((char*) h));

    xmlFree(x);
    xmlFree(y);
    xmlFree(w);
    xmlFree(h);
}

void
getCellN(xmlNodePtr node, const Rectangle& ref, Rectangle& dest)
{
    xmlChar* x = xmlGetProp(node, (xmlChar*) "x");
    xmlChar* y = xmlGetProp(node, (xmlChar*) "y");

    dest.setRectangle(atoi((char*) x), atoi((char*) y), ref.width(), ref.height());

    xmlFree(x);
    xmlFree(y);
}

void
getCell3H(xmlNodePtr node, r3& r)
{
    xmlChar* x = xmlGetProp(node, (xmlChar*) "x");
    xmlChar* y = xmlGetProp(node, (xmlChar*) "y");
    xmlChar* w1 = xmlGetProp(node, (xmlChar*) "w1");
    xmlChar* w2 = xmlGetProp(node, (xmlChar*) "w2");
    xmlChar* w3 = xmlGetProp(node, (xmlChar*) "w3");
    xmlChar* h = xmlGetProp(node, (xmlChar*) "h");

    r.l.setRectangle(atoi((char*) x), atoi((char*) y), atoi((char*) w1), atoi((char*) h));
    r.m.setRectangle(r.l.right(), r.l.y(), atoi((char*) w2), r.l.height());
    r.r.setRectangle(r.m.right(), r.m.y(), atoi((char*) w3), r.m.height());

    xmlFree(x);
    xmlFree(y);
    xmlFree(w1);
    xmlFree(w2);
    xmlFree(w3);
    xmlFree(h);
}

void
getCell3HN(xmlNodePtr node, const r3& ref, r3& dest)
{
    xmlChar* x = xmlGetProp(node, (xmlChar*) "x");
    xmlChar* y = xmlGetProp(node, (xmlChar*) "y");

    dest.l.setRectangle(atoi((char*) x), atoi((char*) y), ref.l.width(), ref.l.height());
    dest.m.setRectangle(dest.l.right(), dest.l.y(), ref.m.width(), ref.m.height());
    dest.r.setRectangle(dest.m.right(), dest.m.y(), ref.r.width(), ref.r.height());

    xmlFree(x);
    xmlFree(y);
}

void
getCell3V(xmlNodePtr node, r3& r)
{
    xmlChar* x = xmlGetProp(node, (xmlChar*) "x");
    xmlChar* y = xmlGetProp(node, (xmlChar*) "y");
    xmlChar* w = xmlGetProp(node, (xmlChar*) "w");
    xmlChar* h1 = xmlGetProp(node, (xmlChar*) "h1");
    xmlChar* h2 = xmlGetProp(node, (xmlChar*) "h2");
    xmlChar* h3 = xmlGetProp(node, (xmlChar*) "h3");

    r.l.setRectangle(atoi((char*) x), atoi((char*) y), atoi((char*) w), atoi((char*) h1));
    r.m.setRectangle(r.l.x(), r.l.bottom(), r.l.width(), atoi((char*) h2));
    r.r.setRectangle(r.m.x(), r.m.bottom(), r.l.width(), atoi((char*) h3));

    xmlFree(x);
    xmlFree(y);
    xmlFree(w);
    xmlFree(h1);
    xmlFree(h2);
    xmlFree(h3);
}

void
getCell3VN(xmlNodePtr node, const r3& ref, r3& dest)
{
    xmlChar* x = xmlGetProp(node, (xmlChar*) "x");
    xmlChar* y = xmlGetProp(node, (xmlChar*) "y");

    dest.l.setRectangle(atoi((char*) x), atoi((char*) y), ref.l.width(), ref.l.height());
    dest.m.setRectangle(dest.l.x(), dest.l.bottom(), ref.m.width(), ref.m.height());
    dest.r.setRectangle(dest.m.x(), dest.m.bottom(), ref.r.width(), ref.r.height());

    xmlFree(x);
    xmlFree(y);
}

void
getCell9(xmlNodePtr node, r9& r)
{
    xmlChar* x = xmlGetProp(node, (xmlChar*) "x");
    xmlChar* y = xmlGetProp(node, (xmlChar*) "y");
    xmlChar* w1 = xmlGetProp(node, (xmlChar*) "w1");
    xmlChar* w2 = xmlGetProp(node, (xmlChar*) "w2");
    xmlChar* w3 = xmlGetProp(node, (xmlChar*) "w3");
    xmlChar* h1 = xmlGetProp(node, (xmlChar*) "h1");
    xmlChar* h2 = xmlGetProp(node, (xmlChar*) "h2");
    xmlChar* h3 = xmlGetProp(node, (xmlChar*) "h3");

    // top-row
    r.tl.setRectangle(atoi((char*) x), atoi((char*) y), atoi((char*) w1), atoi((char*) h1));
    r.tm.setRectangle(r.tl.right(), r.tl.y(), atoi((char*) w2), r.tl.height());
    r.tr.setRectangle(r.tm.right(), r.tm.y(), atoi((char*) w3), r.tm.height());
    // mid-row
    r.l.setRectangle(r.tl.x(), r.tl.bottom(), r.tl.width(), atoi((char*) h2));
    r.m.setRectangle(r.tl.right(), r.tm.bottom(), r.tm.width(), r.l.height());
    r.r.setRectangle(r.tm.right(), r.tr.bottom(), r.tr.width(), r.m.height());
    // bottom-row
    r.bl.setRectangle(r.tl.x(), r.l.bottom(), r.tl.width(), atoi((char*) h3));
    r.bm.setRectangle(r.tl.right(), r.m.bottom(), r.tm.width(), r.bl.height());
    r.br.setRectangle(r.tm.right(), r.r.bottom(), r.tr.width(), r.bm.height());

    xmlFree(x);
    xmlFree(y);
    xmlFree(w1);
    xmlFree(w2);
    xmlFree(w3);
    xmlFree(h1);
    xmlFree(h2);
    xmlFree(h3);
}

void
getCell9N(xmlNodePtr node, const r9& ref, r9& dest)
{
    xmlChar* x = xmlGetProp(node, (xmlChar*) "x");
    xmlChar* y = xmlGetProp(node, (xmlChar*) "y");

    // top-row
    dest.tl.setRectangle(atoi((char*) x), atoi((char*) y), ref.tl.width(), ref.tl.height());
    dest.tm.setRectangle(dest.tl.right(), dest.tl.y(), ref.tm.width(), ref.tm.height());
    dest.tr.setRectangle(dest.tm.right(), dest.tm.y(), ref.tr.width(), ref.tr.height());
    // mid-row
    dest.l.setRectangle(dest.tl.x(), dest.tl.bottom(), dest.tl.width(), ref.l.height());
    dest.m.setRectangle(dest.tl.right(), dest.tm.bottom(), dest.tm.width(), dest.l.height());
    dest.r.setRectangle(dest.tm.right(), dest.tr.bottom(), dest.tr.width(), dest.m.height());
    // bottom-row
    dest.bl.setRectangle(dest.tl.x(), dest.l.bottom(), dest.tl.width(), ref.bl.height());
    dest.bm.setRectangle(dest.tl.right(), dest.m.bottom(), dest.tm.width(), dest.bl.height());
    dest.br.setRectangle(dest.tm.right(), dest.r.bottom(), dest.tr.width(), dest.bm.height());

    xmlFree(x);
    xmlFree(y);
}

std::istream&
operator>>(std::istream& is, r3& obj)
{
    is >> obj.l >> obj.m >> obj.r;
    return is;
}

std::ostream&
operator<<(std::ostream& os, const r3& obj)
{
    return os << obj.l << std::endl << obj.m << std::endl << obj.r << std::endl;
}

std::istream&
operator>>(std::istream& is, r9& obj)
{
    is >> obj.tl >> obj.tm >> obj.tr >> obj.l >> obj.m >> obj.r >> obj.bl >> obj.bm >> obj.br;
    return is;
}

std::ostream&
operator<<(std::ostream& os, const r9& obj)
{
    return os << obj.tl << std::endl << obj.tm << std::endl << obj.tr << std::endl << obj.l << std::endl << obj.m << std::endl << obj.r << std::endl << obj.bl << std::endl << obj.bm << std::endl << obj.br << std::endl;
}

} /* namespace ilixi */
