/*
 * HistoryItem.h
 *
 *  Created on: Jul 1, 2013
 *      Author: tarik
 */

#ifndef ILXSIP_HISTORYITEM_H_
#define ILXSIP_HISTORYITEM_H_

#include <ui/Widget.h>

namespace ilixi
{

class HistoryItem : public Widget
{
public:
    HistoryItem(bool incoming, const std::string& name, const std::string& addr, Widget* parent = 0);

    virtual
    ~HistoryItem();

    Size
    preferredSize() const;

    const std::string&
    addr() const;

    void
    setAddr(const std::string& addr);

    const std::string&
    name() const;

    void
    setName(const std::string& name);

    const std::string&
    timestamp() const;

    void
    setTimestamp(const std::string& timestamp);

    bool
    isselected() const;

    void
    setSelected(bool selected);

protected:
    virtual void
    focusInEvent();

    virtual void
    focusOutEvent();

    virtual void
    compose(const PaintEvent& event);

private:
    bool _incoming;
    bool _selected;
    std::string _name;
    std::string _addr;
    std::string _timestamp;
    Image* _image;

};

} /* namespace ilixi */
#endif /* ILXSIP_HISTORYITEM_H_ */
