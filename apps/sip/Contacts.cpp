/*
 * Contacts.cpp
 *
 *  Created on: Jun 19, 2013
 *      Author: tarik
 */

#include "Contacts.h"

#include <core/Logger.h>
#include <core/PlatformManager.h>

#include <lib/FileSystem.h>
#include <ui/Label.h>
#include <ui/VBoxLayout.h>

#include <sigc++/bind.h>
#include <fstream>

namespace ilixi
{

D_DEBUG_DOMAIN(ILX_SIP_CONTACTS, "ilixi_sip/Contacts", "Contacts");

Contacts::Contacts(ContactManager* manager, Widget* parent)
        : Frame(parent),
          _manager(manager),
          _dialogAddMode(1)
{
    VBoxLayout* box = new VBoxLayout();
    box->setHorizontalAlignment(Alignment::Center);
    setLayout(box);

    ButtonGroup* group1 = new ButtonGroup();
    addWidget(group1);

    _add = new DirectionalButton("Add");
    _add->setIcon(StyleHint::Plus);
    _add->sigClicked.connect(sigc::mem_fun(this, &Contacts::showAddContactDialog));
    group1->addButton(_add);

    _remove = new DirectionalButton("Remove");
    _remove->setDisabled();
    _remove->setIcon(StyleHint::Minus);
    _remove->sigClicked.connect(sigc::mem_fun(this, &Contacts::removeContact));
    group1->addButton(_remove);

    _edit = new DirectionalButton("Edit");
    _edit->setDisabled();
    _edit->setIcon(StyleHint::Edit);
    _edit->sigClicked.connect(sigc::mem_fun(this, &Contacts::showEditContactDialog));
    group1->addButton(_edit);

    _contactsList = new ListBox();
    _contactsList->setDrawFrame(true);
    _contactsList->sigIndexChanged.connect(sigc::mem_fun(this, &Contacts::updateButtons));
    addWidget(_contactsList);

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
    _call->sigClicked.connect(sigc::bind<bool>(sigc::mem_fun(this, &Contacts::callContact), false));
    group2->addButton(_call);

    _video = new DirectionalButton("Video Call");
    _video->setDisabled();
    _video->setIcon(StyleHint::Camera);
    _video->sigClicked.connect(sigc::bind<bool>(sigc::mem_fun(this, &Contacts::callContact), true));
    group2->addButton(_video);

    _contactDialog = new Dialog("Add Contact", Dialog::OKCancelButtonOption);
    _contactDialog->setLayout(new VBoxLayout());
    _contactDialog->addWidget(new Label("Name:"));
    _nameInput = new LineInput("");
    _contactDialog->addWidget(_nameInput);
    Size s = _nameInput->preferredSize();
    _nameInput->setMinimumSize(PlatformManager::instance().getLayerSize().width() - 200, s.height());
    _contactDialog->addWidget(new Label("Address:"));
    _addrInput = new LineInput("");
    _contactDialog->addWidget(_addrInput);
    _contactDialog->sigAccepted.connect(sigc::mem_fun(this, &Contacts::handleDialogAccept));

    populate();
}

Contacts::~Contacts()
{
    delete _contactDialog;
}

void
Contacts::showContactDialog(const std::string& name, const std::string& addr, bool add)
{
    if (add)
    {
        _contactDialog->setTitle("Add Contact");
        _dialogAddMode = 1;
    } else
    {
        _contactDialog->setTitle("Edit Contact");
        _dialogAddMode = 0;
    }
    _nameInput->setText(name);
    _addrInput->setText(addr);
    _contactDialog->execute();
}

void
Contacts::populate()
{
    Contact* contact;
    ContactManager::ContactList contacts = _manager->list();
    for (ContactManager::ContactList::iterator it = contacts.begin(); it != contacts.end(); ++it)
    {
        contact = new Contact(it->name, it->addr);
        _contactsList->addItem(contact);
    }
}

void
Contacts::showAddContactDialog()
{
    showContactDialog("", "", true);
}

void
Contacts::showEditContactDialog()
{
    if (_contactsList->currentItem())
        showContactDialog(((Contact*) _contactsList->currentItem())->name(), ((Contact*) _contactsList->currentItem())->addr(), false);
}

void
Contacts::handleDialogAccept()
{
    if (_dialogAddMode)
    {
        if (_manager->add(_nameInput->text(), _addrInput->text()))
        {
            Contact* contact = new Contact(_nameInput->text(), _addrInput->text());
            _contactsList->addItem(contact);
        }
    } else
    {

    }
}

void
Contacts::removeContact()
{
    if (!_contactsList->currentItem())
        return;

    _manager->remove(((Contact*) _contactsList->currentItem())->addr());

    _contactsList->removeItem(_contactsList->currentItem());
    if (!_contactsList->count())
    {
        _remove->setDisabled();
        _edit->setDisabled();
        _call->setDisabled();
        _video->setDisabled();
        _contactsList->update();
    }
}

void
Contacts::callContact(bool withVideo)
{
    if (!_contactsList->currentItem())
        return;
    std::string name = ((Contact*) _contactsList->currentItem())->name();
    std::string addr = "<sip:";
    addr.append(((Contact*) _contactsList->currentItem())->addr());
    addr.append(">");
    sigCall(name, addr, withVideo);
}

void
Contacts::updateButtons(int oldItem, int newItem)
{
    if (_contactsList->currentItem())
    {
        Contact* previous = ((Contact*) _contactsList->itemAtIndex(oldItem));
        if (previous)
            previous->setSelected(false);

        if (_contactsList->currentItem())
            ((Contact*) _contactsList->currentItem())->setSelected(true);

        _remove->setEnabled();
        _edit->setEnabled();
        _call->setEnabled();
        _video->setEnabled();
    } else
    {
        _remove->setDisabled();
        _edit->setDisabled();
        _call->setDisabled();
        _video->setDisabled();
    }
}

} /* namespace ilixi */
