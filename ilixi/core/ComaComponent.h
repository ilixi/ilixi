/*
 * ComaComponent.h
 *
 *  Created on: Jul 23, 2012
 *      Author: tarik
 */

#ifndef ILIXI_COMACOMPONENT_H_
#define ILIXI_COMACOMPONENT_H_

#include <string>
extern "C"
{
#include <fusiondale.h>
}

namespace ilixi
{

class ComaComponent
{
public:
    ComaComponent(const std::string& name, int numNotifications = 0);

    virtual
    ~ComaComponent();

    std::string
    name() const;

protected:
    DirectResult
    allocate(int bytes, void** arg);

    DirectResult
    deallocate(void* ptr);

    void
    init();

    void
    notify(ComaNotificationID id, void* arg);

    virtual DirectResult
    comaMethod(ComaMethodID method, void* arg);

    void
    createNotification(ComaNotificationID id, ComaNotifyFunc func);

private:
    std::string _name;
    IComaComponent* _component;

    friend void
    ComaComponentMethod(void *ctx, ComaMethodID method, void *arg,
            unsigned int magic);
};

} /* namespace ilixi */
#endif /* ILIXI_COMACOMPONENT_H_ */
