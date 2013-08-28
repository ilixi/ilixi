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

#ifndef ILIXI_SWITCHER_H_
#define ILIXI_SWITCHER_H_

#include <compositor/AppThumbnail.h>

namespace ilixi
{

class ILXCompositor;

//! Allows switching applications using thumbnails.
class Switcher : public Widget
{
public:
    /*!
     * Constructor.
     */
    Switcher(ILXCompositor* compositor, Widget* parent = 0);

    /*!
     * Destructor.
     */
    virtual
    ~Switcher();

    /*!
     * Returns preferred size for switcher.
     */
    virtual Size
    preferredSize() const;

    unsigned int
    itemCount() const;

    /*!
     * Adds a thumb to switcher.
     *
     * Derived classes should add thumbs to their widget tree and connect
     * sigSwitchRequest.
     */
    virtual void
    addThumb(AppThumbnail* thumb) =0;

    /*!
     * Removes thumb from switcher.
     *
     * Derived classes should remove thumbs from their widget tree.
     */
    virtual void
    removeThumb(AppThumbnail* thumb) =0;

    /*!
     * Scrolls to given thumb.
     */
    virtual void
    scrollTo(AppThumbnail* thumb) = 0;

    /*!
     * Makes switcher visible.
     */
    virtual void
    show();

    /*!
     * Makes switcher invisible.
     */
    virtual void
    hide();

    /*!
     * Scrolls to next thumbnail.
     */
    virtual void
    scrollToNext();

    /*!
     * Scrolls to previous thumbnail.
     */
    virtual void
    scrollToPrevious();

    /*!
     * Currently selected thumbnail.
     */
    virtual AppThumbnail*
    currentThumb();

    /*!
     * Returns next thumbnail.
     */
    AppThumbnail*
    nextThumb();

    /*!
     * Returns previous thumbnail.
     */
    AppThumbnail*
    previousThumb();

    void
    setCurrentThumb(AppThumbnail*);

    const Rectangle&
    getSwitcherGeometry() const;

    void
    setSwitcherGeometry(const Rectangle& rect);

    virtual void
    killCurrentApp();

    /*!
     * This signal is used to notify compositor.
     */
    sigc::signal<void, AppInstance*> sigSwitchRequest;

    /*!
     * This signal is emitted for each application which requires a
     * visibility feedback.
     */
    sigc::signal<void, AppInstance*, bool> sigFeedbackRequest;

protected:
    ILXCompositor* _compositor;
    typedef std::vector<AppThumbnail*> Thumbnails;
    //! Thumbnails are also stored in a vector for ease of access.
    Thumbnails _thumbs;

    int _currentIndex;

    Rectangle _switcherGeometry;

};

} /* namespace ilixi */
#endif /* ILIXI_SWITCHER_H_ */
