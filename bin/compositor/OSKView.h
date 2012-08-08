/*
 * OSKView.h
 *
 *  Created on: Aug 8, 2012
 *      Author: tarik
 */

#ifndef OSKVIEW_H_
#define OSKVIEW_H_

#include "AppCompositor.h"

namespace ilixi
{

class OSKView : public AppCompositor
{
public:
    OSKView(Compositor* compositor, AppInstance* instance, Widget* parent =0);

    virtual
    ~OSKView();

    void
    show();

    void
    hide();


    virtual void
    madeAvailable();
};

} /* namespace ilixi */
#endif /* OSKVIEW_H_ */
