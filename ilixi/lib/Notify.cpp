/*
 * Notify.cpp
 *
 *  Created on: Jul 25, 2012
 *      Author: tarik
 */

#include "Notify.h"
#include "core/AppBase.h"
#include "ui/Widget.h"

namespace ilixi
{

const int NotificationWidth = 300;

Notify::Notify(const std::string& title, const std::string& text)
        : _title(), _text(), _icon("")
{
    _title.setText(title);
    _text.setText(text);
}

Notify::~Notify()
{

}

bool
Notify::sticky() const
{
    return (_data.flags & NotifySticky);
}

void
Notify::setTimeout(unsigned int msec)
{
    _data.timeout = msec;
}

void
Notify::setSticky(bool sticky)
{
    _data.flags = (NotifyFlags) (_data.flags | NotifySticky);
}

void
Notify::setIcon(const std::string& imagePath)
{
    _icon = imagePath;
}

void
Notify::show()
{
    // setup surface
    Font* titleFont;
    Font* textFont;
    IDirectFBFont* titleDFBFont;
    IDirectFBFont* textDFBFont;
    int titleHeight = 0;
    int textHeight = 0;
    Image* icon = 0;

    Stylist* style = Widget::stylist();

    titleFont = style->defaultFont(StyleHint::TitleFont);
    titleHeight = _title.heightForWidth(NotificationWidth, titleFont);
    titleDFBFont = titleFont->dfbFont();
    _title.setBounds(0, 0, NotificationWidth, titleHeight);

    if (!_icon.empty())
        icon = new Image(_icon, Size(96, 96));

    textFont = style->defaultFont(StyleHint::DefaultFont);
    textHeight = _text.heightForWidth(NotificationWidth - (icon ? 101 : 0),
            textFont);
    textDFBFont = textFont->dfbFont();
    _text.setBounds(0, 0, NotificationWidth, textHeight);

    Surface* notifySurface = new Surface();
    notifySurface->createDFBSurface(NotificationWidth,
            std::max(96, titleHeight) + textHeight);
    IDirectFBSurface* surface = notifySurface->dfbSurface();
    surface->AllowAccess(surface, "*");
    surface->SetColor(surface, 255, 255, 255, 255);
    surface->FillRectangle(surface, 0, 0, NotificationWidth,
            std::max(96, titleHeight) + textHeight);

    surface->SetColor(surface, 0, 0, 0, 255);
    // draw title
    surface->SetFont(surface, titleDFBFont);
    _title.doLayout(titleFont);
    _title.drawTextLayout(surface, 0, 0);
    surface->Flip(surface, 0, DSFLIP_NONE);

    // draw icon
    if (icon)
    {
        DFBRectangle r = { 0, titleHeight, 96, 96 };
        surface->StretchBlit(surface, icon->getDFBSurface(), NULL, &r);
    }

    // draw text
    surface->SetFont(surface, textDFBFont);
    _text.doLayout(textFont);
    _text.drawTextLayout(surface, (icon ? 101 : 0), titleHeight);
    surface->Flip(surface, 0, DSFLIP_NONE);

    // show popup
    IComaComponent* _component;
    void *ptr;
    int result;

    AppBase::comaGetComponent("PopUpComponent", &_component);

    AppBase::comaGetLocal(sizeof(DFBSurfaceID), &ptr);
    DFBSurfaceID* _id = (DFBSurfaceID*) ptr;
    *_id = notifySurface->dfbSurfaceId();

    AppBase::comaCallComponent(_component, 0, (void*) _id);

    _component->Release(_component);
    surface->Flip(surface, NULL, DSFLIP_NONE);

    delete icon;
    delete notifySurface;
}

} /* namespace ilixi */
