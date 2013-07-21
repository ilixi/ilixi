#include "App.h"
#include <ui/HBoxLayout.h>
#include <ui/VBoxLayout.h>
#include <ui/Spacer.h>
#include <core/Engine.h>
#include <core/Logger.h>
#include <core/PlatformManager.h>

#include <baresip_dale.h>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_SIPAPP, "ilixi_sip", "ilixi_sip");

//-------------------------------------------------------------------------------------------------
// Callbacks
//-------------------------------------------------------------------------------------------------
void
sipLog(void* ctx, void* arg)
{
    App* app = (App*) ctx;
    MessageData data;
    data = *((MessageData*) arg);
    app->_status->addMessage(data.body);
}

void
sipError(void* ctx, void* arg)
{
    App* app = (App*) ctx;
    MessageData data;
    data = *((MessageData*) arg);
    app->_status->addMessage(data.body);
}

void
sipVideo(void* ctx, void* arg)
{
    App* app = (App*) ctx;
    MessageData data;
    data = *((MessageData*) arg);
    app->_status->addMessage(PrintF("Attaching video surface [%u]", data.type));
    app->_call->setVideoSurfaceID(data.type);
}

void
sipContactStatus(void* ctx, void* arg)
{
    App* app = (App*) ctx;
    MessageData data;
    data = *((MessageData*) arg);
    app->_status->addMessage(PrintF("Contact [%s] status [%u]", data.body, data.type));
}

void
sipRegistering(void* ctx, void* arg)
{
    App* app = (App*) ctx;
    app->_baresip->setText("Registering");
    app->_status->addMessage("Registering to server...");
}

void
sipRegisterOK(void* ctx, void* arg)
{
    App* app = (App*) ctx;
    app->_baresip->setText("Online");
    app->_status->addMessage("Online!");
}

void
sipRegisterFail(void* ctx, void* arg)
{
    App* app = (App*) ctx;
    app->_baresip->setText("Offline");
    app->_status->addMessage("Registration failed!");
}

void
sipUnregistering(void* ctx, void* arg)
{
    App* app = (App*) ctx;
    app->_status->addMessage("Unregistering from server...");
}

void
sipUnregisterOK(void* ctx, void* arg)
{
    App* app = (App*) ctx;
    app->_baresip->setText("Offline");
    app->_status->addMessage("Offline");
}

void
sipUnregisterFail(void* ctx, void* arg)
{
    App* app = (App*) ctx;
    app->_status->addMessage("Unregistration failed!");
}

void
sipCallIncoming(void* ctx, void* arg)
{
    App* app = (App*) ctx;
    MessageData data;
    data = *((MessageData*) arg);
    Engine::instance().postUserEvent(BSN_CALL_INCOMING, NULL);
    app->_caller->setText(data.body);
    app->_status->addMessage(PrintF("Incoming call %s!", data.body));
}

void
sipCallRinging(void* ctx, void* arg)
{
    App* app = (App*) ctx;
    app->_status->addMessage("Ringing...");
    app->_call->setStatus(Call::Ringing);
}

void
sipCallProgress(void* ctx, void* arg)
{
    App* app = (App*) ctx;
    app->_status->addMessage("Contact has a call in progress.");
}

void
sipCallEstablished(void* ctx, void* arg)
{
    App* app = (App*) ctx;
    app->_status->addMessage("Call established.");
    app->_call->setStatus(Call::Established);
}

void
sipCallClosed(void* ctx, void* arg)
{
    App* app = (App*) ctx;
    app->_status->addMessage("Call ended.");
    app->_settingsButton->setVisible(true);
    app->_call->setStatus(Call::Ended);
    app->showScreen(0);
}

