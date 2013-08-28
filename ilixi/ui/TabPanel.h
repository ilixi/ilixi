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

#ifndef ILIXI_TABPANEL_H_
#define ILIXI_TABPANEL_H_

#include <ui/Button.h>
#include <ui/Frame.h>
#include <vector>

namespace ilixi
{
//! Used for displaying tabs in a panel.
class TabPanelButton : public Button
{
public:
    /*!
     * Contructor.
     */
    TabPanelButton(std::string label, Widget* parent);

    /*!
     * Destructor.
     */
    virtual
    ~TabPanelButton();

    virtual Size
    preferredSize() const;

    virtual void
    toggleChecked();

    void
    setFirst(bool first);

protected:
    void
    compose(const PaintEvent& event);

    virtual void
    updateTextBaseGeometry();

private:
    bool _first;

    virtual Font*
    defaultFont() const;
};

//! Provides a multi-page container with tabs.
class TabPanel : public Frame
{
public:
    struct TabData
    {
        Widget* widget;
        TabPanelButton* button;
        Size widgetSize;
    };

    /*!
     * Contructor.
     */
    TabPanel(Widget* parent = 0);

    /*!
     * Destructor.
     */
    virtual
    ~TabPanel();

    /*!
     * Returns a height for tab panel given a width.
     *
     * @param width Desired width of tab panel.
     */
    virtual int
    heightForWidth(int width) const;

    /*!
     * Calculates the preferred size of contents and returns an updated size using canvas, margins and borders.
     */
    virtual Size
    preferredSize() const;

    /*!
     * Returns frame's canvas y-coordinate including the top margin.
     */
    virtual int
    canvasY() const;

    /*!
     * Returns frame's canvas height excluding margins.
     */
    virtual int
    canvasHeight() const;

    /*!
     * Returns number of tabs.
     */
    int
    count() const;

    /*!
     * Removes all tabs.
     */
    void
    clear();

    /*!
     * Returns index of currently selected tab.
     */
    int
    currentIndex() const;

    /*!
     * Returns the widget for current tab.
     */
    Widget*
    currentWidget() const;

    /*!
     * Returns tab index of given widget. -1 if not found.
     */
    int
    indexOf(Widget* widget) const;

    /*!
     * Returns true if tab at given index is enabled.
     */
    bool
    tabEnabled(int index) const;

    /*!
     * Returns tab label at given index.
     */
    std::string
    tabLabel(int index) const;

    /*!
     * Adds a new tab at the end.
     *
     * @param widget shown when tab is selected.
     * @param label for tab.
     */
    void
    addTab(Widget* widget, std::string label);

    /*!
     * Insert a new tab at given index.
     *
     * @param index
     * @param widget
     * @param label
     */
    void
    insertTab(int index, Widget* widget, std::string label);

    /*!
     * Removes tab and widget at given index.
     */
    void
    removeTab(int index);

    /*!
     * Switches to tab at given index.
     */
    void
    setCurrentTab(int index);

    /*!
     * Sets label for tab at given index.
     */
    void
    setTabLabel(int index, std::string label);

    /*!
     * Enables/Disables tab at given index.
     */
    void
    setTabEnabled(int index, bool enabled);

    virtual void
    doLayout();

protected:
    /*!
     * Draws tab panel frame.
     */
    virtual void
    compose(const PaintEvent& event);

private:
    //! This property stores index of currently selected tab.
    int _currentIndex;
    //! This property stores the offset for tab's widget.
    int _canvasOffsetY;

    typedef std::vector<TabData> TabList;
    TabList _tabs;

    virtual void
    updateChildrenFrameGeometry();

    virtual void
    paintChildren(const PaintEvent& event);
};
}

#endif /* ILIXI_TABPANEL_H_ */
