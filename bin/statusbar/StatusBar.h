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
#include "StatusbarButton.h"
#include "RemoteContentArea.h"

namespace ilixi
{

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

    void
    clickedHome();

    void
    clickedSwitcher();

    void
    clickedTemp();

    void
    clickedSound();

private:
    Image* _bg;
    StatusbarButton* _home;
    StatusbarButton* _switch;
    StatusbarButton* _dash;
    StatusbarButton* _sound;
    RemoteContentArea* _rca;

    IComaComponent* _soundComponent;
    IComaComponent* _compComponent;
    StatusbarComponent* _statComp;

    friend void
    volumeListener(void* ctx, void* arg);

    friend void
    homeShowing(void* ctx, void* arg);

    friend void
    homeHidden(void* ctx, void* arg);

    friend void
    switcherShowing(void* ctx, void* arg);

    friend void
    switcherHidden(void* ctx, void* arg);

    friend void
    soundVisible(void* ctx, void* arg);

    friend void
    soundHidden(void* ctx, void* arg);

    friend void
    tempVisible(void* ctx, void* arg);

    friend void
    tempHidden(void* ctx, void* arg);
};

}
#endif /* STATUSBAR_H_ */