//-------------------------------------------------------------------------------------------------
// Main application
//-------------------------------------------------------------------------------------------------
App::App(int argc, char* argv[])
        : Application(&argc, &argv, OptDaleAuto),
          _sipComponent(NULL),
          _pid(0)
{
    setLayout(new HBoxLayout());
    setMargins(5, 5, 5, 5);

    //-------------------------------------------------------------------------------------------------
    // Create toolbar
    //-------------------------------------------------------------------------------------------------
    ToolBar* bar = new ToolBar();
    setToolbar(bar);

    _baresip = new ToolButton("Offline");
    _baresip->setIcon(StyleHint::Network);
    _baresip->setInputMethod(NoInput);
    _baresip->setDrawFrame(false);
    bar->addWidget(_baresip);

    bar->addWidget(new Spacer(Horizontal));

    _contactsButton = new ToolBarButton("Contacts");
    _contactsButton->setIcon(StyleHint::Contacts2);
    _contactsButton->sigClicked.connect(sigc::bind<int>(sigc::mem_fun(this, &App::showScreen), 0));
    bar->addWidget(_contactsButton);

    _historyButton = new ToolBarButton("Call Log");
    _historyButton->setIcon(StyleHint::Contacts1);
    _historyButton->sigClicked.connect(sigc::bind<int>(sigc::mem_fun(this, &App::showScreen), 1));
    bar->addWidget(_historyButton);

    _settingsButton = new ToolBarButton("Settings");
    _settingsButton->setIcon(StyleHint::Settings);
    _settingsButton->sigClicked.connect(sigc::bind<int>(sigc::mem_fun(this, &App::showScreen), 2));
    bar->addWidget(_settingsButton);

    _holdButton = new ToolBarButton("Hold");
    _holdButton->setVisible(false);
    _holdButton->setIcon(StyleHint::Pause);
    _holdButton->sigClicked.connect(sigc::mem_fun(this, &App::holdCall));
    bar->addWidget(_holdButton);

    _muteButton = new ToolBarButton("Mute");
    _muteButton->setVisible(false);
    _muteButton->setIcon(StyleHint::VolumeMuted);
    _muteButton->sigClicked.connect(sigc::mem_fun(this, &App::muteCall));
    bar->addWidget(_muteButton);

    _hangupButton = new ToolBarButton("Hang Up");
    _hangupButton->setVisible(false);
    _hangupButton->setIcon(StyleHint::Hangup);
    _hangupButton->sigClicked.connect(sigc::mem_fun(this, &App::hangupCall));
    bar->addWidget(_hangupButton);

    //-------------------------------------------------------------------------------------------------
    // Create views
    //-------------------------------------------------------------------------------------------------
    _manager = new ContactManager();

    _contacts = new Contacts(_manager);
    _contacts->sigCall.connect(sigc::mem_fun(this, &App::callContact));
    addWidget(_contacts);

    _history = new History();
    _history->sigCall.connect(sigc::mem_fun(this, &App::callContact));
    _history->setVisible(false);
    addWidget(_history);

    _settings = new Settings();
    _settings->setVisible(false);
    addWidget(_settings);

    _call = new Call();
    _call->setVisible(false);
    _call->sigHangup.connect(sigc::mem_fun(this, &App::hangupCall));
    addWidget(_call);

    _rightPanel = new VBoxLayout();
    addWidget(_rightPanel);

    _dialer = new Dialer();
    _rightPanel->addWidget(_dialer);

    _status = new Status();
    _rightPanel->addWidget(_status);

    //-------------------------------------------------------------------------------------------------
    // Incoming call dialog
    //-------------------------------------------------------------------------------------------------
    _incomingCall = new Dialog("Incoming Call", Dialog::YesNoButtonOption);
    _incomingCall->setLayout(new VBoxLayout());
    _caller = new Label("");
    Size s = _caller->preferredSize();
    _caller->setMinimumSize(PlatformManager::instance().getLayerSize().width() - 200, s.height());
    _incomingCall->addWidget(_caller);
    _incomingCall->addWidget(new Label("Do you wish to accept?"));
    _incomingCall->sigAccepted.connect(sigc::mem_fun(this, &App::acceptCall));
    _incomingCall->sigRejected.connect(sigc::mem_fun(this, &App::hangupCall));

    sigVisible.connect(sigc::mem_fun(this, &App::runBaresip));
}

