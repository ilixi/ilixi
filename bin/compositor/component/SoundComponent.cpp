/*
 * SoundComponent.cpp
 *
 *  Created on: Jul 23, 2012
 *      Author: tarik
 */

#include "SoundComponent.h"
#include <core/Logger.h>

D_DEBUG_DOMAIN( ILX_SOUNDCOMP, "ilixi/SoundComponent", "SoundComponent");

namespace ilixi
{

SoundComponent::SoundComponent()
        : ComaComponent("SoundComponent", SoundCompNumNotifications), _volume(
                50)
{
    init();
    createNotification(0, NULL);
    createNotification(1, NULL);
    createNotification(2, NULL);
}

SoundComponent::~SoundComponent()
{
}

DirectResult
SoundComponent::comaMethod(ComaMethodID method, void *arg)
{
    DirectResult ret = DR_OK;
    int volume = *((int*) arg);
    switch (method)
    {
    case SetVolume:
        ILOG_DEBUG(ILX_SOUNDCOMP, "SetVolume\n");
        setVolume(volume);
        break;

    case Mute:
        ILOG_DEBUG(ILX_SOUNDCOMP, "Mute\n");
        setVolume(0);
        break;

    case UnMute:
        ILOG_DEBUG(ILX_SOUNDCOMP, "UnMute\n");
        setVolume(50);
        break;

    default:
        ret = DR_NOIMPL;
        break;
    }
    return ret;
}

void
SoundComponent::setVolume(unsigned int volume)
{
    _volume = volume;
    int* vol;
    allocate(sizeof(vol), (void**) &vol);
    *vol = volume;
    notify(0, vol);
    ILOG_DEBUG(ILX_SOUNDCOMP, "Volume is %d\n", _volume);
    deallocate((void*) vol);
}

void
SoundComponent::VolumeChangedDispatchCallback(void *ctx,
        ComaNotificationID notification, void *arg)
{
    ILOG_DEBUG(ILX_SOUNDCOMP,
            "%s( %p, %lu, %p ) called!\n", __FUNCTION__, ctx, notification, arg);
}

void
SoundComponent::MutedDispatchCallback(void *ctx,
        ComaNotificationID notification, void *arg)
{
    ILOG_DEBUG(ILX_SOUNDCOMP,
            "%s( %p, %lu, %p ) called!\n", __FUNCTION__, ctx, notification, arg);
}

void
SoundComponent::UnMutedDispatchCallback(void *ctx,
        ComaNotificationID notification, void *arg)
{
    ILOG_DEBUG(ILX_SOUNDCOMP,
            "%s( %p, %lu, %p ) called!\n", __FUNCTION__, ctx, notification, arg);
}

} /* namespace ilixi */
