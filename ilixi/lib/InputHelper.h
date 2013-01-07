/*
 Copyright 2010-2012 Tarik Sekmen.

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

#ifndef ILIXI_INPUTHELPER_H_
#define ILIXI_INPUTHELPER_H_

#include <stdint.h>
#include <string>
#include <vector>
#include <sigc++/signal.h>

namespace ilixi
{

/*!
 *
 */
class InputHelper
{
public:
    InputHelper();

    virtual
    ~InputHelper();

    void
    append(uint32_t symbol);

    std::wstring
    conversion() const;

    unsigned int
    canditates() const;

    std::wstring
    getCanditate(unsigned int index);

    //! This signal is emitted when a conversion is successful.
    sigc::signal<void> sigConverted;

protected:
    //! This property stores input for processing.
    std::string _buffer;
    //! This property stores converted text.
    std::wstring _conversion;
    //! This vector stores conversion candidates.
    std::vector<std::wstring> _candidates;
    //! This property stores the number of chars processed from buffer at each conversion step.
    std::list<unsigned int> _processed;
    //! This property stores the number of inserted chars into conversion at each step.
    std::list<unsigned int> _steps;

private:
    //! Implement this method.
    /*!
     *
     * @return false if unable to convert.
     */
    virtual bool
    convert(uint32_t symbol)=0;
};

} /* namespace ilixi */
#endif /* ILIXI_INPUTHELPER_H_ */
