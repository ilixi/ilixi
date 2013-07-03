/*
 * History.h
 *
 *  Created on: Jul 1, 2013
 *      Author: tarik
 */

#ifndef ILXSIP_HISTORY_H_
#define ILXSIP_HISTORY_H_

#include <ui/Frame.h>
#include <ui/ListBox.h>
#include <ui/ButtonGroup.h>

#include "ContactManager.h"

namespace ilixi
{

class History : public Frame
{
public:
    History(Widget* parent = 0);

    virtual
    ~History();

    void
    clear();

    void
    add(bool incoming, const std::string& name, const std::string& addr);

    sigc::signal<void, const std::string&, const std::string&, bool> sigCall;

private:
    ListBox* _history;

    DirectionalButton* _clearAll;
    DirectionalButton* _remove;

    DirectionalButton* _txt;
    DirectionalButton* _call;
    DirectionalButton* _video;

    void
    remove();

    void
    callContact(bool withVideo);

    void
    updateButtons(int oldItem, int newItem);
};

} /* namespace ilixi */
#endif /* ILXSIP_HISTORY_H_ */
