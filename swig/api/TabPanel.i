%module ilixi_TabPanel

%import(module="ilixi_Frame") <ui/Frame.h>
%import(module="ilixi_Button") <ui/Button.h>
%import(module="ilixi_Widget") <ui/Widget.h>
%import(module="ilixi_Size") <types/Size.h>

%include "std_string.i"

%{
#include <ilixiGUI.h>
using namespace ilixi;
%}

%include <ui/TabPanel.h>
