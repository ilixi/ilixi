/*
 * PopupComponent.h
 *
 *  Created on: Jul 24, 2012
 *      Author: tarik
 */

#ifndef ILIXI_POPUPCOMPONENT_H_
#define ILIXI_POPUPCOMPONENT_H_

#include <core/ComaComponent.h>

namespace ilixi
{

class Compositor;
class NotificationManager;

class PopupComponent : public ilixi::ComaComponent
{
public:
    enum PopupMethods
    {
        AddNotification, // e.g. song changed.
        AddOverlay, // No input
        AddDialog // Alerts with OK button.
    };

    PopupComponent(Compositor* compositor);

    virtual
    ~PopupComponent();

protected:
    virtual DirectResult
    comaMethod(ComaMethodID method, void *arg);

private:
    Compositor* _compositor;
    NotificationManager* _notificationMan;
};

} /* namespace ilixi */
#endif /* ILIXI_POPUPCOMPONENT_H_ */
