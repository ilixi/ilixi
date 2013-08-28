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

#ifndef ILIXI_STYLE_H_
#define ILIXI_STYLE_H_

#include <types/Image.h>
#include <types/Rectangle.h>
#include <libxml/tree.h>
#include <map>
#include <iostream>

namespace ilixi
{

//! Contains images and other style related data.
class Style
{
public:
    /*!
     * Initialise to default.
     */
    Style();

    /*!
     * Destructor.
     */
    ~Style();

    /*!
     * Initialise style from an XML file.
     *
     * Returns false on error.
     */
    bool
    parseStyle(const char* filename);

    //! This image stores the widget pack.
    Image* _pack;

#ifndef ILX_DOXYGEN_SKIP

    struct r1_Input
    {
        Rectangle def;
        Rectangle pre;
        Rectangle exp;
        Rectangle dis;
        Rectangle foc;
    };

    struct r1_View_Panel
    {
        Rectangle tl;
        Rectangle tr;
        Rectangle bl;
        Rectangle br;
    };

    struct r3
    {
        Rectangle l;
        Rectangle m;
        Rectangle r;
    };

    struct r3_Input
    {
        r3 def;
        r3 pre;
        r3 exp;
        r3 dis;
        r3 foc;
    };

    struct r3_View
    {
        r3 def;
        r3 dis;
    };

    struct r3_View_Fill
    {
        r3 def;
        r3 dis;
        r3 fill;
        r3 fill_dis;
    };

    struct r9
    {
        Rectangle tl;
        Rectangle tm;
        Rectangle tr;
        Rectangle l;
        Rectangle m;
        Rectangle r;
        Rectangle bl;
        Rectangle bm;
        Rectangle br;
    };

    struct r9_Input
    {
        r9 def;
        r9 pre;
        r9 exp;
        r9 dis;
        r9 foc;
    };

    struct r9_Input2
    {
        r9 def;
        r9 dis;
        r9 foc;
    };

    struct r9_View
    {
        r9 def;
        r9 dis;
    };

    struct r9_View_Panel
    {
        r9 def;
        r9 dis;
        r9 passive;
    };

    // PushButton
    r3_Input pb;
    // PushButtonOK
    r3_Input pbOK;
    // PushButtonCancel
    r3_Input pbCAN;

    // ToolButton
    r9_Input tb;
    // DirectionalButton1
    r9_Input db1;
    // DirectionalButton2
    r9_Input db2;
    // LineInput
    r9_Input2 li;
    r9_Input2 li2;

    // Checkbox
    r1_Input cb;
    // Checkbox checked
    r1_Input cbC;
    // Checkbox tri state
    r1_Input cbT;
    // RadioButton on
    r1_Input rbOn;
    // RadioButton off
    r1_Input rbOff;

    // Horizontal progress bar
    r3_View prH;
    // Horizontal progress bar indicator
    r3_View prHI;
    // Vertical progress bar
    r3_View prV;
    // Vertical progress bar indicator
    r3_View prVI;

    // Horizontal slider
    r3_View_Fill hSl;
    // Vertical slider
    r3_View_Fill vSl;
    // Slider indicator
    r1_Input slI;

    // Frame
    r9_View fr;

    // Panel
    r9_View_Panel panel;
    // Panel inverse
    r1_View_Panel panelInv;
    char overlap;
    // Panel inverse disabled
    r1_View_Panel panelInvDis;

    // Horizontal scroller
    r3 hScr;
    // Vertical scroller
    r3 vScr;

    // ScrollBar
    r9_View sb;
    // Horizontal scroll bar ridge
    Rectangle sbRH;
    // Vertical scroll bar ridge
    Rectangle sbRV;

    // ToolBar
    Rectangle tbar;
    // ToolBarButton
    r3_Input tbarb;

    // Horizontal line
    Rectangle hLine;
    // Vertical line
    Rectangle vLine;

    // toolbutton indicators
    r3_View tbIndH;
    r3_View tbIndV;

    // dialog frame
    r9 dialog;
#endif

protected:
    void
    parseTheme(xmlNodePtr node);

    void
    getCell(xmlNodePtr node, Rectangle& dest);

    void
    getCellN(xmlNodePtr node, const Rectangle& ref, Rectangle& dest);

    void
    getCell3H(xmlNodePtr node, r3& dest);

    void
    getCell3HN(xmlNodePtr node, const r3& ref, r3& dest);

    void
    getCell3V(xmlNodePtr node, r3& dest);

    void
    getCell3VN(xmlNodePtr node, const r3& ref, r3& dest);

    void
    getCell9(xmlNodePtr node, r9& dest);

    void
    getCell9N(xmlNodePtr node, const r9& ref, r9& dest);

private:
    void
    release();

    friend std::istream&
    operator>>(std::istream& is, Style& obj);

    friend std::ostream&
    operator<<(std::ostream& os, const Style& obj);

    friend std::istream&
    operator>>(std::istream& is, r3& obj);

    friend std::ostream&
    operator<<(std::ostream& os, const r3& obj);

    friend std::istream&
    operator>>(std::istream& is, r9& obj);

    friend std::ostream&
    operator<<(std::ostream& os, const r9& obj);

};

}

#endif /* ILIXI_STYLE_H_ */
