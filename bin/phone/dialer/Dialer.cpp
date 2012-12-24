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

#include "Dialer.h"
#include <ui/GridLayout.h>
#include <ui/VBoxLayout.h>
#include <sigc++/bind.h>

namespace ilixi
{

Dialer::Dialer(int argc, char* argv[])
        : Application(&argc, &argv)
{
    setMargins(90, 50, 20, 20);
    setLayout(new VBoxLayout());

    _font = new Font("Sans", 36);

    _input = new LineInput("");
    _input->setInputMethod(NoInput);
    _input->setFont(_font);
    addWidget(_input);

    GridLayout* grid = new GridLayout(5, 3);
    grid->setKeyNavChildrenFirst(true);
    addWidget(grid);

    _keyCall = new PushButton("Call");
    _keyCall->setPushButtonStyle(OK);
    grid->addWidget(_keyCall);
    _keyCLR = new PushButton("CLR");
    _keyCLR->sigClicked.connect(sigc::bind<int, int>(sigc::mem_fun(_input, &LineInput::clear), -1, 1));
    grid->addWidget(_keyCLR);
    _keyEnd = new PushButton("End");
    _keyEnd->setPushButtonStyle(CANCEL);
    grid->addWidget(_keyEnd);

    _key1 = new PushButton("1");
    _key1->sigClicked.connect(sigc::bind<std::string>(sigc::mem_fun(_input, &LineInput::append), _key1->text()));
    grid->addWidget(_key1);
    _key2 = new PushButton("2");
    _key2->sigClicked.connect(sigc::bind<std::string>(sigc::mem_fun(_input, &LineInput::append), _key2->text()));
    grid->addWidget(_key2);
    _key3 = new PushButton("3");
    _key3->sigClicked.connect(sigc::bind<std::string>(sigc::mem_fun(_input, &LineInput::append), _key3->text()));
    grid->addWidget(_key3);

    _key4 = new PushButton("4");
    _key4->sigClicked.connect(sigc::bind<std::string>(sigc::mem_fun(_input, &LineInput::append), _key4->text()));
    grid->addWidget(_key4);
    _key5 = new PushButton("5");
    _key5->sigClicked.connect(sigc::bind<std::string>(sigc::mem_fun(_input, &LineInput::append), _key5->text()));
    grid->addWidget(_key5);
    _key6 = new PushButton("6");
    _key6->sigClicked.connect(sigc::bind<std::string>(sigc::mem_fun(_input, &LineInput::append), _key6->text()));
    grid->addWidget(_key6);

    _key7 = new PushButton("7");
    _key7->sigClicked.connect(sigc::bind<std::string>(sigc::mem_fun(_input, &LineInput::append), _key7->text()));
    grid->addWidget(_key7);
    _key8 = new PushButton("8");
    _key8->sigClicked.connect(sigc::bind<std::string>(sigc::mem_fun(_input, &LineInput::append), _key8->text()));
    grid->addWidget(_key8);
    _key9 = new PushButton("9");
    _key9->sigClicked.connect(sigc::bind<std::string>(sigc::mem_fun(_input, &LineInput::append), _key9->text()));
    grid->addWidget(_key9);

    _keyStar = new PushButton("*");
    _keyStar->sigClicked.connect(sigc::bind<std::string>(sigc::mem_fun(_input, &LineInput::append), _keyStar->text()));
    grid->addWidget(_keyStar);
    _key0 = new PushButton("0");
    _key0->sigClicked.connect(sigc::bind<std::string>(sigc::mem_fun(_input, &LineInput::append), _key0->text()));
    grid->addWidget(_key0);
    _keyHash = new PushButton("#");
    _keyHash->sigClicked.connect(sigc::bind<std::string>(sigc::mem_fun(_input, &LineInput::append), _keyHash->text()));
    grid->addWidget(_keyHash);
}

Dialer::~Dialer()
{
    delete _font;
}

bool
Dialer::windowPreEventFilter(const DFBWindowEvent& event)
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

        case DIKS_ASTERISK:
            _keyStar->setFocus();
            _keyStar->click();
            return true;

        case DIKS_0:
            _key0->setFocus();
            _key0->click();
            return true;

        case DIKS_NUMBER_SIGN:
            _keyHash->setFocus();
            _keyHash->click();
            return true;

        case DIKS_CLEAR:
            _keyCLR->setFocus();
            _keyCLR->click();
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
    ilixi::Dialer app(argc, argv);
    app.exec();
    return 0;
}
