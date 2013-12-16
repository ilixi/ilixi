%module ilixi_Notify

%import(module="ilixi_sigc_signal") <sigc++/signal.h>

%include "std_string.i"

%{
#include <sigc++/signal.h>
#include <ilixiGUI.h>
#include <lib/Notify.h>
using namespace sigc;
using namespace ilixi;
%}

%template(NotifyVoidSignal) sigc::signal<void>;

%include <lib/Notify.h>