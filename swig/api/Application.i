%module ilixi_Application

%import(module="ilixi_WindowWidget") <ui/WindowWidget.h>
%import(module="ilixi_Margin") <types/Margin.h>
%import(module="ilixi_LayoutBase") <ui/LayoutBase.h>
%import(module="ilixi_Widget") <ui/Widget.h>

%{
#include <ilixiGUI.h>
using namespace ilixi;
%}
%include <core/Application.h>
