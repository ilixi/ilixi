%module ilixi_Button

%import(module="ilixi_Widget") <ui/Widget.h>
%import(module="ilixi_TextBase") <ui/TextBase.h>
%import(module="ilixi_sigc_signal") <sigc++/signal.h>

%runtime %{
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

%template(ButtonVoidSignal) sigc::signal<void>;
%template(ButtonVoidBoolSignal) sigc::signal<void, bool>;

%pragma(csharp) moduleimports=%{
public delegate void DelegateNotify();
%}

%extend sigc::signal<void> {
  void connect(ilixi::Button *button, DelegateNotify cb)
  {
    button->sigClicked.connect(sigc::ptr_fun(cb));
  }
}


%include <ui/Button.h>
