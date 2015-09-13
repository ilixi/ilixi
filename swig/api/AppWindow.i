%module ilixi_AppWindow

%import(module="ilixi_WindowWidget") <ui/WindowWidget.h>

%{
#include <ilixiGUI.h>
using namespace ilixi;
%}

%include <ui/AppWindow.h>
