/*
 Copyright 2010-2015 Tarik Sekmen.

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

#include "Player.h"
#include <ui/HBoxLayout.h>
#include <ui/Label.h>
#include <ui/PushButton.h>
#include <ui/Spacer.h>

namespace ilixi
{

Player::Player(int argc, char* argv[])
        : Application(&argc, &argv)
{
    _errorDialog = new Dialog("Error!", Dialog::OKButtonOption);
    _errorDialog->setLayout(new HBoxLayout());
    _errorDialog->addWidget(new Label("Cannot load file for playback."));

    // Create toolbar
    ToolBar* bar = new ToolBar();
    setToolbar(bar);

    bar->addWidget(new Label("Player v0.2"));
    bar->addWidget(new Spacer(Horizontal));

    _keepAspect = new ToolBarButton("Keep aspect ratio");
    _keepAspect->sigClicked.connect(sigc::mem_fun(this, &Player::toggleKeepAspect));
    _keepAspect->setIcon(StyleHint::Fullscreen);
    bar->addWidget(_keepAspect);

    _load = new ToolBarButton("Load");
    _load->sigClicked.connect(sigc::mem_fun(this, &Player::toggleBrowser));
    _load->setIcon(StyleHint::Eject);
    bar->addWidget(_load);

    setLayout(new HBoxLayout());
    _player = new VideoPlayer();
    addWidget(_player);

    _browser = new FileBrowser(ILIXI_DATADIR);
    _browser->setVisible(false);
    _browser->setFilter(".avi .mp4 .mpeg .mpg .mp2 .ogm .3gp");
    _browser->sigFileSelected.connect(sigc::mem_fun(this, &Player::loadMovie));
    addWidget(_browser);
}

Player::~Player()
{
}

void
Player::toggleBrowser()
{
    if (_browser->visible())
    {
        _browser->setVisible(false);
        _player->setVisible(true);
    } else
    {
        _browser->setVisible(true);
        _player->setVisible(false);
    }
}

void
Player::loadMovie(const std::string file)
{
    if (!_player->load(file))
        _errorDialog->execute();
    else
    {
        _browser->setVisible(false);
        _player->setVisible(true);
    }
}

void
Player::toggleKeepAspect()
{
    static bool aspect = true;
    if (aspect)
        _keepAspect->setText("Strecth");
    else
        _keepAspect->setText("Keep aspect ratio");
    _player->setKeepAspectRatio(aspect);
    aspect = !aspect;
}

} /* namespace ilixi */

int
main(int argc, char* argv[])
{
    ilixi::Player app(argc, argv);
    app.exec();
    return 0;
}
