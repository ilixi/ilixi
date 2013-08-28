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

#ifndef ILIXI_NOTIFY_H_
#define ILIXI_NOTIFY_H_

#include <graphics/Surface.h>
#include <types/TextLayout.h>
#include <string>
#include <sigc++/sigc++.h>

namespace ilixi
{

//! Helps send a notification to compositor.
class Notify
{
public:
    /*!
     * Creates a new notification.
     *
     * @param title of notification, max 128 characters.
     * @param body text for notification, max 256 characters.
     * @param iconPath for image to use, max 256 characters.
     * @param tag is used to tag conceptually similar notifications. If two notifications have the same tag then old one will be replaced by new.
     */
    Notify(const std::string& title, const std::string& body, const std::string& iconPath = "", const std::string& tag = "");

    /*!
     * Destructor.
     */
    virtual
    ~Notify();

    /*!
     * Returns body string.
     */
    const std::string&
    body() const;

    /*!
     * Returns icon path string.
     */
    const std::string&
    icon() const;

    /*!
     * Returns tag string for notification.
     */
    const std::string&
    tag() const;

    /*!
     * Returns title string for notification.
     */
    const std::string&
    title() const;

    /*!
     * Returns UUID for notification.
     */
    const char*
    uuid() const;

    /*!
     * Sets icon path of notification.
     */
    void
    setIcon(const std::string& iconPath);

    /*!
     * Sets body string of notification.
     */
    void
    setBody(const std::string& body);

    /*!
     * Sets tag string of notification.
     */
    void
    setTag(const std::string& tag);

    /*!
     * Sets title string of notification.
     */
    void
    setTitle(const std::string& title);

    /*!
     * Show notification.
     *
     * \warning notification will actually be visible when display has enough space.
     */
    void
    show();

    /*!
     * This signal is emitted when user clicks on notification.
     */
    sigc::signal<void> sigClick;

    /*!
     * This signal is emitted when notification becomes visible.
     */
    sigc::signal<void> sigShow;

    /*!
     * This signal is emitted if icon URL is invalid.
     */
    sigc::signal<void> sigError;

    /*!
     * This signal is emitted when notification becomes hidden.
     */
    sigc::signal<void> sigClose;

private:
    //! This property stores the title of notification.
    std::string _title;
    //! This property stores the body text of notification.
    std::string _body;
    //! This property stores the path to image.
    std::string _icon;
    //! This property stores the tag for the notification.
    std::string _tag;
    //! This property stores the UUID for the notification.
    char _uuid[37];
};

} /* namespace ilixi */
#endif /* ILIXI_NOTIFY_H_ */
