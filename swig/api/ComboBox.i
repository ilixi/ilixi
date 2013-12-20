%module ilixi_ComboBox

%import(module="ilixi_Widget") <ui/Widget.h>
%import(module="ilixi_TextBase") <ui/TextBase.h>
%import(module="ilixi_sigc_signal") <sigc++/signal.h>
%import(module="ilixi_Size") <types/Size.h>

%include "std_string.i"

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

%template(ComboboxUIntSignal) sigc::signal<void, unsigned int>;

%extend ilixi::ComboBox {
  void connectSelectionChanged(DelegateNotify cb)
  {
    $self->sigSelectionChanged.connect(sigc::ptr_fun(cb));
  }
}

%include <ui/ComboBox.h>