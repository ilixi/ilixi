/*
 * DaleDFB.h
 *
 *  Created on: Aug 27, 2012
 *      Author: tarik
 */

#ifndef ILIXI_DALEDFB_H_
#define ILIXI_DALEDFB_H_

#include <directfb.h>
extern "C"
{
#include <fusiondale.h>
}
#include <types/Rectangle.h>

namespace ilixi
{

class DaleDFB
{
    friend class AppBase;
public:

    /*!
     * Returns Coma interface.
     */
    static IComa*
    getComa();

    static DFBResult
    comaGetComponent(const char* name, IComaComponent** component);

    static DFBResult
    comaGetLocal(unsigned int bytes, void** ret);

    static DFBResult
    comaCallComponent(IComaComponent* component, ComaMethodID method,
                      void* arg);

    static DFBResult
    showOSK(const Rectangle& rect);

    static DFBResult
    hideOSK();

private:
    static IFusionDale* __dale;
    static IComa* __coma;
    static IComaComponent* __oskComp;

    struct OSKRequest
    {
        DFBRectangle inputRect;
        int mode;
        pid_t process;
    };

    DaleDFB();

    virtual
    ~DaleDFB();

    static DFBResult
    initDale(int* argc, char*** argv);

    static void
    releaseDale();

    static DFBResult
    getOSKComp();

};

} /* namespace ilixi */
#endif /* DALEDFB_H_ */
