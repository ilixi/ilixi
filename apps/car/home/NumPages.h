/*
 * NumPages.h
 *
 *  Created on: Sep 3, 2012
 *      Author: tarik
 */

#ifndef NUMPAGES_H_
#define NUMPAGES_H_

#include <ui/Widget.h>
#include <ui/GridLayout.h>
#include <types/Font.h>
#include <lib/TweenAnimation.h>
#include <core/ComponentData.h>
#include <vector>
#include "PageButton.h"

namespace ilixi
{

class AppButton;

class NumPages : public Widget
{
public:
    NumPages(Widget* parent = 0);

    virtual
    ~NumPages();

    void
    addItem(const char* name, const char* icon);

    void
    setAppStatus(Compositor::VisibilityData notification);

    void
    setAppStarting(Compositor::VisibilityData notification);

    void
    initPages();

    sigc::signal<void, const char*> sigAppStart;

protected:
    void
    compose(const PaintEvent& event);

private:
    unsigned int _currentPage;
    unsigned int _previousPage;
    TweenAnimation _anim;
    Tween* _tween;

    std::vector<AppButton*> _items;
    std::vector<GridLayout*> _pages;
    std::vector<PageButton*> _buttons;
    Font* _font;

    void
    runApp(const std::string& name);

    void
    showPage(unsigned int index);

    void
    tweenSlot();

    void
    updateNumPagesGeometry();
};

} /* namespace ilixi */
#endif /* NUMPAGES_H_ */
