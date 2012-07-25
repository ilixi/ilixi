/*
 * SoundComponent.h
 *
 *  Created on: Jul 23, 2012
 *      Author: tarik
 */

#ifndef SOUNDCOMPONENT_H_
#define SOUNDCOMPONENT_H_

#include <core/ComaComponent.h>

namespace ilixi
{

class SoundComponent : public ComaComponent
{
public:
    enum SoundCompMethods
    {
        SetVolume, Mute, UnMute
    };

    enum SoundCompNotifications
    {
        VolumeChanged, Muted, UnMuted, SoundCompNumNotifications
    };

    SoundComponent();

    virtual
    ~SoundComponent();

    void
    setVolume(unsigned int volume);

protected:
    virtual DirectResult
    comaMethod(ComaMethodID method, void *arg);

private:
    unsigned int _volume;

    void
    VolumeChangedDispatchCallback(void *ctx, ComaNotificationID notification,
            void *arg);

    void
    MutedDispatchCallback(void *ctx, ComaNotificationID notification,
            void *arg);

    void
    UnMutedDispatchCallback(void *ctx, ComaNotificationID notification,
            void *arg);
};

} /* namespace ilixi */
#endif /* SOUNDCOMPONENT_H_ */
