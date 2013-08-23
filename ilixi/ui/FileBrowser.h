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

#ifndef ILIXI_FILEBROWSER_H_
#define ILIXI_FILEBROWSER_H_

#include <ui/Widget.h>

namespace ilixi
{

class FileInfo;
class HBoxLayout;
class Icon;
class Label;
class LineInput;
class ListBox;
class RadioButton;
class VBoxLayout;
class FileBrowser;

class FileBrowserItem : public Widget
{
    friend class FileBrowser;
public:
    FileBrowserItem(FileInfo* info, FileBrowser* parent = 0);

    virtual
    ~FileBrowserItem();

    Size
    preferredSize() const;

protected:
    void
    compose(const PaintEvent& event);

    void
    pointerButtonDownEvent(const PointerEvent& pointerEvent);

    void
    keyUpEvent(const KeyEvent& keyEvent);

    void
    focusInEvent();

    void
    focusOutEvent();

private:
    FileBrowser* _owner;
    FileInfo* _info;
    Label* _label;
    Label* _date;
    Label* _size;
    Icon* _icon;
    RadioButton* _select;
    HBoxLayout* _box;

    void
    updateFileBrowserItemGeometry();

    friend bool
    itemsSort(FileBrowserItem* a, FileBrowserItem* b);
};

class FileBrowser : public Widget
{
public:
    FileBrowser(const std::string& path = "", Widget* parent = 0);

    virtual
    ~FileBrowser();

    Size
    preferredSize() const;

    void
    refresh();

    void
    setPath(const std::string& path);

    void
    setFilter(const std::string& filter);

    /*!
     * This signal is emitted once a file is selected either by pressing pointer or
     * by clicking SPACE key.
     */
    sigc::signal<void, std::string> sigFileSelected;

protected:
    void
    compose(const PaintEvent& event);

private:
    bool _modified;
    std::string _filter;
    Label* _path;
    LineInput* _search;
    ListBox* _list;
    VBoxLayout* _box;

    void
    updateFileBrowserGeometry();
};

} /* namespace ilixi */
#endif /* ILIXI_FILEBROWSER_H_ */
