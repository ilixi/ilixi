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

#include "OSK.h"
#include <graphics/Painter.h>
#include <sigc++/bind.h>
#include <core/Logger.h>
#include <ui/GridLayout.h>
#include <ui/VBoxLayout.h>
#include <ui/HBoxLayout.h>
#include <ilixiConfig.h>

LineInput* OSKButton::_target = NULL;
short OSKButton::_mode = 1;

OSKButtonThread::OSKButtonThread(OSKButton* button)
        : Thread(),
          _button(button)
{

}

OSKButtonThread::~OSKButtonThread()
{

}

int
OSKButtonThread::run()
{
    while (1)
    {
        pthread_testcancel();
        usleep(10000);
        pthread_testcancel();
        _button->pressSlot();
    }
    return 1;
}

//***********************************************************************
//***********************************************************************
OSKButton::OSKButton(std::string text, Widget* parent)
        : ToolButton(text, parent)
{
    setInputMethod(PointerInput);
    setConstraints(MinimumConstraint, MinimumConstraint);
    setToolButtonStyle(TextOnly);
}

OSKButton::OSKButton(std::string c1, std::string c2, std::string c3,
                     DFBInputDeviceKeySymbol s1, DFBInputDeviceKeySymbol s2,
                     DFBInputDeviceKeySymbol s3, Widget* parent)
        : ToolButton(c1, parent),
          _c1(c1),
          _c2(c2),
          _c3(c3),
          _s1(s1),
          _s2(s2),
          _s3(s3)
{
    setInputMethod(PointerInput);
    setConstraints(MinimumConstraint, MinimumConstraint);
    setToolButtonStyle(TextOnly);

    sigClicked.connect(sigc::mem_fun(this, &OSKButton::pressSlot));
}

OSKButton::~OSKButton()
{
}

void
OSKButton::pressSlot()
{
    if (_mode == 1)
        _target->consumeKeyEvent(KeyEvent(KeyDownEvent, _s1));
    else if (_mode == 2)
        _target->consumeKeyEvent(KeyEvent(KeyDownEvent, _s2));
    else
        _target->consumeKeyEvent(KeyEvent(KeyDownEvent, _s3));
}

//***********************************************************************
//***********************************************************************

OSK::OSK(int argc, char* argv[])
        : Application(&argc, &argv)
{
    setTitle("OSK");
    setBackgroundFilled(true);
    setLayout(new VBoxLayout());

//    _input = new LineInput("");
//    addWidget(_input);
//
//    OSKButton::_target = _input;
//
//    createKeys();
//
//    sigVisible.connect(sigc::mem_fun(this, &OSK::setOptimalFontSize));

    _keyboard = new Keyboard();
    _keyboard->parseLayoutFile(ILIXI_DATADIR"osk/keyboard.xml");
    addWidget(_keyboard);
}

OSK::~OSK()
{
    delete _buttonFont;
}

void
OSK::append(DFBInputDeviceKeySymbol symbol)
{
    _input->consumeKeyEvent(KeyEvent(KeyDownEvent, symbol));
}

void
OSK::changeCase()
{
    if (OSKButton::_mode == 3)
        return;
    else if (OSKButton::_mode == 1)
        OSKButton::_mode = 2;
    else if (OSKButton::_mode == 2)
        OSKButton::_mode = 1;
    changeSymbols();
}

void
OSK::changeLayout()
{
    if (OSKButton::_mode == 3)
    {
        if (_caps->checked())
            OSKButton::_mode = 2;
        else
            OSKButton::_mode = 1;
        _caps->setEnabled();
        _style->setText("123");
    } else
    {
        OSKButton::_mode = 3;
        _caps->setDisabled();
        if (_caps->checked())
            _style->setText("QWE");
        else
            _style->setText("qwe");
    }
    changeSymbols();
}

void
OSK::changeSymbols()
{
    if (OSKButton::_mode == 1)
        for (int i = 0; i < _keys.size(); i++)
            _keys[i]->setText(_keys[i]->_c1);
    else if (OSKButton::_mode == 2)
        for (int i = 0; i < _keys.size(); i++)
            _keys[i]->setText(_keys[i]->_c2);
    else
        for (int i = 0; i < _keys.size(); i++)
            _keys[i]->setText(_keys[i]->_c3);
}

