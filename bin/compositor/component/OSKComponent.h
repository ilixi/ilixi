/*
 * OSKComponent.h
 *
 *  Created on: Aug 8, 2012
 *      Author: tarik
 */

#ifndef ILIXI_OSKCOMPONENT_H_
#define ILIXI_OSKCOMPONENT_H_

#include <core/ComponentData.h>
#include <core/ComaComponent.h>
#include <directfb.h>

namespace ilixi
{

class ILXCompositor;

class OSKComponent : public ilixi::ComaComponent
{
public:
    OSKComponent(ILXCompositor* compositor);

    virtual
    ~OSKComponent();

protected:
    virtual DirectResult
    comaMethod(ComaMethodID method, void *arg);

private:
    ILXCompositor* _compositor;
};

} /* namespace ilixi */
#endif /* ILIXI_OSKCOMPONENT_H_ */
