/*
 * RemoteContentArea.cpp
 *
 *  Created on: 9 Aug 2012
 *      Author: tarik
 */

#include "RemoteContentArea.h"
#include <core/Logger.h>
#include <graphics/Painter.h>
#include <ui/SurfaceView.h>
#include <ui/HBoxLayout.h>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_STATUSRCA, "ilixi/car/status/RCA", "RemoteContentArea");

Image* RemoteContentArea::_bgDef = NULL;

RemoteContentArea::RemoteContentArea(Widget* parent)
        : ContainerBase(parent)
{
    if (!_bgDef)
        _bgDef = new Image(ILIXI_DATADIR"car/statusbar/statusbar-buttons.png");

    setConstraints(MinimumConstraint, FixedConstraint);
    setLayout(new HBoxLayout());
}

RemoteContentArea::~RemoteContentArea()
{
}

Size
RemoteContentArea::preferredSize() const
{
    Size s = _layout->preferredSize();
    return Size(s.width(), 55);
}

bool
RemoteContentArea::addRemoteContent(DFBSurfaceID id)
{
    SurfaceView* s = new SurfaceView();
    s->setSourceFromSurfaceID(id);
    s->setBlendingEnabled(true);
    s->setConstraints(FixedConstraint, FixedConstraint);

    if (addWidget(s))
    {
        _remoteContent.push_back(s);
        ILOG_DEBUG(ILX_STATUSRCA, "addRemoteContent with ID: %u\n", id);
        update();
        return true;
    }
    delete s;
    return false;
}

bool
RemoteContentArea::removeRemoteContent(DFBSurfaceID id)
{
    for (std::vector<SurfaceView*>::iterator it = _remoteContent.begin(); it != _remoteContent.end(); ++it)
    {
        if ((*it)->sourceID() == id)
        {
            removeWidget(*it);
            _remoteContent.erase(it);
            ILOG_DEBUG(ILX_STATUSRCA, "removeRemoteContent with ID: %u\n", id);
            update();
            return true;
        }
    }
    return false;
}

void
RemoteContentArea::compose(const PaintEvent& event)
{
    Painter p(this);
    p.begin(event);

    p.blitImage(_bgDef, Rectangle(0, 0, 15, 55), 0, 0);
    p.stretchImage(_bgDef, Rectangle(15, 0, width() - 15, 55), Rectangle(15, 0, 91, 55)); // mid
    p.end();
}

void
RemoteContentArea::updateLayoutGeometry()
{
    _layout->setGeometry(15, 5, width() - 15, 50);
}

} /* namespace ilixi */
