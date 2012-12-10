/*
 * CarCompositor.h
 *
 *  Created on: Dec 4, 2012
 *      Author: tarik
 */

#ifndef CARCOMPOSITOR_H_
#define CARCOMPOSITOR_H_

#include <compositor/Compositor.h>

namespace ilixi
{

class CarCompositor : public ILXCompositor
{
public:
    CarCompositor(int argc, char* argv[]);

    virtual
    ~CarCompositor();
};

} /* namespace ilixi */
#endif /* CARCOMPOSITOR_H_ */
