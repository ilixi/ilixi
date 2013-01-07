/*
 * AppButton.h
 *
 *  Created on: Aug 29, 2012
 *      Author: tarik
 */

#ifndef APPBUTTON_H_
#define APPBUTTON_H_

#include <ui/ToolButton.h>
#include <lib/Timer.h>

namespace ilixi
{

class PAppButton : public ilixi::ToolButton
{
public:
    PAppButton(const std::string& text, Widget* parent = 0);

    virtual
    ~PAppButton();

    Size
    preferredSize() const;

    void
    setAppVisible(bool visible);

    void
    appStarting();

    void
    appQuit();

protected:
    void
    compose(const PaintEvent& event);

private:
    bool _colorize;
    Point _iconPos;
    Color _textColor;

    Timer _timer;

    void
    updateIconGeometry();

    void
    timerSlot();

};

} /* namespace ilixi */
#endif /* APPBUTTON_H_ */
