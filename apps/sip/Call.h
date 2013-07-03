/*
 * Call.h
 *
 *  Created on: Jun 18, 2013
 *      Author: tarik
 */

#ifndef ILXSIP_CALL_H_
#define ILXSIP_CALL_H_

#include <lib/Timer.h>
#include <ui/ButtonGroup.h>
#include <ui/Frame.h>
#include <ui/Label.h>
#include <ui/PushButton.h>
#include <ui/SurfaceView.h>
#include <time.h>

namespace ilixi
{

class Call : public Frame
{
public:
    enum CallStatus
    {
        Connecting,
        Ringing,
        Established,
        Progress,
        Ended,
        Mute,
        Hold
    };

    Call(Widget* parent = 0);

    virtual
    ~Call();

    void
    setContact(const std::string& name);

    void
    setStatus(CallStatus status);

    void
    setVideoSurfaceID(unsigned int surfaceID);

    bool
    holdFlag() const;

    bool
    muteFlag() const;

    sigc::signal<void> sigHangup;

private:
    time_t _duration;
    Timer* _timer;
    Label* _contact;
    Label* _info;

    SurfaceView* _video;

    PushButton* _cancel;

    bool _muteFlag;
    bool _holdFlag;

    std::string
    formatting(int value);

    void
    updateTime();

};

} /* namespace ilixi */
#endif /* ILXSIP_CALL_H_ */
