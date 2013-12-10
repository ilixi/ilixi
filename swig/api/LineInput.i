%module ilixi_LineInput

%import(module="ilixi_Widget") <ui/Widget.h>
%import(module="ilixi_TextBase") <ui/TextBase.h>
%import(module="ilixi_sigc_signal") <sigc++/signal.h>

%include "std_string.i"

%{
#include <sigc++/signal.h>
#include <ilixiGUI.h>
using namespace sigc;
using namespace ilixi;
typedef void (SWIGSTDCALL *DelegateNotify)(void);
%}

%define %cs_callback(TYPE, CSTYPE)
  %typemap(ctype) TYPE, TYPE& "void*"
  %typemap(in) TYPE  %{ $1 = ($1_type)$input; %}
  %typemap(in) TYPE& %{ $1 = ($1_type)&$input; %}
  %typemap(imtype, out="IntPtr") TYPE, TYPE& "CSTYPE"
  %typemap(cstype, out="IntPtr") TYPE, TYPE& "CSTYPE"
  %typemap(csin) TYPE, TYPE& "$csinput"
%enddef

%cs_callback(DelegateNotify, DelegateNotify)

%template(LineInputVoidSignal) sigc::signal<void>;
%template(LineInputIntIntSignal) sigc::signal<void, int, int>;
%template(LineInputStringSignal) sigc::signal<void, const std::string&>;

%extend ilixi::LineInput {
  void connectTextEdited(DelegateNotify cb)
  {
    $self->sigTextEdited.connect(sigc::ptr_fun(cb));
  }
}

%extend ilixi::LineInput {
  void setText(std::string text)
  {
    $self->setText(text);
  }
}

%extend ilixi::LineInput {
  std::string getText()
  {
    $self->text();
  }
}

%include <ui/LineInput.h>
