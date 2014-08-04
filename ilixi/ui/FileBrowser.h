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
class ListBox;
class VBoxLayout;
class FileBrowser;

//! Provides a clickable item for files/directories on a file browser.
/*!
 * Shows name for both files and directories; but files have extra information like size.
 */
class FileBrowserItem : public Widget
{
    friend class FileBrowser;
public:
    /*!
     * Constructor
     */
    FileBrowserItem(FileInfo* info, FileBrowser* parent = 0);

    /*!
     * Destructor
     */
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
    //! This is a pointer to parent file browser.
    FileBrowser* _owner;
    //! This stores information about file
    FileInfo* _info;
    //! This label shows name of file
    Label* _label;
    //! This label shows modified date of file
    Label* _date;
    //! This label shows size of file
    Label* _size;
    //! This icon is created based on mime type
    Icon* _icon;
    //! This is used for drawing an alternate background color
    bool _alternateRow;
    //! This stores all child widgets
    HBoxLayout* _box;

    void
    updateFileBrowserItemGeometry();

    friend bool
    itemsSort(FileBrowserItem* a, FileBrowserItem* b);
};

//! Provides a simple widget to list/select files and directories
class FileBrowser : public Widget
{
public:
    /*!
     *
     */
    FileBrowser(const std::string& path = "", Widget* parent = 0);

    /*!
     * Destructor
     */
    virtual
    ~FileBrowser();

    Size
    preferredSize() const;

    /*!
     * Refreshes current path and updates widget
     */
    void
    refresh();

    /*!
     * Returns true if hidden files are listed, false otherwise.
     */
    bool
    showHidden() const;

    /*!
     * Parses given path for files and directories and updates list using filter.
     */
    void
    setPath(const std::string& path);

    /*!
     * Sets filter for files.
     *
     * An example filter to list movies:
     * \code
     * setFilter(".avi .mp4 .mpeg .mpg .mp2 .ogm .3gp");
     * \endcode
     */
    void
    setFilter(const std::string& filter);

    /*!
     * Sets whether to how hidden files.
     */
    void
    setShowHidden(bool showHidden);

    /*!
     * This signal is emitted once a file is selected either by pressing pointer or
     * by clicking SPACE key.
     */
    sigc::signal<void, std::string> sigFileSelected;

protected:
    void
    compose(const PaintEvent& event);

private:
    //! This flag stores whether hidden files are shown.
    bool _showHidden;
    //! This flag is set to true if filter and/or path are modified.
    bool _modified;
    //! This property stores filter string.
    std::string _filter;
    //! This label shows current path.
    Label* _path;
    //! This list contains file browser items.
    ListBox* _list;
    //! This layout is used to position path label and list.
    VBoxLayout* _box;

    void
    updateFileBrowserGeometry();
};

} /* namespace ilixi */
#endif /* ILIXI_FILEBROWSER_H_ */
