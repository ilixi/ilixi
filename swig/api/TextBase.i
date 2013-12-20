%module ilixi_TextBase

%import(module="ilixi_Font") <types/Font.h>

%{
#include <ilixiGUI.h>
using namespace ilixi;
%}

%include "csgcfix.i"

%include <ui/TextBase.h>
