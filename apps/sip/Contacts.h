/*
 * Contacts.h
 *
 *  Created on: Jun 19, 2013
 *      Author: tarik
 */

#ifndef ILXSIP_CONTACTS_H_
#define ILXSIP_CONTACTS_H_

#include <ui/ButtonGroup.h>
#include <ui/Dialog.h>
#include <ui/LineInput.h>
#include <ui/ListBox.h>
#include <ui/Frame.h>

#include "Contact.h"
#include "ContactManager.h"

namespace ilixi
{


class Contacts : public Frame
{
public:
    Contacts(ContactManager* manager, Widget* parent = 0);

    virtual
    ~Contacts();

    void
    showContactDialog(const std::string& name = "", const std::string& addr = "", bool add = false);

    sigc::signal<void, const std::string&, const std::string&, bool> sigCall;

private:
    ContactManager* _manager;

    bool _dialogAddMode;

    Dialog* _contactDialog;
    ListBox* _contactsList;

    LineInput* _nameInput;
    LineInput* _addrInput;

    DirectionalButton* _add;
    DirectionalButton* _remove;
    DirectionalButton* _edit;

    DirectionalButton* _txt;
    DirectionalButton* _call;
    DirectionalButton* _video;

    void
    populate();

    void
    showAddContactDialog();

    void
    showEditContactDialog();

    void
    handleDialogAccept();

    void
    removeContact();

    void
    callContact(bool withVideo);

    void
    updateButtons(int oldItem, int newItem);
};

} /* namespace ilixi */
#endif /* ILXSIP_CONTACTS_H_ */
