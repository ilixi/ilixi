/*
 Copyright 2010-2015 Tarik Sekmen.

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

#ifndef ILIXI_FILESYSTEM_H_
#define ILIXI_FILESYSTEM_H_

#include <string>
#include <vector>

namespace ilixi
{

//! File system related functions.
namespace FileSystem
{
// FileInfo and File

//! Returns true if file exists.
bool
fileExists(const std::string& path);

//! Returns true if directory exists.
bool
dirExists(const std::string& path);

//! Renames file, returns true on success.
bool
renameFile(const std::string& oldName, const std::string& newName);

//! Deletes file, returns true on success.
bool
deleteFile(const std::string& path);

//! Deletes directory, returns true on success.
bool
deleteDirectory(const std::string& path);

//! Returns size of file in bytes.
long long
getFileSize(const std::string& path);

//! Returns last modified time/date.
time_t
getModificationTime(const std::string& path);

//! Returns path to home directory.
std::string
homeDirectory();

//! Returns directory used by ilixi for storing configuration and cached files.
const std::string
ilxDirectory();

//! Parses given path and returns directory name.
std::string
directoryName(const std::string& path);

//! Parses given path and returns file name.
std::string
fileName(const std::string& path);

//! Returns list of files in given path.
std::vector<std::string>
listDirectory(const std::string& path);

//! Creates a temporary file.
FILE*
temporaryFile(const std::string& prefix);

//! Closes file.
void
closeFile(FILE* handle);

//! Returns mime type of given extension if recognised by system.
const std::string
getMimeType(const std::string& extension);

}

} /* namespace ilixi */
#endif /* ILIXI_FILESYSTEM_H_ */
