/*
 *
 */

#ifndef APP_H_
#define APP_H_

#include <core/Application.h>
#include <core/DaleDFB.h>
#include <ui/ToolBar.h>
#include <ui/ToolButton.h>
#include <ui/VBoxLayout.h>

#include "Call.h"
#include "Contacts.h"
#include "ContactManager.h"
#include "Dialer.h"
#include "History.h"
#include "Settings.h"
#include "Status.h"

namespace ilixi
{

class App : public Application
{
public:
    App(int argc, char* argv[]);

    virtual
    ~App();

    void
    runBaresip();

    void
    callContact(const std::string& name, const std::string& addr, bool video);

    void
    showScreen(int screen);

protected:
    virtual void
    handleUserEvent(const DFBUserEvent& event);

private:
    ContactManager* _manager;
    IComaComponent* _sipComponent;
    pid_t _pid;

    Dialog* _incomingCall;
    Label* _caller;

    // Views
    Call* _call;
    Contacts* _contacts;
    Dialer* _dialer;
    History* _history;
    Settings* _settings;
    Status* _status;
    VBoxLayout* _rightPanel;

    // Toolbar
    ToolButton* _baresip;
    ToolBarButton* _historyButton;
    ToolBarButton* _contactsButton;
    ToolBarButton* _settingsButton;

    ToolBarButton* _holdButton;
    ToolBarButton* _muteButton;
    ToolBarButton* _hangupButton;

    void
    acceptCall();

    void
    holdCall();

    void
    muteCall();

    void
    hangupCall();

    virtual bool
    windowPreEventFilter(const DFBWindowEvent& event);

    // friend callbacks
    friend void
    sipLog(void* ctx, void* arg);

    friend void
    sipError(void* ctx, void* arg);

    friend void
    sipVideo(void* ctx, void* arg);

    friend void
    sipContactStatus(void* ctx, void* arg);

    friend void
    sipRegistering(void* ctx, void* arg);

    friend void
    sipRegisterOK(void* ctx, void* arg);

    friend void
    sipRegisterFail(void* ctx, void* arg);

    friend void
    sipUnregistering(void* ctx, void* arg);

    friend void
    sipUnregisterOK(void* ctx, void* arg);

    friend void
    sipUnregisterFail(void* ctx, void* arg);

    friend void
    sipCallIncoming(void* ctx, void* arg);

    friend void
    sipCallRinging(void* ctx, void* arg);

    friend void
    sipCallProgress(void* ctx, void* arg);

    friend void
    sipCallEstablished(void* ctx, void* arg);

    friend void
    sipCallClosed(void* ctx, void* arg);
};

}

#endif // APP_H_
