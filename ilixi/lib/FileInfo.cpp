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

#include <lib/FileInfo.h>
#include <lib/FileSystem.h>
#include <core/Logger.h>
#include <libgen.h>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_FILEINFO, "ilixi/lib/FileInfo", "FileInfo");

FileInfo::FileInfo()
        : _ready(false),
          _perms(None)
{
}

FileInfo::FileInfo(const std::string& file)
{
    setFile(file);
}

FileInfo::~FileInfo()
{
}

std::string
FileInfo::absolutePath() const
{
    if (_ready)
        return std::string(dirname(const_cast<char*>(_file.c_str())));
    return std::string("");
}

std::string
FileInfo::baseName() const
{
    if (_ready)
        return std::string(basename(const_cast<char*>(_file.c_str())));
    return std::string("");
}

bool
FileInfo::exists() const
{
    return _ready;
}

std::string
FileInfo::file() const
{
    return _file;
}

std::string
FileInfo::fileName() const
{
    if (_ready)
    {
        std::string s = baseName();
        size_t pos = s.rfind(".");
        if (pos == std::string::npos)
            return s;
        if (pos == 0)
            return s;
        return s.substr(0, pos);
    }
    return std::string("");
}

bool
FileInfo::isDir() const
{
    if (_ready)
        return S_ISDIR(_stat.st_mode);
    return false;
}

bool
FileInfo::isFile() const
{
    if (_ready)
        return S_ISREG(_stat.st_mode);
    return false;
}

bool
FileInfo::isHidden() const
{
    std::string s = baseName();
    if (s.size() && s.at(0) == '.')
        return true;
    return false;
}

bool
FileInfo::isExecutable() const
{
    if (_ready)
        if (access(_file.c_str(), X_OK) == 0)
            return true;
    return false;
}

bool
FileInfo::isReadable() const
{
    if (_ready)
        if (access(_file.c_str(), R_OK) == 0)
            return true;
    return false;
}

bool
FileInfo::isSymLink() const
{
    if (_ready)
        return S_ISLNK(_stat.st_mode);
    return false;
}

bool
FileInfo::isWritable() const
{
    if (_ready)
        if (access(_file.c_str(), W_OK) == 0)
            return true;
    return false;
}

time_t
FileInfo::lastAccessed() const
{
    if (_ready)
        return _stat.st_atime;
    return 0;
}

time_t
FileInfo::lastModified() const
{
    if (_ready)
        return _stat.st_mtime;
    return 0;
}

unsigned int
FileInfo::ownerID() const
{
    if (_ready)
        return _stat.st_uid;
    return 0;
}

unsigned int
FileInfo::groupID() const
{
    if (_ready)
        return _stat.st_gid;
    return 0;
}

FileInfo::FilePermissions
FileInfo::permissions() const
{
    return _perms;
}

void
FileInfo::refresh()
{
    if (access(_file.c_str(), F_OK) == 0)
    {
        if (stat(_file.c_str(), &_stat) == 0)
            _ready = true;
        else
            _ready = false;
    } else
        _ready = false;

    if (_ready)
    {
        _perms = None;
        if (_stat.st_mode & S_IRUSR)
            _perms = (FilePermissions) (_perms | ReadOwner);
        if (_stat.st_mode & S_IWUSR)
            _perms = (FilePermissions) (_perms | WriteOwner);
        if (_stat.st_mode & S_IXUSR)
            _perms = (FilePermissions) (_perms | ExeOwner);
        if (_stat.st_mode & S_IRGRP)
            _perms = (FilePermissions) (_perms | ReadGroup);
        if (_stat.st_mode & S_IWGRP)
            _perms = (FilePermissions) (_perms | WriteGroup);
        if (_stat.st_mode & S_IXGRP)
            _perms = (FilePermissions) (_perms | ExeGroup);
        if (_stat.st_mode & S_IROTH)
            _perms = (FilePermissions) (_perms | ReadOther);
        if (_stat.st_mode & S_IWOTH)
            _perms = (FilePermissions) (_perms | WriteOther);
        if (_stat.st_mode & S_IXOTH)
            _perms = (FilePermissions) (_perms | ExeOther);
    }
}

bool
FileInfo::setFile(const std::string& file)
{
    if (_file != file)
    {
        _file = file;
        refresh();
    }
    return _ready;
}

long long
FileInfo::size() const
{
    if (_ready)
        return _stat.st_size;
    return 0;
}

std::string
FileInfo::suffix() const
{
    if (_ready)
    {
        std::string s = baseName();
        size_t pos = s.rfind(".");
        if (pos == std::string::npos)
            return std::string("");
        if (pos == 0)
            return std::string("");
        return s.substr(pos, s.size() - pos);
    }
    return std::string("");
}

std::string
FileInfo::symLinkTarget() const
{
    if (_ready && isSymLink())
    {
        char* buffer = realpath(_file.c_str(),NULL);
        std::string s(buffer);
        free(buffer);
        return s;
    }
    return std::string("");
}

} /* namespace ilixi */
