#include "Player.h"
#include <ui/HBoxLayout.h>
#include <ui/PushButton.h>

namespace ilixi
{

Player::Player(int argc, char* argv[])
        : Application(&argc, &argv)
{
    setLayout(new HBoxLayout());
    _player = new VideoPlayer();
    addWidget(_player);
    _browser = new FileBrowser(ILIXI_DATADIR);
    addWidget(_browser);
    _player->load(ILIXI_DATADIR"player/test.avi");
}

Player::~Player()
{
}

} /* namespace ilixi */

int
main(int argc, char* argv[])
{
    ilixi::Player app(argc, argv);
    app.exec();
    return 0;
}
