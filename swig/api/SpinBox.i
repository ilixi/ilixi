%module ilixi_SpinBox

%import(module="ilixi_Widget") <ui/Widget.h>
%import(module="ilixi_Enums") <types/Enums.h>
%import(module="ilixi_Size") <types/Size.h>

%include "std_string.i"

%{
#include <ilixiGUI.h>
using namespace ilixi;
%}

%include <ui/SpinBox.h>
