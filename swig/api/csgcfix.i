%typemap(csbody) SWIGTYPE %{
  // Store widgets added from C# so GC doesn't collect
  private static System.Collections.Generic.List<System.Object> $csclassnameRefs = new System.Collections.Generic.List<System.Object>();
  
  private System.Runtime.InteropServices.HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal $csclassname(System.IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new System.Runtime.InteropServices.HandleRef(this, cPtr);
    $csclassnameRefs.Add(this);
  }

  internal static System.Runtime.InteropServices.HandleRef getCPtr($csclassname obj) {
    return (obj == null) ? new System.Runtime.InteropServices.HandleRef(null, System.IntPtr.Zero) : obj.swigCPtr;
  }

  ~$csclassname() { 
    unref();
  }
    
  public void unref() {
    $csclassnameRefs.Remove(this);
    Dispose();
  }
%}

%typemap(csfinalize) SWIGTYPE %{
%}