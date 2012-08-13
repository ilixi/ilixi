/*
 * OSKComponent.h
 *
 *  Created on: Aug 8, 2012
 *      Author: tarik
 */

#ifndef ILIXI_OSKCOMPONENT_H_
#define ILIXI_OSKCOMPONENT_H_

#include <core/ComaComponent.h>
#include <directfb.h>

namespace ilixi
{

class Compositor;

class OSKComponent : public ilixi::ComaComponent
{
    enum OSKMethods
    {
        ShowOSK = 0,
        HideOSK = 1,
        ConsumeKey = 2
    };

    enum LayoutMode
    {
        Standard, Numeric, Internet
    };

    struct OSKRequest
    {
        DFBRectangle inputRect;
        LayoutMode mode;
        pid_t process;
    };

public:
    OSKComponent(Compositor* compositor);

    virtual
    ~OSKComponent();

protected:
    virtual DirectResult
    comaMethod(ComaMethodID method, void *arg);

private:
    Compositor* _compositor;
};

} /* namespace ilixi */
#endif /* ILIXI_OSKCOMPONENT_H_ */
