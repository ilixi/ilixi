%module ilixi_LayoutBase

%import(module="ilixi_Widget") <ui/Widget.h>
%import(module="ilixi_Size") <types/Size.h>

%{
#include <ilixiGUI.h>
using namespace ilixi;
%}

%include <ui/LayoutBase.h>