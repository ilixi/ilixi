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

#include "StatusBar.h"
#include <ui/HBoxLayout.h>
#include <ui/VBoxLayout.h>
#include <graphics/Painter.h>
#include <core/Logger.h>
#include <sigc++/sigc++.h>
#include <string.h>
#include "Clock.h"

D_DEBUG_DOMAIN( ILX_STATUSBAR, "ilixi/StatusBar", "StatusBar");

void
volumeListener(void* ctx, void* arg)
{
    StatusBar* bar = (StatusBar*) ctx;
    char volText[3];
    sprintf(volText, "%d", *((int*) arg));
    bar->_volume->setText(volText);
    ILOG_DEBUG(ILX_STATUSBAR, "Volume %d\n", *((int*) arg));
}

StatusBar::StatusBar(int argc, char* argv[])
        : Application(&argc, &argv, (AppOptions) (OptStatusBar | OptDale))
{
    setTitle("StatusBar");
    setBackgroundFilled(true);
    setMargins(0, 0, 10, 10);
    HBoxLayout* mainLayout = new HBoxLayout();
    mainLayout->setVerticalAlignment(Alignment::Middle);
    mainLayout->setSpacing(0);
    setLayout(mainLayout);

    _app = new Label("dfb_ilixi");
    _app->setLayoutAlignment(TextLayout::Left);
    _app->setFont(new Font("decker", 36));
    addWidget(_app);

    addWidget(new Clock());

    _volume = new Label("  0  ");
    _volume->setLayoutAlignment(TextLayout::Left);
    _volume->setFont(new Font("decker", 36));
    addWidget(_volume);

    sigVisible.connect(sigc::mem_fun(this, &StatusBar::onShow));
    sigHidden.connect(sigc::mem_fun(this, &StatusBar::onHide));

    _bg = new Image(ILIXI_DATADIR"/images/statusbar_bg.png");
    tl = Rectangle(0, 0, 32, 32);
    tm = Rectangle(32, 0, 1, 32);
    tr = Rectangle(33, 0, 32, 32);
    l = Rectangle(0, 32, 32, 1);
    m = Rectangle(32, 32, 1, 1);
    r = Rectangle(33, 32, 32, 1);
    bl = Rectangle(0, 33, 32, 32);
    bm = Rectangle(32, 33, 1, 32);
    br = Rectangle(33, 33, 32, 32);
}

StatusBar::~StatusBar()
{
    delete _bg;
}

void
StatusBar::onHide()
{
    _soundComponent->Release(_soundComponent);
}

void
StatusBar::onShow()
{
    comaGetComponent("SoundComponent", &_soundComponent);
    _soundComponent->Listen(_soundComponent, 0, volumeListener, this);
}

void
StatusBar::compose(const PaintEvent& event)
{
    Painter painter(this);
    painter.begin(event);

    int w = width();
    int h = height();
    int midWidth = w - tl.width() - tr.width();
    int midHeight = h - bl.height();
    int midY = tl.height();
    int by = h - bl.height();

    // top
    painter.blitImage(_bg, tl, 0, 0);
    painter.setClip(tl.width(), 0, midWidth, bm.height());
    painter.tileImage(_bg, tl.width(), 0, tm);
    painter.resetClip();
    painter.blitImage(_bg, tr, w - tr.width(), 0);

    // left
    painter.setClip(0, midY, l.width(), midHeight);
    painter.tileImage(_bg, 0, midY, l);
    painter.resetClip();

    // right
    painter.setClip(w - r.width(), midY, r.width(), midHeight);
    painter.tileImage(_bg, w - r.width(), midY, r);
    painter.resetClip();

    // mid
    painter.setClip(l.width(), midY, midWidth, midHeight);
    painter.tileImage(_bg, l.width(), midY, m);
    painter.resetClip();

//    // bottom
//    painter.blitImage(_bg, bl, 0, by);
//    painter.setClip(bl.width(), by, midWidth, bm.height());
//    painter.tileImage(_bg, bl.width(), by, bm);
//    painter.resetClip();
//    painter.blitImage(_bg, br, w - br.width(), by);

    painter.end();
}

int
main(int argc, char* argv[])
{
    StatusBar app(argc, argv);
    app.exec();
    return 0;
}
