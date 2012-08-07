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

#ifndef STATUSBAR_H_
#define STATUSBAR_H_

#include <ui/Application.h>
#include <ui/Label.h>
#include <ui/ToolButton.h>
#include <vector>

namespace ilixi
{

class SurfaceView;
class StatusbarComponent;

class StatusBar : public Application
{
public:
    StatusBar(int argc, char* argv[]);

    virtual
    ~StatusBar();

    virtual void
    onHide();

    virtual void
    onShow();

    bool
    addRemoteContent(DFBSurfaceID id);

    bool
    removeRemoteContent(DFBSurfaceID id);

private:
    unsigned int _currentAppID;
    Label* _app;

    Label* _volume;

    ToolButton* _home;
    ToolButton* _shutdown;

    IComaComponent* _soundComponent;
    StatusbarComponent* _statComp;

    std::vector<SurfaceView*> _remoteContent;

    Image* _bg;
    Rectangle tl;
    Rectangle tm;
    Rectangle tr;
    Rectangle l;
    Rectangle m;
    Rectangle r;
    Rectangle bl;
    Rectangle bm;
    Rectangle br;

    virtual void
    compose(const PaintEvent& event);

    friend void
    volumeListener(void* ctx, void* arg);
};

}
#endif /* STATUSBAR_H_ */
