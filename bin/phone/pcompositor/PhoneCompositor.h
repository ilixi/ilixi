#ifndef PHONECOMPOSITOR_H_
#define PHONECOMPOSITOR_H_

#include <compositor/Compositor.h>

namespace ilixi
{

class PhoneCompositor : public ILXCompositor
{
public:
    PhoneCompositor(int argc, char* argv[]);

    virtual
    ~PhoneCompositor();

private:
    void
    setUIGeomety();

    bool
    windowCustomEventFilter(const DFBWindowEvent& event);
};

} /* namespace ilixi */
#endif /* PHONECOMPOSITOR_H_ */
