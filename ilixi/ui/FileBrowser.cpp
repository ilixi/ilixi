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

#include <ui/FileBrowser.h>
#include <ui/Icon.h>
#include <ui/Label.h>
#include <ui/LineInput.h>
#include <ui/ListBox.h>
#include <ui/RadioButton.h>
#include <ui/VBoxLayout.h>
#include <ui/HBoxLayout.h>
#include <lib/FileSystem.h>
#include <lib/FileInfo.h>
#include <graphics/Painter.h>
#include <core/Logger.h>
#include <algorithm>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_FILEBRWSRITEM, "ilixi/ui/FileBrowserItem", "FileBrowserItem");
D_DEBUG_DOMAIN( ILX_FILEBRWSR, "ilixi/ui/FileBrowser", "FileBrowser");

const std::string audio = ".mp3 .wav";
const std::string video = ".mp4 .avi";
const std::string image = ".bmp .png .jpg";

bool
itemsSort(FileBrowserItem* a, FileBrowserItem* b)
{
    if (a->_info->isDir() && !b->_info->isDir())
        return true;
    else if (!a->_info->isDir() && b->_info->isDir())
        return false;
    return (a->_info->fileName() < b->_info->fileName());
}

FileBrowserItem::FileBrowserItem(const std::string& path, FileBrowser* parent)
        : Widget(parent),
          _owner(parent),
          _info(NULL),
          _icon(NULL),
          _select(NULL)
{
    ILOG_TRACE_W(ILX_FILEBRWSRITEM);
    ILOG_DEBUG(ILX_FILEBRWSRITEM, " -> path: %s\n", path.c_str());
    setInputMethod(KeyPointer);
    setConstraints(MinimumConstraint, FixedConstraint);
    _box = new HBoxLayout();
    addChild(_box);

    _info = new FileInfo(path);
    if (_info->isDir())
        _icon = new Icon(StyleHint::Right);
    else if (_info->isFile())
    {
        std::string suffix = _info->suffix();
        if (audio.find(suffix) != std::string::npos)
            _icon = new Icon(StyleHint::Music);
        else if (video.find(suffix) != std::string::npos)
            _icon = new Icon(StyleHint::Movie);
        else if (image.find(suffix) != std::string::npos)
            _icon = new Icon(StyleHint::Picture);
        else
            _icon = new Icon(StyleHint::File);
    } else
        _icon = new Icon(StyleHint::Clock);
    _icon->setSize(32, 32);
    _box->addWidget(_icon);

    _label = new Label(_info->baseName());
    _label->setConstraints(ExpandingConstraint, MinimumConstraint);
    _label->setSingleLine(true);
    _box->addWidget(_label);

    if (_info->isFile())
    {
        _size = new Label(formatSize(_info->size()));
        _size->setSingleLine(true);
        _box->addWidget(_size);
    }

    sigGeometryUpdated.connect(sigc::mem_fun(this, &FileBrowserItem::updateFileBrowserItemGeometry));
}

FileBrowserItem::~FileBrowserItem()
{
    ILOG_TRACE_W(ILX_FILEBRWSRITEM);
    delete _info;
}

Size
FileBrowserItem::preferredSize() const
{
    return _box->preferredSize();
}

void
FileBrowserItem::compose(const PaintEvent& event)
{
    Painter p(this);
    p.begin(event);
    if (state() & FocusedState)
        p.setBrush(stylist()->palette()->focus);
    else
        p.setBrush(stylist()->palette()->_default.base);
    p.fillRectangle(0, 0, width(), height());
    p.end();
}

void
FileBrowserItem::pointerButtonDownEvent(const PointerEvent& pointerEvent)
{
    std::string s;
    if (_info->isDir())
    {
        s = _info->file();
        char* buffer = realpath(s.c_str(),NULL);
        std::string s(buffer);
        free(buffer);
        _owner->setPath(s + "/");
    }
}

void
FileBrowserItem::keyUpEvent(const KeyEvent& keyEvent)
{
    if (keyEvent.keySymbol == DIKS_SPACE)
    {
        std::string s;
        if (_info->isDir())
        {
            s = _info->file();
            char* buffer = realpath(s.c_str(),NULL);
            std::string s(buffer);
            free(buffer);
            _owner->setPath(s + "/");
        }
    }
}

void
FileBrowserItem::focusInEvent()
{
    update();
}

void
FileBrowserItem::focusOutEvent()
{
    update();
}

void
FileBrowserItem::updateFileBrowserItemGeometry()
{
    ILOG_TRACE_W(ILX_FILEBRWSRITEM);
    _box->setGeometry(5, 0, width() - 10, height());
}

//************************************************************************

FileBrowser::FileBrowser(const std::string& path, Widget* parent)
        : Widget(parent),
          _filter("")
{
    ILOG_TRACE_W(ILX_FILEBRWSR);
    setInputMethod(PointerPassthrough);
    _box = new VBoxLayout();
    addChild(_box);

    _path = new Label("");
    _box->addWidget(_path);

//    _search = new LineInput("");
//    _box->addWidget(_search);

    _list = new ListBox();
    _list->setConstraints(NoConstraint, ExpandingConstraint);
    _box->addWidget(_list);

    setPath(path);
    sigGeometryUpdated.connect(sigc::mem_fun(this, &FileBrowser::updateFileBrowserGeometry));
}

FileBrowser::~FileBrowser()
{
    ILOG_TRACE_W(ILX_FILEBRWSR);
}

Size
FileBrowser::preferredSize() const
{
    ILOG_TRACE_W(ILX_FILEBRWSR);
    return _box->preferredSize();
}

void
FileBrowser::refresh()
{
    ILOG_TRACE_W(ILX_FILEBRWSR);
}

void
FileBrowser::setPath(const std::string& path)
{
    ILOG_TRACE_W(ILX_FILEBRWSR);
    if (path != _path->text())
    {
        printf("PATH: %s\n", path.c_str());
        _path->setText(path);
        std::vector<std::string> files = FileSystem::listDirectory(path);
        std::vector<FileBrowserItem*> items;
        for (int i = 0; i < files.size(); ++i)
        {
            if (files[i] == ".")
                continue;
            items.push_back(new FileBrowserItem(path + files[i], this));
        }

        std::sort(items.begin(), items.end(), itemsSort);

        _list->clear();
        for (int i = 0; i < items.size(); ++i)
            _list->addItem(items[i]);
        items[0]->setFocus();
        update();
    }
}

void
FileBrowser::setFilter(const std::string& filter)
{
    ILOG_TRACE_W(ILX_FILEBRWSR);
}

void
FileBrowser::compose(const PaintEvent& event)
{
//    Painter p(this);
//    p.begin(event);
//    p.setBrush(Color(125, 125, 125));
//    p.fillRectangle(0, 0, width(), height());
}

void
FileBrowser::updateFileBrowserGeometry()
{
    ILOG_TRACE_W(ILX_FILEBRWSR);
    _box->setGeometry(0, 0, width(), height());
}

} /* namespace ilixi */