void
OSK::createKeys()
{
    GridLayout* keyLayout = new GridLayout(4, 10);
    //***********************************************************************
    // Row 1
    //***********************************************************************
    // q w e r t y u i o p
    // Q W E R T Y U I O P
    // 1 2 3 4 5 6 7 8 9 0
    //***********************************************************************
    OSKButton* q = new OSKButton("q", "Q", "1", DIKS_SMALL_Q, DIKS_CAPITAL_Q,
                                 DIKS_1);
    keyLayout->addWidget(q);
    _keys.push_back(q);

    OSKButton* w = new OSKButton("w", "W", "2", DIKS_SMALL_W, DIKS_CAPITAL_W,
                                 DIKS_2);
    keyLayout->addWidget(w);
    _keys.push_back(w);

    OSKButton* e = new OSKButton("e", "E", "3", DIKS_SMALL_E, DIKS_CAPITAL_E,
                                 DIKS_3);
    keyLayout->addWidget(e);
    _keys.push_back(e);

    OSKButton* r = new OSKButton("r", "R", "4", DIKS_SMALL_R, DIKS_CAPITAL_R,
                                 DIKS_4);
    keyLayout->addWidget(r);
    _keys.push_back(r);

    OSKButton* t = new OSKButton("t", "T", "5", DIKS_SMALL_T, DIKS_CAPITAL_T,
                                 DIKS_5);
    keyLayout->addWidget(t);
    _keys.push_back(t);

    OSKButton* y = new OSKButton("y", "Y", "6", DIKS_SMALL_Y, DIKS_CAPITAL_Y,
                                 DIKS_6);
    keyLayout->addWidget(y);
    _keys.push_back(y);

    OSKButton* u = new OSKButton("u", "U", "7", DIKS_SMALL_U, DIKS_CAPITAL_U,
                                 DIKS_7);
    keyLayout->addWidget(u);
    _keys.push_back(u);

    OSKButton* i = new OSKButton("i", "I", "8", DIKS_SMALL_I, DIKS_CAPITAL_I,
                                 DIKS_8);
    keyLayout->addWidget(i);
    _keys.push_back(i);

    OSKButton* o = new OSKButton("o", "O", "9", DIKS_SMALL_O, DIKS_CAPITAL_O,
                                 DIKS_9);
    keyLayout->addWidget(o);
    _keys.push_back(o);

    OSKButton* p = new OSKButton("p", "P", "0", DIKS_SMALL_P, DIKS_CAPITAL_P,
                                 DIKS_0);
    keyLayout->addWidget(p);
    _keys.push_back(p);

    //***********************************************************************
    // Row 2
    //***********************************************************************
    // a s d f g h j k l '
    // A S D F G H J K L @
    // + - * / % & ( ) [ ]
    //***********************************************************************
    OSKButton* a = new OSKButton("a", "A", "+", DIKS_SMALL_A, DIKS_CAPITAL_A,
                                 DIKS_PLUS_SIGN);
    keyLayout->addWidget(a);
    _keys.push_back(a);

    OSKButton* s = new OSKButton("s", "S", "-", DIKS_SMALL_S, DIKS_CAPITAL_S,
                                 DIKS_MINUS_SIGN);
    keyLayout->addWidget(s);
    _keys.push_back(s);

    OSKButton* d = new OSKButton("d", "D", "*", DIKS_SMALL_D, DIKS_CAPITAL_D,
                                 DIKS_ASTERISK);
    keyLayout->addWidget(d);
    _keys.push_back(d);

    OSKButton* f = new OSKButton("f", "F", "/", DIKS_SMALL_F, DIKS_CAPITAL_F,
                                 DIKS_SLASH);
    keyLayout->addWidget(f);
    _keys.push_back(f);

    OSKButton* g = new OSKButton("g", "G", "%", DIKS_SMALL_G, DIKS_CAPITAL_G,
                                 DIKS_PERCENT_SIGN);
    keyLayout->addWidget(g);
    _keys.push_back(g);

    OSKButton* h = new OSKButton("h", "H", "&", DIKS_SMALL_H, DIKS_CAPITAL_H,
                                 DIKS_AMPERSAND);
    keyLayout->addWidget(h);
    _keys.push_back(h);

    OSKButton* j = new OSKButton("j", "J", "(", DIKS_SMALL_J, DIKS_CAPITAL_J,
                                 DIKS_PARENTHESIS_LEFT);
    keyLayout->addWidget(j);
    _keys.push_back(j);

    OSKButton* k = new OSKButton("k", "K", ")", DIKS_SMALL_K, DIKS_CAPITAL_K,
                                 DIKS_PARENTHESIS_RIGHT);
    keyLayout->addWidget(k);
    _keys.push_back(k);

    OSKButton* l = new OSKButton("l", "L", "[", DIKS_SMALL_L, DIKS_CAPITAL_L,
                                 DIKS_SQUARE_BRACKET_LEFT);
    keyLayout->addWidget(l);
    _keys.push_back(l);

    OSKButton* apostrophe = new OSKButton("'", "@", "]", DIKS_APOSTROPHE,
                                          DIKS_AT, DIKS_SQUARE_BRACKET_RIGHT);
    keyLayout->addWidget(apostrophe);
    _keys.push_back(apostrophe);

    //***********************************************************************
    // Row 3
    //***********************************************************************
    // Z X C V B N M , . ?
    // Z X C V B N M ; : !
    // = # $ \ ~ ^ { } < >
    //***********************************************************************
    OSKButton* z = new OSKButton("z", "Z", "=", DIKS_SMALL_Z, DIKS_CAPITAL_Z,
                                 DIKS_EQUALS_SIGN);
    keyLayout->addWidget(z);
    _keys.push_back(z);

    OSKButton* x = new OSKButton("x", "X", "#", DIKS_SMALL_X, DIKS_CAPITAL_X,
                                 DIKS_NUMBER_SIGN);
    keyLayout->addWidget(x);
    _keys.push_back(x);

    OSKButton* c = new OSKButton("c", "C", "$", DIKS_SMALL_C, DIKS_CAPITAL_C,
                                 DIKS_DOLLAR_SIGN);
    keyLayout->addWidget(c);
    _keys.push_back(c);

    OSKButton* v = new OSKButton("v", "V", "\\", DIKS_SMALL_V, DIKS_CAPITAL_V,
                                 DIKS_BACKSLASH);
    keyLayout->addWidget(v);
    _keys.push_back(v);

    OSKButton* b = new OSKButton("b", "B", "~", DIKS_SMALL_B, DIKS_CAPITAL_B,
                                 DIKS_TILDE);
    keyLayout->addWidget(b);
    _keys.push_back(b);

    OSKButton* n = new OSKButton("n", "N", "^", DIKS_SMALL_N, DIKS_CAPITAL_N,
                                 DIKS_CIRCUMFLEX_ACCENT);
    keyLayout->addWidget(n);
    _keys.push_back(n);

    OSKButton* m = new OSKButton("m", "M", "{", DIKS_SMALL_M, DIKS_CAPITAL_M,
                                 DIKS_CURLY_BRACKET_LEFT);
    keyLayout->addWidget(m);
    _keys.push_back(m);

    OSKButton* comma = new OSKButton(",", ";", "}", DIKS_COMMA, DIKS_SEMICOLON,
                                     DIKS_CURLY_BRACKET_RIGHT);
    keyLayout->addWidget(comma);
    _keys.push_back(comma);

    OSKButton* period = new OSKButton(".", ":", "<", DIKS_PERIOD, DIKS_COLON,
                                      DIKS_LESS_THAN_SIGN);
    keyLayout->addWidget(period);
    _keys.push_back(period);

    OSKButton* question = new OSKButton("?", "!", ">", DIKS_QUESTION_MARK,
                                        DIKS_EXCLAMATION_MARK,
                                        DIKS_GREATER_THAN_SIGN);
    keyLayout->addWidget(question);
    _keys.push_back(question);

    //***********************************************************************
    // ToolButtons (Row 4)
    //***********************************************************************
    HBoxLayout* tkeyLayout = new HBoxLayout();
    OSKButton* doneB = new OSKButton("Done");
    doneB->setToolButtonStyle(ToolButton::IconOnly);
    doneB->setIcon(ILIXI_IMAGEDIR"ok.png");
    keyLayout->addWidget(doneB);
    doneB->sigClicked.connect(sigc::mem_fun(this, &OSK::done));

    _caps = new OSKButton("CAP");
    _caps->setToolButtonStyle(ToolButton::IconOnly);
    _caps->setCheckable(true);
    _caps->setIcon(ILIXI_IMAGEDIR"caps.png");
    _caps->sigClicked.connect(sigc::mem_fun(this, &OSK::changeCase));
    keyLayout->addWidget(_caps);

    _style = new OSKButton("123");
    _style->sigClicked.connect(sigc::mem_fun(this, &OSK::changeLayout));
    keyLayout->addWidget(_style);

    OSKButton* left = new OSKButton("&lt;");
    left->setToolButtonStyle(ToolButton::IconOnly);
    left->setIcon(ILIXI_IMAGEDIR"left_arrow.png");
    left->sigClicked.connect(
            sigc::bind<DFBInputDeviceKeySymbol>(
                    sigc::mem_fun(this, &OSK::append), DIKS_CURSOR_LEFT));
    keyLayout->addWidget(left);

    OSKButton* space = new OSKButton("Space");
    space->setToolButtonStyle(ToolButton::IconOnly);
    space->setIcon(ILIXI_IMAGEDIR"space.png");
    space->sigClicked.connect(
            sigc::bind<DFBInputDeviceKeySymbol>(
                    sigc::mem_fun(this, &OSK::append), DIKS_SPACE));
    keyLayout->addWidget(space, 3, 4, 1, 2);

    OSKButton* right = new OSKButton("&gt;");
    right->setToolButtonStyle(ToolButton::IconOnly);
    right->setIcon(ILIXI_IMAGEDIR"right_arrow.png");
    right->sigClicked.connect(
            sigc::bind<DFBInputDeviceKeySymbol>(
                    sigc::mem_fun(this, &OSK::append), DIKS_CURSOR_RIGHT));
    keyLayout->addWidget(right);

    OSKButton* clr = new OSKButton("Clr");
    clr->setToolButtonStyle(ToolButton::IconOnly);
    clr->setIcon(ILIXI_IMAGEDIR"cancel.png");
    clr->sigClicked.connect(sigc::mem_fun(_input, &LineInput::clear));
    keyLayout->addWidget(clr);

    OSKButton* rturn = new OSKButton("Rtn");
    rturn->setToolButtonStyle(ToolButton::IconOnly);
    rturn->setIcon(ILIXI_IMAGEDIR"return.png");
    rturn->setDisabled();
    comma->sigClicked.connect(
            sigc::bind<DFBInputDeviceKeySymbol>(
                    sigc::mem_fun(this, &OSK::append), DIKS_RETURN));
    keyLayout->addWidget(rturn);

    OSKButton* backSpace = new OSKButton("&lt;-");
    backSpace->setToolButtonStyle(ToolButton::IconOnly);
    backSpace->setIcon(ILIXI_IMAGEDIR"backspace.png");
    backSpace->sigClicked.connect(
            sigc::bind<DFBInputDeviceKeySymbol>(
                    sigc::mem_fun(this, &OSK::append), DIKS_BACKSPACE));
    keyLayout->addWidget(backSpace);

    addWidget(keyLayout);
}

void
OSK::done()
{
//    setOSKText(_input->text());
//    callMaestro(OSKEvent, Ready);
//    hide(Slide, getStatusBarHeight(), _layerConfig.height);
}

void
OSK::compose(const PaintEvent& event)
{
    Painter painter(this);
    painter.begin(event);
    painter.setBrush(Color(0, 0, 0, 128));
    painter.drawRectangle(0, 0, width(), height());
    painter.end();
}

void
OSK::setOptimalFontSize()
{
    _buttonFont = new Font("decker", (width() - 250) / 10);
    _buttonFont->setStyle(Font::Bold);
    _buttonFont->dfbFont();
    for (int i = 0; i < _keys.size(); i++)
        _keys[i]->setFont(_buttonFont);
    update();
}

int
main(int argc, char* argv[])
{
    OSK app(argc, argv);
    app.exec();
    return 0;
}
