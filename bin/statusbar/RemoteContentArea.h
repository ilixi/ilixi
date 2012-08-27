/*
 Copyright 2010-2012 Tarik Sekmen.

 All Rights Reserved.

 Written by Tarik Sekmen <tarik@ilixi.org>.

 This file is part of ilixi.

 ilixi is free software: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 ilixi is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with ilixi.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef REMOTECONTENTAREA_H_
#define REMOTECONTENTAREA_H_

#include <ui/ContainerBase.h>
#include <vector>

namespace ilixi
{
class SurfaceView;
class RemoteContentArea : public ContainerBase
{
public:
    RemoteContentArea(Widget* parent = 0);

    virtual
    ~RemoteContentArea();

    virtual Size
    preferredSize() const;

    bool
    addRemoteContent(DFBSurfaceID id);

    bool
    removeRemoteContent(DFBSurfaceID id);

protected:
    void
    compose(const PaintEvent& event);

private:
    static Image* _bgDef;
    std::vector<SurfaceView*> _remoteContent;

    virtual void
    updateLayoutGeometry();
};

} /* namespace ilixi */
#endif /* REMOTECONTENTAREA_H_ */
