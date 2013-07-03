/*
 * ContactManager.h
 *
 *  Created on: Jul 1, 2013
 *      Author: tarik
 */

#ifndef ILXSIP_CONTACTMANAGER_H_
#define ILXSIP_CONTACTMANAGER_H_

#include <baresip_dale.h>
#include <string>
#include <list>
#include <sigc++/signal.h>

namespace ilixi
{

class ContactManager
{
public:
    typedef std::list<ContactData> ContactList;

    ContactManager();

    virtual
    ~ContactManager();

    unsigned int
    count() const;

    const ContactList&
    list() const;

    bool
    add(const std::string& name, const std::string& addr);

    bool
    remove(const std::string& addr);

    std::string
    getName(const std::string& addr);

    std::string
    getAddr(const std::string& name);

    void
    writeContacts();

    sigc::signal<void, ContactData> sigAdd;
    sigc::signal<void, ContactData> sigRemove;

private:
    ContactList _list;

    void
    parseContacts();

};

} /* namespace ilixi */
#endif /* ILXSIP_CONTACTMANAGER_H_ */
