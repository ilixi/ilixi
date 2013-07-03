/*
 * Call.cpp
 *
 *  Created on: Jun 18, 2013
 *      Author: tarik
 */

#include "Call.h"
#include <ui/VBoxLayout.h>
#include <ui/SurfaceView.h>
#include <ui/Spacer.h>
#include <sstream>

namespace ilixi
{

Call::Call(Widget* parent)
        : Frame(parent),
          _duration(0),
          _holdFlag(true),
          _muteFlag(true)
{
    VBoxLayout* box = new VBoxLayout();
    box->setHorizontalAlignment(Alignment::Center);
    setLayout(box);
    setConstraints(MinimumConstraint, MinimumConstraint);

    _contact = new Label("");
    _contact->setLayoutAlignment(TextLayout::Center);
    _contact->setFont(stylist()->defaultFont(StyleHint::ButtonFont));
    addWidget(_contact);

    _info = new Label("Connecting...");
    _info->setLayoutAlignment(TextLayout::Center);
    addWidget(_info);

    _video = new SurfaceView();
    _video->setVisible(false);
    _video->setConstraints(ExpandingConstraint, ExpandingConstraint);
    _video->setMinimumSize(352, 288);
    _video->setMaximumSize(352, 288);
    addWidget(_video);

    addWidget(new Spacer(Vertical));

    _cancel = new PushButton("Cancel Call");
    _cancel->setPushButtonStyle(CANCEL);
    _cancel->sigClicked.connect(sigHangup);
    addWidget(_cancel);

    _timer = new Timer();
    _timer->sigExec.connect(sigc::mem_fun(this, &Call::updateTime));

}

Call::~Call()
{
}

void
Call::setContact(const std::string& name)
{
    _contact->setText(name);
}

void
Call::setStatus(CallStatus status)
{
    if (status == Connecting)
    {
        _info->setText("Connecting...");
        _cancel->setVisible(true);
    } else if (status == Ringing)
    {
        _info->setText("Ringing...");
        _cancel->setVisible(true);
    } else if (status == Progress)
    {

    } else if (status == Established)
    {
        _duration = time(0);
        _info->setText("00:00:00");
        _timer->start(1000);
        _cancel->setVisible(false);
    } else if (status == Ended)
    {
        _muteFlag = true;
        _holdFlag = true;
        _info->setText("Connecting...");
        _video->setVisible(false);
        _timer->stop();
    } else if (status == Mute)
    {
        if (_muteFlag)
            _muteFlag = false;
        else
            _muteFlag = true;
    } else if (status == Hold)
    {
        if (_holdFlag)
            _holdFlag = false;
        else
            _holdFlag = true;
    }
}

void
Call::setVideoSurfaceID(unsigned int surfaceID)
{
    if (surfaceID)
    {
        _video->setSourceFromSurfaceID(surfaceID);
        _video->setVisible(true);
    } else
    {
        _video->setVisible(true);
    }
}

bool
Call::holdFlag() const
{
    return _holdFlag;
}

bool
Call::muteFlag() const
{
    return _muteFlag;
}

std::string
Call::formatting(int value)
{
    std::stringstream str;
    if (value == 0)
        str << "00";
    else if (value < 10)
        str << "0" << value;
    else
        str << value;
    return str.str();
}

void
Call::updateTime()
{
    int seconds = difftime(time(0), _duration);
    std::stringstream str;
    if (seconds > 0)
    {
        str << formatting((int) (seconds / 3600)) << ":";
        str << formatting((int) ((seconds % 3600) / 60)) << ":";
        str << formatting((int) ((seconds % 3600) % 60));
        _info->setText(str.str());
    } else
        return _info->setText("00:00:00");

}

} /* namespace ilixi */
