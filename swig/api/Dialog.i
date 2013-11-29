%module ilixi_Dialog

%import(module="ilixi_WindowWidget") <ui/WindowWidget.h>
%import(module="ilixi_sigc_signal") <sigc++/signal.h>

%runtime %{
#include <sigc++/signal.h>
#include <ilixiGUI.h>
using namespace sigc;
using namespace ilixi;
%}

%template(DialogVoidSignal) sigc::signal<void>;
%template(DialogVoidIntSignal) sigc::signal<void, int>;

%include "std_string.i"

%include <ui/Dialog.h>
