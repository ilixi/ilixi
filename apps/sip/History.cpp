/*
 * History.cpp
 *
 *  Created on: Jul 1, 2013
 *      Author: tarik
 */

#include "History.h"

#include <ui/VBoxLayout.h>

#include <sigc++/bind.h>
#include "HistoryItem.h"

namespace ilixi
{

History::History(Widget* parent)
        : Frame(parent)
{
    VBoxLayout* box = new VBoxLayout();
    box->setHorizontalAlignment(Alignment::Center);
    setLayout(box);
    setConstraints(MinimumConstraint, MinimumConstraint);

    ButtonGroup* _group1 = new ButtonGroup();
    addWidget(_group1);

    _remove = new DirectionalButton("Remove");
    _remove->setIcon(StyleHint::Minus);
    _remove->setDisabled();
    _remove->sigClicked.connect(sigc::mem_fun(this, &History::remove));
    _group1->addButton(_remove);

    _clearAll = new DirectionalButton("Clear All");
    _clearAll->setIcon(StyleHint::Trash);
    _clearAll->setDisabled();
    _clearAll->sigClicked.connect(sigc::mem_fun(this, &History::clear));
    _group1->addButton(_clearAll);

    _history = new ListBox();
    _history->setDrawFrame(true);
    _history->sigIndexChanged.connect(sigc::mem_fun(this, &History::updateButtons));
    addWidget(_history);

    ButtonGroup* group2 = new ButtonGroup();
    addWidget(group2);

    _txt = new DirectionalButton("Text");
    _txt->setIcon(StyleHint::Comment);
    _txt->setDisabled();
//    _txt->sigClicked.connect(sigc::mem_fun(this, &Contacts::addContact));
    group2->addButton(_txt);

    _call = new DirectionalButton("Call");
    _call->setDisabled();
    _call->setIcon(StyleHint::Phone);
    _call->sigClicked.connect(sigc::bind<bool>(sigc::mem_fun(this, &History::callContact), false));
    group2->addButton(_call);

    _video = new DirectionalButton("Video Call");
    _video->setDisabled();
    _video->setIcon(StyleHint::Camera);
    _video->sigClicked.connect(sigc::bind<bool>(sigc::mem_fun(this, &History::callContact), true));
    group2->addButton(_video);
}

History::~History()
{
    // TODO Save history to file.
}

void
History::clear()
{
    _history->clear();
    _remove->setDisabled();
    _clearAll->setDisabled();
    _call->setDisabled();
    _video->setDisabled();
}

void
History::add(bool incoming, const std::string& name, const std::string& addr)
{
    HistoryItem* item = new HistoryItem(incoming, name, addr);
    _history->addItem(item);
}

void
History::remove()
{
    if (!_history->currentItem())
        return;

    _history->removeItem(_history->currentItem());
    if (!_history->count())
    {
        _remove->setDisabled();
        _clearAll->setDisabled();
        _call->setDisabled();
        _video->setDisabled();
        _history->update();
    }
}

void
History::callContact(bool withVideo)
{
    if (!_history->currentItem())
        return;
    std::string name = ((HistoryItem*) _history->currentItem())->name();
    std::string addr = ((HistoryItem*) _history->currentItem())->addr();
    sigCall(name, addr, withVideo);
}

void
History::updateButtons(int oldItem, int newItem)
{
    if (_history->currentItem())
    {
        HistoryItem* previous = ((HistoryItem*) _history->itemAtIndex(oldItem));
        if (previous)
            previous->setSelected(false);

        if (_history->currentItem())
            ((HistoryItem*) _history->currentItem())->setSelected(true);

        _remove->setEnabled();
        _clearAll->setEnabled();
        _call->setEnabled();
        _video->setEnabled();
    } else
    {
        _remove->setDisabled();
        _clearAll->setDisabled();
        _call->setDisabled();
        _video->setDisabled();
    }
}

} /* namespace ilixi */
