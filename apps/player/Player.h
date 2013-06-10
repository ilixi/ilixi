/*
 * Player.h
 *
 *  Created on: Nov 23, 2012
 *      Author: tarik
 */

#ifndef PLAYER_H_
#define PLAYER_H_

#include <core/Application.h>
#include <ui/VideoPlayer.h>
#include <ui/FileBrowser.h>

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
};

} /* namespace ilixi */
#endif /* PLAYER_H_ */