App::~App()
{
    delete _incomingCall;
    if (_sipComponent)
    {
//        DaleDFB::comaCallComponent(_sipComponent, BSM_QUIT, NULL);
        _sipComponent->Release(_sipComponent);
    }

    if (_pid)
        kill(_pid, SIGKILL);
}

void
App::runBaresip()
{
    ILOG_TRACE_F(ILX_SIPAPP);

    if (_pid)
    {
        _status->addMessage(PrintF("baresip quitting..."));
        DaleDFB::comaCallComponent(_sipComponent, BSM_QUIT, NULL);
        sleep(1);
        kill(_pid, SIGKILL);
    }

    pid_t pid = vfork();
    switch (pid)
    {
    case -1:
        perror("vfork");
        ILOG_FATAL(ILX_SIPAPP, "vfork error!\n");
        break;

    case 0:
        setsid();
        const char* args[2];
        args[0] = "baresip";
        args[1] = NULL;
        execvp(args[0], (char**) args);
        perror("execvp");
        _exit(0);
        break;

    default:
        _pid = pid;

        _status->addMessage(PrintF("baresip [%d] is running.", _pid));

        DaleDFB::comaGetComponent("baresip", &_sipComponent);

        _sipComponent->Listen(_sipComponent, BSN_DEBUG, sipLog, this);
        _sipComponent->Listen(_sipComponent, BSN_ERROR, sipError, this);

        _sipComponent->Listen(_sipComponent, BSN_VIDEO_SURFACE, sipVideo, this);
        _sipComponent->Listen(_sipComponent, BSN_CONTACT_STATUS, sipContactStatus, this);

        _sipComponent->Listen(_sipComponent, BSN_REGISTERING, sipRegistering, this);
        _sipComponent->Listen(_sipComponent, BSN_REGISTER_OK, sipRegisterOK, this);
        _sipComponent->Listen(_sipComponent, BSN_REGISTER_FAIL, sipRegisterFail, this);

        _sipComponent->Listen(_sipComponent, BSN_UNREGISTERING, sipRegistering, this);
        _sipComponent->Listen(_sipComponent, BSN_UNREGISTER_OK, sipRegisterOK, this);
        _sipComponent->Listen(_sipComponent, BSN_UNREGISTER_FAIL, sipRegisterFail, this);

        _sipComponent->Listen(_sipComponent, BSN_CALL_INCOMING, sipCallIncoming, this);
        _sipComponent->Listen(_sipComponent, BSN_CALL_RINGING, sipCallRinging, this);
        _sipComponent->Listen(_sipComponent, BSN_CALL_PROGRESS, sipCallProgress, this);
        _sipComponent->Listen(_sipComponent, BSN_CALL_ESTABLISHED, sipCallEstablished, this);
        _sipComponent->Listen(_sipComponent, BSN_CALL_CLOSED, sipCallClosed, this);
        break;
    }
}

void
App::callContact(const std::string& name, const std::string& addr, bool video)
{
    if (video)
        _status->addMessage(PrintF("Video calling %s.", name.c_str()));
    else
        _status->addMessage(PrintF("Calling %s.", name.c_str()));
    _call->setStatus(Call::Connecting);
    _call->setContact(name);
    _history->add(false, name, addr);
    showScreen(3);

    if (_sipComponent)
    {
        void *ptr;
        DaleDFB::comaGetLocal(sizeof(MessageData), &ptr);
        MessageData* data = (MessageData*) ptr;
        data->type = (unsigned int) video;
        snprintf(data->body, 512, "%s", addr.c_str());
        DaleDFB::comaCallComponent(_sipComponent, BSM_CONTACT_DIAL, (void*) data);
    }
}

