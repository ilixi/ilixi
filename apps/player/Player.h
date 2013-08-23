/*
 * Player.h
 *
 *  Created on: Nov 23, 2012
 *      Author: tarik
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
    Dialog* _errorDialog;

    void
    toggleBrowser();

    void
    loadMovie(const std::string file);
};

} /* namespace ilixi */
#endif /* PLAYER_H_ */
