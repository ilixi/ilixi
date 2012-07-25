/*
 * Notify.h
 *
 *  Created on: Jul 25, 2012
 *      Author: tarik
 */

#ifndef ILIXI_NOTIFY_H_
#define ILIXI_NOTIFY_H_

#include "graphics/Surface.h"
#include "types/TextLayout.h"
#include <string>

namespace ilixi
{

class Notify
{
public:
    enum NotifyFlags
    {
        NotifyDefault,
        NotifyPriorityLow,
        NotifyPriorityNormal,
        NotifyPriorityHigh,
        NotifySticky
    };

    struct NotifyData
    {
        NotifyData()
                : id(0), flags(NotifyDefault), timeout(2000)
        {
        }

        DFBSurfaceID id;
        NotifyFlags flags;
        unsigned int timeout;
    };

    Notify(const std::string& title, const std::string& text);

    virtual
    ~Notify();

    bool
    sticky() const;

    void
    setTimeout(unsigned int msec);

    void
    setSticky(bool sticky);

    void
    setIcon(const std::string& imagePath);

    void
    show();

private:
    TextLayout _title;
    TextLayout _text;
    std::string _icon;
    NotifyData _data;
};

} /* namespace ilixi */
#endif /* ILIXI_NOTIFY_H_ */
