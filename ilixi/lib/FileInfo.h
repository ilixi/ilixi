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

#ifndef ILIXI_FILEINFO_H_
#define ILIXI_FILEINFO_H_

#include <sys/stat.h>
#include <string>

namespace ilixi
{
//! Provides information about a file.
class FileInfo
{
public:
    enum FilePermissions
    {
        None = 0x0000,
        ReadOwner = 0x0001,
        WriteOwner = 0x0002,
        ExeOwner = 0x0004,
        ReadGroup = 0x0010,
        WriteGroup = 0x0020,
        ExeGroup = 0x0040,
        ReadOther = 0x0100,
        WriteOther = 0x0200,
        ExeOther = 0x0400
    };

    /*!
     * Constructor.
     */
    FileInfo();

    /*!
     * Constructor.
     *
     * @param file
     */
    FileInfo(const std::string& file);

    /*!
     * Destructor.
     */
    virtual
    ~FileInfo();

    /*!
     * Returns absolute path. Does not include file name.
     */
    std::string
    absolutePath() const;

    /*!
     * Returns the file name and suffix. Does not include path.
     */
    std::string
    baseName() const;

    /*!
     * Returns true if file exists; otherwise returns false.
     */
    bool
    exists() const;

    /*!
     * Returns the file name including path and suffix.
     */
    std::string
    file() const;

    /*!
     * Returns base name of file without path and suffix.
     */
    std::string
    fileName() const;

    /*!
     * Returns true if the file is a directory; otherwise returns false.
     */
    bool
    isDir() const;

    /*!
     * Returns true if the file is a regular file; otherwise returns false.
     */
    bool
    isFile() const;

    /*!
     * Returns true if file is hidden; otherwise returns false.
     */
    bool
    isHidden() const;

    /*!
     * Returns true if file is executable by this process; otherwise returns false.
     */
    bool
    isExecutable() const;

    /*!
     * Returns true if file is readable by this process; otherwise returns false.
     */
    bool
    isReadable() const;

    /*!
     * Returns true if file is a symlink; otherwise returns false.
     */
    bool
    isSymLink() const;

    /*!
     * Returns true if file is writable by this process; otherwise returns false.
     */
    bool
    isWritable() const;

    /*!
     * Returns the time when this file was last accessed, measured in milliseconds since January 1st, 1970 00:00 UTC.
     */
    time_t
    lastAccessed() const;

    /*!
     * Returns the time when this file was last modified, measured in milliseconds since January 1st, 1970 00:00 UTC.
     */
    time_t
    lastModified() const;

    /*!
     * Returns the owner ID of the file. 0 if not ready.
     */
    unsigned int
    ownerID() const;

    /*!
     * Returns the group ID of the file. 0 if not ready.
     */
    unsigned int
    groupID() const;

    /*!
     * Returns the permissions of the file.
     */
    FilePermissions
    permissions() const;

    /*!
     * Refreshes the information about the file.
     */
    void
    refresh();

    /*!
     * Sets the file.
     *
     * Returns true if file exists and can stat.
     */
    bool
    setFile(const std::string& file);

    /*!
     * Returns size in bytes.
     */
    long long
    size() const;

    /*!
     * Returns suffix of file.
     */
    std::string
    suffix() const;

    /*!
     * Returns the absolute path to the file this symlink points to, or an empty string if file is not a symlink.
     */
    std::string
    symLinkTarget() const;

private:
    //! This flag is set to true if file exists and can stat.
    bool _ready;
    //! Cached permissions.
    FilePermissions _perms;
    //! This property stores the path to file.
    std::string _file;
    //! This property stores cached file attributes.
    struct stat _stat;
};

} /* namespace ilixi */
#endif /* ILIXI_FILEINFO_H_ */
