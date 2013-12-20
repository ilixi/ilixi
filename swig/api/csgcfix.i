%typemap(csbody) SWIGTYPE %{
  // Store widgets added from C# so GC doesn't collect
  private static System.Collections.Generic.List<Object> $csclassnameRefs = new System.Collections.Generic.List<Object>();
  
  private HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal $csclassname(IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new HandleRef(this, cPtr);
    $csclassnameRefs.Add(this);
  }

  internal static HandleRef getCPtr($csclassname obj) {
    return (obj == null) ? new HandleRef(null, IntPtr.Zero) : obj.swigCPtr;
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