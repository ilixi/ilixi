/*
 * Dialer.cpp
 *
 *  Created on: Jun 18, 2013
 *      Author: tarik
 */

#include "Dialer.h"
#include <ui/GridLayout.h>
#include <ui/HBoxLayout.h>
#include <ui/VBoxLayout.h>
#include <sigc++/bind.h>

namespace ilixi
{

Dialer::Dialer(Widget* parent)
        : Frame(parent)
{
    setConstraints(MinimumConstraint, FixedConstraint);
    setLayout(new VBoxLayout());

    HBoxLayout* box = new HBoxLayout();
    box->setVerticalAlignment(Alignment::Middle);
    box->setConstraints(MinimumConstraint, FixedConstraint);
    addWidget(box);

    _input = new LineInput("");
    _input->setInputMethod(NoInput);
    box->addWidget(_input);

    _keyCall = new PushButton("Dial");
    _keyCall->setPushButtonStyle(OK);
    box->addWidget(_keyCall);

    GridLayout* grid = new GridLayout(4, 3);
    grid->setConstraints(MinimumConstraint, FixedConstraint);
    grid->setKeyNavChildrenFirst(true);
    addWidget(grid);

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
}

bool
Dialer::consumeKey(const DFBWindowEvent& event)
{
    if (!visible())
        return false;

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
//            _keyCLR->setFocus();
//            _keyCLR->click();
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

void
Dialer::sendKey(const std::string& key)
{

}

} /* namespace ilixi */
