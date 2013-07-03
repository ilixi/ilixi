/*
 * Contact.h
 *
 *  Created on: Jul 1, 2013
 *      Author: tarik
 */

#ifndef ILXSIP_CONTACT_H_
#define ILXSIP_CONTACT_H_

#include <ui/Widget.h>

namespace ilixi
{

class Contact : public Widget
{
public:
    enum ContactStatus
    {
        Online,
        Offline,
        Busy,
        Away
    };

    Contact(const std::string& name, const std::string& addr, Widget* parent = 0);

    virtual
    ~Contact();

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

    ContactStatus
    status() const;

    void
    setStatus(ContactStatus status);

    void
    setSelected(bool selected);

    sigc::signal<void> sigClicked;

protected:
    virtual void
    compose(const PaintEvent& event);

    virtual void
    keyDownEvent(const KeyEvent& event);

    virtual void
    pointerButtonDownEvent(const PointerEvent& event);

    virtual void
    pointerButtonUpEvent(const PointerEvent& event);

    virtual void
    enterEvent(const PointerEvent& event);

    virtual void
    leaveEvent(const PointerEvent& event);

    virtual void
    focusInEvent();

    virtual void
    focusOutEvent();

private:
    bool _selected;
    bool _pressed;
    ContactStatus _status;
    std::string _name;
    std::string _addr;
};

} /* namespace ilixi */
#endif /* ILXSIP_CONTACT_H_ */
