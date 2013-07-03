/*
 * Settings.cpp
 *
 *  Created on: Jun 18, 2013
 *      Author: tarik
 */

#include "Settings.h"
#include <ui/Label.h>
#include <ui/Spacer.h>
#include <ui/TabPanel.h>
#include <ui/HBoxLayout.h>
#include <ui/VBoxLayout.h>

#include <core/Logger.h>
#include <lib/FileSystem.h>

#include <fstream>

namespace ilixi
{

D_DEBUG_DOMAIN(ILX_SIP_SETTINGS, "ilixi_sip/Settings", "Settings");

Settings::Settings(Widget* parent)
        : ContainerBase(parent)
{
    setLayout(new VBoxLayout());

    TabPanel* panel = new TabPanel();
    panel->setConstraints(ExpandingConstraint, ExpandingConstraint);
    addWidget(panel);

    VBoxLayout* general = new VBoxLayout();
    panel->addTab(general, "General");
    general->addWidget(new Label("Soon :)"));

    VBoxLayout* account = new VBoxLayout();
    panel->addTab(account, "Account");

    account->addWidget(new Label("Server:"));
    _server = new LineInput("");
    account->addWidget(_server);

    account->addWidget(new Label("Transport Protocol:"));
    _transport = new ComboBox("Transport Protocol");
    ComboBox::StringList list;
    list.push_back("udp");
    list.push_back("tcp");
    list.push_back("tls");
    _transport->addItems(list);
    account->addWidget(_transport);

    account->addWidget(new Label("User:"));
    _user = new LineInput("");
    account->addWidget(_user);

    account->addWidget(new Label("Password:"));
    _password = new LineInput("");
    account->addWidget(_password);
    account->addWidget(new Spacer(Vertical));

    HBoxLayout* buttons = new HBoxLayout();
    addWidget(buttons);
    buttons->addWidget(new Spacer(Horizontal));

    _apply = new PushButton("Apply");
    _apply->sigClicked.connect(sigc::mem_fun(this, &Settings::apply));
    buttons->addWidget(_apply);

    init();
}

Settings::~Settings()
{
}

void
Settings::compose(const PaintEvent& event)
{
}

void
Settings::init()
{
    ILOG_TRACE_F(ILX_SIP_SETTINGS);
    std::string accountsFile = PrintF("%s/.baresip/accounts", FileSystem::homeDirectory().c_str());
    if (!FileSystem::fileExists(accountsFile))
    {
        ILOG_FATAL(ILX_SIP_SETTINGS, "Account file not found: %s\n", accountsFile.c_str());
        return;
    }

    std::string line;
    std::ifstream infile;
    infile.open(accountsFile.c_str(), std::ifstream::in);
    while (!infile.eof())
    {
        getline(infile, line);
        if (line[0] == '#' || line[0] == ' ' || line.length() < 2)
            continue;

        std::size_t userPos = line.find_first_of(":", 5);
        std::string name = line.substr(5, userPos - 5);

        std::size_t passPos = line.find_first_of("@", 5);
        std::string password = line.substr(userPos + 1, passPos - userPos - 1);

        std::size_t serverPos = line.find_first_of(">;", 5);
        std::string server = line.substr(passPos + 1, serverPos - passPos - 1);

        if (serverPos < line.length() - 1)
        {
            std::size_t endPos = line.find_first_of(">", 5);
            std::size_t transportPos = line.find_first_of("=", 5);
            std::string transport = line.substr(transportPos + 1, endPos - transportPos - 1);

            if (transport == "udp")
                _transport->setSelected(0);
            else if (transport == "tcp")
                _transport->setSelected(1);
            else
                _transport->setSelected(2);
        }

        _user->setText(name);
        _password->setText(password);
        _server->setText(server);

        if (name.length() && password.length() && server.length())
            break;
    }
    infile.close();

}

void
Settings::apply()
{
    // TODO save accounts file.
}

} /* namespace ilixi */