void
App::handleUserEvent(const DFBUserEvent& event)
{
    if (event.type == BSN_CALL_INCOMING)
    {
        _incomingCall->execute();
        _call->setContact(_caller->text());
        _history->add(true, _manager->getName(_caller->text()), _caller->text());
    }
}

void
App::showScreen(int screen)
{
    if (screen == 0)
    {
        // Contacts
        _rightPanel->setVisible(true);
        _contacts->setVisible(true);
        _call->setVisible(false);
        _history->setVisible(false);
        _settings->setVisible(false);

        // Buttons
        _contactsButton->setVisible(true);
        _historyButton->setVisible(true);
        _settingsButton->setVisible(true);

        _holdButton->setVisible(false);
        _muteButton->setVisible(false);
        _hangupButton->setVisible(false);

        _status->update();
    } else if (screen == 1)
    {
        // Call history
        _rightPanel->setVisible(true);
        _contacts->setVisible(false);
        _call->setVisible(false);
        _history->setVisible(true);
        _settings->setVisible(false);
        _status->update();
    } else if (screen == 2)
    {
        // Settings
        _rightPanel->setVisible(false);
        _contacts->setVisible(false);
        _call->setVisible(false);
        _history->setVisible(false);
        _settings->setVisible(true);
    } else if (screen == 3)
    {
        // Call
        _rightPanel->setVisible(false);
        _contacts->setVisible(false);
        _call->setVisible(true);
        _history->setVisible(false);
        _settings->setVisible(false);

        // Buttons
        _contactsButton->setVisible(false);
        _historyButton->setVisible(false);
        _settingsButton->setVisible(false);

        _holdButton->setVisible(true);
        _muteButton->setVisible(true);
        _hangupButton->setVisible(true);
    }
}

void
App::acceptCall()
{
    showScreen(3);
    if (_sipComponent)
        DaleDFB::comaCallComponent(_sipComponent, BSM_CALL_ACCEPT, NULL);
}

void
App::holdCall()
{
    if (_call->holdFlag())
    {
        _holdButton->setIcon(StyleHint::Play);
        _holdButton->setText("Resume");
    } else
    {
        _holdButton->setIcon(StyleHint::Pause);
        _holdButton->setText("Hold");
    }
    _call->setStatus(Call::Hold);

    if (_sipComponent)
    {
        void *ptr;
        DaleDFB::comaGetLocal(sizeof(MessageData), &ptr);
        MessageData* data = (MessageData*) ptr;
        data->type = (unsigned int) !_call->holdFlag();

        DaleDFB::comaCallComponent(_sipComponent, BSM_CALL_HOLD, (void*) data);
    }
}

void
App::muteCall()
{
    if (_call->muteFlag())
    {
        _muteButton->setIcon(StyleHint::Volume2);
        _muteButton->setText("Unmute");
    } else
    {
        _muteButton->setIcon(StyleHint::VolumeMuted);
        _muteButton->setText("Mute");
    }
    _call->setStatus(Call::Mute);

    if (_sipComponent)
    {
        void *ptr;
        DaleDFB::comaGetLocal(sizeof(MessageData), &ptr);
        MessageData* data = (MessageData*) ptr;
        data->type = (unsigned int) !_call->muteFlag();

        DaleDFB::comaCallComponent(_sipComponent, BSM_CALL_MUTE, (void*) data);
    }
}

void
App::hangupCall()
{
    showScreen(0);
    _status->addMessage("Call terminated.");
    _call->setStatus(Call::Ended);
    if (_sipComponent)
        DaleDFB::comaCallComponent(_sipComponent, BSM_CALL_HANGUP, NULL);
}

bool
App::windowPreEventFilter(const DFBWindowEvent& event)
{
    return _dialer->consumeKey(event);
}

} // namespace ilixi

int
main(int argc, char *argv[])
{
    ilixi::App app(argc, argv);
    app.exec();
    return 0;
}
