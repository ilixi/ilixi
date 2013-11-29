%module ilixi_ContainerBase

%import(module="ilixi_Widget") <ui/Widget.h>
%import(module="ilixi_LayoutBase") <ui/LayoutBase.h>

%{
#include <ilixiGUI.h>
using namespace ilixi;
%}
%include <ui/ContainerBase.h>
