%module ilixi_CheckBox

%import(module="ilixi_Button") <ui/Button.h>
%import(module="ilixi_Widget") <ui/Widget.h>

%include "std_string.i"

%{
#include <ilixiGUI.h>
using namespace ilixi;
%}

%include <ui/CheckBox.h>
