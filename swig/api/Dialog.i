%module ilixi_Dialog

%import(module="ilixi_WindowWidget") <ui/WindowWidget.h>
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

%template(DialogVoidSignal) sigc::signal<void>;
%template(DialogVoidIntSignal) sigc::signal<void, int>;

%include "std_string.i"

%extend ilixi::Dialog {
  void connectAccept(DelegateNotify cb)
  {
    $self->sigAccepted.connect(sigc::ptr_fun(cb));
  }
}

%extend ilixi::Dialog {
  void connectReject(DelegateNotify cb)
  {
    $self->sigRejected.connect(sigc::ptr_fun(cb));
  }
}

%include <ui/Dialog.h>
