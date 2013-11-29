%module ilixi_Frame

%import(module="ilixi_ContainerBase") <ui/ContainerBase.h>
%import(module="ilixi_Margin") <types/Margin.h>
%import(module="ilixi_Widget") <ui/Widget.h>

%{
#include <ilixiGUI.h>
using namespace ilixi;
%}
%include <ui/Frame.h>
