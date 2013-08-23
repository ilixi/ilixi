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
    _load = new ToolBarButton("Load");
    _load->sigClicked.connect(sigc::mem_fun(this, &Player::toggleBrowser));
    _load->setIcon(StyleHint::Eject);
    bar->addWidget(_load);

    setLayout(new HBoxLayout());
    _player = new VideoPlayer();
    _player->setAutoHideControls(false);
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

} /* namespace ilixi */

int
main(int argc, char* argv[])
{
    ilixi::Player app(argc, argv);
    app.exec();
    return 0;
}
