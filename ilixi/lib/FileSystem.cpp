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

#include <lib/FileSystem.h>
#include <libgen.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <core/Logger.h>

namespace ilixi
{

D_DEBUG_DOMAIN( ILX_FILESYSTEM, "ilixi/lib/FileSystem", "FileSystem");

namespace FileSystem
{

bool
fileExists(const std::string& path)
{
    if (access(path.c_str(), F_OK) != 0)
        return false;
    return true;
}

bool
dirExists(const std::string& path)
{
    const char *myDir = dirname(const_cast<char*>(path.c_str()));
    struct stat myStat;
    if ((stat(myDir, &myStat) == 0) && (((myStat.st_mode) & S_IFMT) == S_IFDIR))
        return true;
    return false;
}

bool
renameFile(const std::string& oldName, const std::string& newName)
{
    if (rename(oldName.c_str(), newName.c_str()) == 0)
        return true;
    return false;
}

bool
deleteFile(const std::string& path)
{
    if (unlink(path.c_str()) == 0)
        return true;
    return false;
}

bool
deleteDirectory(const std::string& path)
{
    if (rmdir(path.c_str()) == 0)
        return true;
    return false;
}

long long
getFileSize(const std::string& path)
{
    struct stat filestatus;
    if (stat(path.c_str(), &filestatus) == 0)
        return filestatus.st_size;
    return 0;
}

time_t
getModificationTime(const std::string& path)
{
    struct stat filestatus;
    if (stat(path.c_str(), &filestatus) == 0)
        return filestatus.st_mtime;
    return 0;
}

std::string
homeDirectory()
{
#ifdef ANDROID_NDK
    const char *home = "/mnt/sdcard/ilixi/";
#else
    const char *home = getenv("HOME");
#endif
    if (!home)
    {
        home = getenv("TMPDIR");
        if (!home)
            home = "/tmp";
    }
    return std::string(home);
}

const std::string
ilxDirectory()
{
    static std::string path = PrintF("%s%s", homeDirectory().c_str(), "/.ilixi/");
    struct stat myStat;
    if (!((stat(path.c_str(), &myStat) == 0) && (((myStat.st_mode) & S_IFMT) == S_IFDIR)))
    {
        int status = mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        if (status != 0)
            ILOG_ERROR(ILX_FILESYSTEM, "Cannot create %s - %d\n", path.c_str(), status);
        else
            ILOG_DEBUG(ILX_FILESYSTEM, "Created %s\n", path.c_str());
    }
    return path;
}

std::string
directoryName(const std::string& path)
{
    return std::string(dirname(const_cast<char*>(path.c_str())));
}

std::string
fileName(const std::string& path)
{
    return std::string(basename(const_cast<char*>(path.c_str())));
}

std::vector<std::string>
listDirectory(const std::string& path)
{
    std::vector<std::string> entries;

    DIR *dp = NULL;
    struct dirent *dirp = NULL;
    if ((dp = opendir(path.c_str())) == NULL)
    {
        ILOG_ERROR(ILX_FILESYSTEM, "Error opening %s\n", path.c_str());
        return entries;
    }

    while ((dirp = readdir(dp)) != NULL)
        entries.push_back(std::string(dirp->d_name));
    closedir(dp);

    return entries;
}

FILE*
temporaryFile(const std::string& prefix)
{
    char buffer[L_tmpnam];
    // XX ignoring return value of 'char* tmpnam(char*)'
    tmpnam(buffer);
    FILE * pFile;
    pFile = tmpfile();
    return pFile;
}

void
closeFile(FILE* handle)
{
    fclose(handle);
}

} /* namespace FileSystem */

} /* namespace ilixi */
