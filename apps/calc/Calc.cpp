/*
 Copyright 2010-2012 Tarik Sekmen.

 All Rights Reserved.

 Written by Tarik Sekmen <tarik@ilixi.org>,
            Sonat Deniz <sonat@ilixi.org>.

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

#include "Calc.h"
#include <ui/VBoxLayout.h>
#include <ui/GridLayout.h>
#include <sigc++/bind.h>

#include "Parser.h"

namespace ilixi
{

Calc::Calc(int argc, char* argv[])
        : Application(&argc, &argv)
{
    setMargins(50, 50, 20, 20);
    VBoxLayout* box = new VBoxLayout();
    box->setSpacing(20);
    setLayout(box);

    _font = new Font("Sans", 48);

    _input = new LineInput("");
    _input->setInputMethod(NoInput);
    _input->setFont(_font);
    addWidget(_input);

    GridLayout* grid = new GridLayout(5, 5);
    grid->setConstraints(ExpandingConstraint, ExpandingConstraint);
    grid->setKeyNavChildrenFirst(true);
    addWidget(grid);

    _keyCLR = new ToolButton("CLR");
    _keyCLR->setConstraints(ExpandingConstraint, ExpandingConstraint);
    _keyCLR->sigClicked.connect(sigc::bind<int, int>(sigc::mem_fun(_input, &LineInput::clear), -1, -1));
    grid->addWidget(_keyCLR);
    _keyPo = new ToolButton("(");
    _keyPo->setConstraints(ExpandingConstraint, ExpandingConstraint);
    _keyPo->sigClicked.connect(sigc::bind<std::string>(sigc::mem_fun(_input, &LineInput::append), _keyPo->text()));
    grid->addWidget(_keyPo);
    _keyPc = new ToolButton(")");
    _keyPc->setConstraints(ExpandingConstraint, ExpandingConstraint);
    _keyPc->sigClicked.connect(sigc::bind<std::string>(sigc::mem_fun(_input, &LineInput::append), _keyPc->text()));
    grid->addWidget(_keyPc);
    _keyDiv = new ToolButton("/");
    _keyDiv->setConstraints(ExpandingConstraint, ExpandingConstraint);
    _keyDiv->sigClicked.connect(sigc::bind<std::string>(sigc::mem_fun(_input, &LineInput::append), _keyDiv->text()));
    grid->addWidget(_keyDiv);
    _keyBack = new ToolButton("");
    _keyBack->setIcon(ILIXI_DATADIR"osk/backspace.png", Size(32, 32));
    _keyBack->setToolButtonStyle(ToolButton::IconOnly);
    _keyBack->setConstraints(ExpandingConstraint, ExpandingConstraint);
    _keyBack->sigClicked.connect(sigc::bind<int, int>(sigc::mem_fun(_input, &LineInput::clear), -1, 1));
    grid->addWidget(_keyBack);

    _key7 = new ToolButton("7");
    _key7->setConstraints(ExpandingConstraint, ExpandingConstraint);
    _key7->sigClicked.connect(sigc::bind<std::string>(sigc::mem_fun(_input, &LineInput::append), _key7->text()));
    grid->addWidget(_key7);
    _key8 = new ToolButton("8");
    _key8->setConstraints(ExpandingConstraint, ExpandingConstraint);
    _key8->sigClicked.connect(sigc::bind<std::string>(sigc::mem_fun(_input, &LineInput::append), _key8->text()));
    grid->addWidget(_key8);
    _key9 = new ToolButton("9");
    _key9->setConstraints(ExpandingConstraint, ExpandingConstraint);
    _key9->sigClicked.connect(sigc::bind<std::string>(sigc::mem_fun(_input, &LineInput::append), _key9->text()));
    grid->addWidget(_key9);
    _keyMult = new ToolButton("*");
    _keyMult->setConstraints(ExpandingConstraint, ExpandingConstraint);
    _keyMult->sigClicked.connect(sigc::bind<std::string>(sigc::mem_fun(_input, &LineInput::append), _keyMult->text()));
    grid->addWidget(_keyMult);
    _keyMod = new ToolButton("%");
    _keyMod->setConstraints(ExpandingConstraint, ExpandingConstraint);
    _keyMod->sigClicked.connect(sigc::bind<std::string>(sigc::mem_fun(_input, &LineInput::append), _keyMod->text()));
    grid->addWidget(_keyMod);

    _key4 = new ToolButton("4");
    _key4->setConstraints(ExpandingConstraint, ExpandingConstraint);
    _key4->sigClicked.connect(sigc::bind<std::string>(sigc::mem_fun(_input, &LineInput::append), _key4->text()));
    grid->addWidget(_key4);
    _key5 = new ToolButton("5");
    _key5->setConstraints(ExpandingConstraint, ExpandingConstraint);
    _key5->sigClicked.connect(sigc::bind<std::string>(sigc::mem_fun(_input, &LineInput::append), _key5->text()));
    grid->addWidget(_key5);
    _key6 = new ToolButton("6");
    _key6->setConstraints(ExpandingConstraint, ExpandingConstraint);
    _key6->sigClicked.connect(sigc::bind<std::string>(sigc::mem_fun(_input, &LineInput::append), _key6->text()));
    grid->addWidget(_key6);
    _keyMin = new ToolButton("-");
    _keyMin->setConstraints(ExpandingConstraint, ExpandingConstraint);
    _keyMin->sigClicked.connect(sigc::bind<std::string>(sigc::mem_fun(_input, &LineInput::append), _keyMin->text()));
    grid->addWidget(_keyMin);
    _keyPow = new ToolButton("^");
    _keyPow->setConstraints(ExpandingConstraint, ExpandingConstraint);
    _keyPow->sigClicked.connect(sigc::bind<std::string>(sigc::mem_fun(_input, &LineInput::append), _keyPow->text()));
    grid->addWidget(_keyPow);

    _key1 = new ToolButton("1");
    _key1->setConstraints(ExpandingConstraint, ExpandingConstraint);
    _key1->sigClicked.connect(sigc::bind<std::string>(sigc::mem_fun(_input, &LineInput::append), _key1->text()));
    grid->addWidget(_key1);
    _key2 = new ToolButton("2");
    _key2->setConstraints(ExpandingConstraint, ExpandingConstraint);
    _key2->sigClicked.connect(sigc::bind<std::string>(sigc::mem_fun(_input, &LineInput::append), _key2->text()));
    grid->addWidget(_key2);
    _key3 = new ToolButton("3");
    _key3->setConstraints(ExpandingConstraint, ExpandingConstraint);
    _key3->sigClicked.connect(sigc::bind<std::string>(sigc::mem_fun(_input, &LineInput::append), _key3->text()));
    grid->addWidget(_key3);
    _keyPlus = new ToolButton("+");
    _keyPlus->setConstraints(ExpandingConstraint, ExpandingConstraint);
    _keyPlus->sigClicked.connect(sigc::bind<std::string>(sigc::mem_fun(_input, &LineInput::append), _keyPlus->text()));
    grid->addWidget(_keyPlus);
    _keySqrt = new ToolButton("sqrt");
    _keySqrt->setConstraints(ExpandingConstraint, ExpandingConstraint);
    _keySqrt->sigClicked.connect(sigc::bind<std::string>(sigc::mem_fun(_input, &LineInput::append), "|"));
    grid->addWidget(_keySqrt);

    _key0 = new ToolButton("0");
    _key0->setConstraints(ExpandingConstraint, ExpandingConstraint);
    _key0->sigClicked.connect(sigc::bind<std::string>(sigc::mem_fun(_input, &LineInput::append), _key0->text()));
    grid->addWidget(_key0);
    _keyDot = new ToolButton(".");
    _keyDot->setConstraints(ExpandingConstraint, ExpandingConstraint);
    _keyDot->sigClicked.connect(sigc::bind<std::string>(sigc::mem_fun(_input, &LineInput::append), _keyDot->text()));
    grid->addWidget(_keyDot);
    _keyEq = new ToolButton("=");
    _keyEq->setConstraints(ExpandingConstraint, ExpandingConstraint);
    _keyEq->sigClicked.connect(sigc::mem_fun(this, &Calc::calculate));
    grid->addWidget(_keyEq, 4, 2, 0, 3);

    _dialog = new Dialog("Exception", Dialog::OKButtonOption);
    _dialog->setLayout(new VBoxLayout());
    _text = new Label("");
    _dialog->addWidget(_text);
    _errorText = new Label("");
    _dialog->addWidget(_errorText);
}

Calc::~Calc()
{
    delete _font;
}

void
Calc::calculate()
{
    if (_input->text().empty())
        return;

    try
    {
        Parser p(_input->text());
        _input->setText(p.getResult());
    } catch (std::exception& e)
    {
        _text->setText(_input->text());
        _errorText->setText(e.what());
        _dialog->execute();
    }
}

bool
Calc::windowPreEventFilter(const DFBWindowEvent& event)
{
    switch (event.type)
    {
    case DWET_KEYDOWN:
        switch (event.key_symbol)
        {
        case DIKS_1:
            _key1->setFocus();
            _key1->click();
            return true;

        case DIKS_2:
            _key2->setFocus();
            _key2->click();
            return true;

        case DIKS_3:
            _key3->setFocus();
            _key3->click();
            return true;

        case DIKS_4:
            _key4->setFocus();
            _key4->click();
            return true;

        case DIKS_5:
            _key5->setFocus();
            _key5->click();
            return true;

        case DIKS_6:
            _key6->setFocus();
            _key6->click();
            return true;

        case DIKS_7:
            _key7->setFocus();
            _key7->click();
            return true;

        case DIKS_8:
            _key8->setFocus();
            _key8->click();
            return true;

        case DIKS_9:
            _key9->setFocus();
            _key9->click();
            return true;

        case DIKS_0:
            _key0->setFocus();
            _key0->click();
            return true;

        case DIKS_PERIOD:
            _keyDot->setFocus();
            _keyDot->click();
            return true;

        case DIKS_CLEAR:
            _keyCLR->setFocus();
            _keyCLR->click();
            return true;

        case DIKS_PARENTHESIS_LEFT:
            _keyPo->setFocus();
            _keyPo->click();
            return true;

        case DIKS_PARENTHESIS_RIGHT:
            _keyPc->setFocus();
            _keyPc->click();
            return true;

        case DIKS_SLASH:
            _keyDiv->setFocus();
            _keyDiv->click();
            return true;

        case DIKS_BACKSPACE:
            _keyBack->setFocus();
            _keyBack->click();
            return true;

        case DIKS_ASTERISK:
            _keyMult->setFocus();
            _keyMult->click();
            return true;

        case DIKS_PERCENT_SIGN:
            _keyMod->setFocus();
            _keyMod->click();
            return true;

        case DIKS_MINUS_SIGN:
            _keyMin->setFocus();
            _keyMin->click();
            return true;

        case DIKS_PLUS_SIGN:
            _keyPlus->setFocus();
            _keyPlus->click();
            return true;

        case DIKS_VERTICAL_BAR:
            _keySqrt->setFocus();
            _keySqrt->click();
            return true;

        case DIKS_CIRCUMFLEX_ACCENT:
            _keyPow->setFocus();
            _keyPow->click();
            return true;

        case DIKS_RETURN:
            _keyEq->setFocus();
            _keyEq->click();
            return true;

        default:
            break;
        }
        break;

    default:
        break;
    }

    return false;
}

} /* namespace ilixi */

int
main(int argc, char* argv[])
{
    ilixi::Calc app(argc, argv);
    app.exec();
    return 0;
}
