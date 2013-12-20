%module ilixi_ToolButton

%import(module="ilixi_Button") <ui/Button.h>
%import(module="ilixi_Size") <types/Size.h>
%import(module="ilixi_Enums") <types/Enums.h>

%include "std_string.i"

%{
#include <ilixiGUI.h>
using namespace ilixi;
%}
  
%include <ui/ToolButton.h>
