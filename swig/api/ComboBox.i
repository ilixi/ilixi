%module ilixi_ComboBox

%import(module="ilixi_Widget") <ui/Widget.h>
%import(module="ilixi_TextBase") <ui/TextBase.h>
%import(module="ilixi_sigc_signal") <sigc++/signal.h>

%include "std_string.i"

%{
#include <sigc++/signal.h>
#include <ilixiGUI.h>
using namespace sigc;
using namespace ilixi;
%}

%template(ComboboxUIntSignal) sigc::signal<void, unsigned int>;

%include <ui/ComboBox.h>