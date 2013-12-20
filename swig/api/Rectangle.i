%module ilixi_Rectangle

%import(module="ilixi_Widget") <types/Size.h>

%{
#include <ilixiGUI.h>
using namespace ilixi;
%}

%include "csgcfix.i"

%include <types/Rectangle.h>