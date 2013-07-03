/*
 * Status.cpp
 *
 *  Created on: Jun 22, 2013
 *      Author: tarik
 */

#include "Status.h"
#include <ui/Label.h>
#include <ui/ListBox.h>
#include <ui/PushButton.h>
#include <ui/VBoxLayout.h>

namespace ilixi
{

Status::Status(Widget* parent)
        : Frame(parent)

{
    setConstraints(NoConstraint, NoConstraint);
    setDrawFrame(false);
    setLayout(new VBoxLayout());
    _list = new ListBox();
    _list->setDrawFrame(true);
    addWidget(_list);
}

Status::~Status()
{
}

void
Status::addMessage(const std::string& message)
{
    if (_list->count() > 20)
        _list->removeItem(_list->itemAtIndex(0));

    struct timeval tv;
    struct tm* tm;
    gettimeofday(&tv, NULL);
    tm = localtime(&tv.tv_sec);

    char time[9];
    sprintf(time, "%02d:%02d:%02d", tm->tm_hour, tm->tm_min, tm->tm_sec);

    Label* item = new Label(PrintF("[%s]  %s", time, message.c_str()), _list);
    item->setYConstraint(FixedConstraint);
    if (_list->currentItem())
        item->moveTo(_list->currentItem()->x(), _list->currentItem()->y() + 2 * _list->currentItem()->height());
    item->setSize(item->preferredSize());
    _list->addItem(item);
    _list->setCurrentItem(item);
}

} /* namespace ilixi */
