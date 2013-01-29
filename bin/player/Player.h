/*
 * Player.h
 *
 *  Created on: Nov 23, 2012
 *      Author: tarik
 */

#ifndef PLAYER_H_
#define PLAYER_H_

#include <ui/Application.h>
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
};

} /* namespace ilixi */
#endif /* PLAYER_H_ */
