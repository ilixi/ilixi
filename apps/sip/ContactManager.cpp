/*
 * ContactManager.cpp
 *
 *  Created on: Jul 1, 2013
 *      Author: tarik
 */

#include "ContactManager.h"

#include <core/Logger.h>
#include <lib/FileSystem.h>

#include <fstream>

namespace ilixi
{

D_DEBUG_DOMAIN(ILX_SIP_CONTACTMANAGER, "ilixi_sip/ContactManager", "ContactManager");

ContactManager::ContactManager()
{
    parseContacts();
}

ContactManager::~ContactManager()
{
}

unsigned int
ContactManager::count() const
{
    return _list.size();
}

const ContactManager::ContactList&
ContactManager::list() const
{
    return _list;
}

bool
ContactManager::add(const std::string& name, const std::string& addr)
{
    ContactData data;
    snprintf(data.name, 128, "%s", name.c_str());
    snprintf(data.addr, 256, "%s", addr.c_str());
    _list.push_back(data);
    sigAdd(data);
    return true;
}

bool
ContactManager::remove(const std::string& addr)
{
    for (ContactList::iterator it = _list.begin(); it != _list.end(); ++it)
    {
        std::string uri = ((ContactData) *it).addr;
        if (addr.find(uri) != std::string::npos)
        {
            _list.erase(it);
            return true;
        }
    }
    return false;
}

std::string
ContactManager::getName(const std::string& addr)
{
    for (ContactList::iterator it = _list.begin(); it != _list.end(); ++it)
    {
        std::string uri = ((ContactData) *it).addr;
        if (addr.find(uri) != std::string::npos)
            return ((ContactData) *it).name;
    }
    return "Unknown";
}

std::string
ContactManager::getAddr(const std::string& name)
{
    for (ContactList::iterator it = _list.begin(); it != _list.end(); ++it)
    {
        std::string contact = ((ContactData) *it).name;
        if (name.find(contact) != std::string::npos)
            return ((ContactData) *it).addr;
    }
    return "Unknown";
}

void
ContactManager::writeContacts()
{
    ILOG_TRACE_F(ILX_SIP_CONTACTMANAGER);
    std::string contactsFile = PrintF("%s/.baresip/contacts", FileSystem::homeDirectory().c_str());
    for (ContactList::iterator it = _list.begin(); it != _list.end(); ++it)
    {

    }
}

void
ContactManager::parseContacts()
{
    ILOG_TRACE_F(ILX_SIP_CONTACTMANAGER);
    std::string contactsFile = PrintF("%s/.baresip/contacts", FileSystem::homeDirectory().c_str());
    if (!FileSystem::fileExists(contactsFile))
    {
        ILOG_FATAL(ILX_SIP_CONTACTMANAGER, "Contacts file not found: %s\n", contactsFile.c_str());
        return;
    }

    std::string line;
    std::ifstream infile;
    infile.open(contactsFile.c_str(), std::ifstream::in);
    while (!infile.eof())
    {
        getline(infile, line);
        if (line[0] == '#' || line[0] == ' ' || line.length() < 2)
            continue;

        std::size_t addrPos = line.find_first_of("\"", 1);
        std::string name = line.substr(1, addrPos - 1);
        std::string addr = line.substr(addrPos + 7, line.length() - addrPos - 8);

        add(name, addr);
    }
    infile.close();
    ILOG_DEBUG(ILX_SIP_CONTACTMANAGER, " -> Populated with %u entries.\n", _list.size());
}

} /* namespace ilixi */
