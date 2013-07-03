/*
 * Status.h
 *
 *  Created on: Jun 22, 2013
 *      Author: tarik
 */

#ifndef ILXSIP_STATUS_H_
#define ILXSIP_STATUS_H_

#include <ui/Frame.h>

namespace ilixi
{

class Status : public Frame
{
public:
    Status(Widget* parent = 0);

    virtual
    ~Status();

    void
    addMessage(const std::string& message);

private:
    ListBox* _list;
};

} /* namespace ilixi */
#endif /* ILXSIP_STATUS_H_ */
