/*
 Copyright 2010-2013 Tarik Sekmen.

 All Rights Reserved.

 Written by Tarik Sekmen <tarik@ilixi.org>,
            Andreas Shimokawa <andi@directfb.org>.

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

#ifndef ILIXI_INPUTHELPERJP_H_
#define ILIXI_INPUTHELPERJP_H_

#include <lib/InputHelper.h>
#include <ilixiConfig.h>
#include <map>
#if ILIXI_HAVE_LIBWNN
extern "C"
{
#include <wnn/jllib.h>
}
#endif

namespace ilixi
{

/*!
 * Japanese input helper.
 */
class InputHelperJP : public ilixi::InputHelper
{
public:
    /*!
     * Constructor.
     */
    InputHelperJP();

    /*!
     * Destructor.
     */
    virtual
    ~InputHelperJP();

    /*!
     * Implemented for wnn.
     *
     * Converts utf8 hiragana to euc-jp.
     */
    void
    process();

    /*!
     * Implemented for wnn.
     */
    void
    resizeSegment(int direction);

protected:
    /*!
     * Implemented for wnn.
     */
    virtual void
    generateSegments(int segmentCount);

    /*!
     * Implemented for wnn.
     */
    virtual void
    generateCandidates();

private:
    //! This map is used to convert romaji to hiragana.
    std::map<std::string, std::string> _hiraganaMap;

#if ILIXI_HAVE_LIBWNN
    //! WNN handle.
    wnn_buf* _wnn;
    //! stores text in EUC-JP.
    w_char _ws[1024];
#endif

    //! Converts romaji to hiragana.
    void
    preProcessInputData();

    //! Initialise romaji->hiragana map.
    void
    initHiraganaMap();
};

} /* namespace ilixi */
#endif /* ILIXI_INPUTHELPERJP_H_ */
