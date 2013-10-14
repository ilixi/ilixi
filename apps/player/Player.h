/*
 Copyright 2010-2013 Tarik Sekmen.

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

#ifndef PLAYER_H_
#define PLAYER_H_

#include <core/Application.h>
#include <ui/Dialog.h>
#include <ui/FileBrowser.h>
#include <ui/ToolBar.h>
#include <ui/VideoPlayer.h>

namespace ilixi
{

class Player : public Application
{
public:
    Player(int argc, char* argv[]);

    virtual
    ~Player();

private:
    VideoPlayer* _player;
    FileBrowser* _browser;
    ToolBarButton* _load;
    ToolBarButton* _keepAspect;
    Dialog* _errorDialog;

    void
    toggleBrowser();

    void
    toggleKeepAspect();

    void
    loadMovie(const std::string file);
};

} /* namespace ilixi */
#endif /* PLAYER_H_ */
