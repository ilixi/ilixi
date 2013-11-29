%module ilixi_ToolButton

%import(module="ilixi_Button") <ui/Button.h>

%include "std_string.i"

%{
#include <ilixiGUI.h>
using namespace ilixi;
%}
%include <ui/ToolButton.h>
