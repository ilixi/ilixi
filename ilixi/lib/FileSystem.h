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

bool
fileExists(const std::string& path);

bool
dirExists(const std::string& path);

bool
renameFile(const std::string& oldName, const std::string& newName);

bool
deleteFile(const std::string& path);

bool
deleteDirectory(const std::string& path);

long long
getFileSize(const std::string& path);

time_t
getModificationTime(const std::string& path);

std::string
homeDirectory();

const std::string
ilxDirectory();

std::string
directoryName(const std::string& path);

std::string
fileName(const std::string& path);

std::vector<std::string>
listDirectory(const std::string& path);

FILE*
temporaryFile(const std::string& prefix);

void
closeFile(FILE* handle);

}

} /* namespace ilixi */
#endif /* ILIXI_FILESYSTEM_H_ */
