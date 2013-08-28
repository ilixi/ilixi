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

#ifndef ILIXI_INPUTHELPER_H_
#define ILIXI_INPUTHELPER_H_

#include <stdint.h>
#include <string>
#include <vector>
#include <sigc++/signal.h>

namespace ilixi
{

/*!
 * This class provides an API to input in other languages.
 */
class InputHelper : virtual public sigc::trackable
{
public:
    /*!
     * Constructor.
     */
    InputHelper();

    /*!
     * Destructor.
     */
    virtual
    ~InputHelper();

    /*!
     * Sets input data (UTF8) for conversion engine.
     */
    void
    setInputData(std::string data);

    /*!
     * Appends symbol to input data (UTF8).
     *
     * This method is called when a new symbol is typed.
     */
    void
    append(uint32_t symbol);

    /*!
     * Returns raw symbol data stored for processing.
     */
    const std::string&
    getData() const;

    /*!
     * Returns pre-processed symbol data.
     */
    const std::string&
    getPdata() const;

    /*!
     * Returns current segment.
     */
    int
    getCurrentSegment() const;

    /*!
     * Sets current segment.
     */
    void
    setCurrentSegment(int currentSegment);

    /*!
     * Returns number of segments.
     */
    unsigned int
    segments() const;

    /*!
     * Returns concatenated segments.
     */
    std::string
    concatedSegments() const;

    /*!
     * Returns segment at given index.
     */
    std::string
    getSegment(unsigned int index);

    /*!
     * Increments current segment index.
     */
    void
    getNextSegment();

    /*!
     * Decrements current segment index.
     */
    void
    getPreviousSegment();

    /*!
     * Returns number of candidates for a current segment.
     */
    unsigned int
    canditates() const;

    /*!
     * Returns the candidate for current segment at index.
     */
    std::string
    getCanditate(unsigned int index);

    /*!
     * Increments current candidate index for current segment.
     */
    void
    getNextCandidate();

    /*!
     * Decrements current candidate index for current segment.
     */
    void
    getPreviousCandidate();

    /*!
     * This method should run conversion logic and generate segments.
     */
    virtual void
    process()=0;

    /*!
     * This method should resize current segment.
     *
     * You should call generateSegments() in your implementation if necessary.
     *
     * @param direction -1 for left, 1 for right.
     */
    virtual void
    resizeSegment(int direction) =0;

    /*!
     * This method should fill segments vector.
     *
     * @param segmentCount number of segments.
     */
    virtual void
    generateSegments(int segmentCount)=0;

    /*!
     * This method should fill candidates vector for current segment.
     */
    virtual void
    generateCandidates()=0;

    /*!
     * Copies current candidate text to current segment.
     */
    void
    updateCurrentSegment();

    /*!
     * Reset all data.
     */
    void
    reset();

    //! This signal is emitted when a UI update is needed.
    sigc::signal<void> sigUpdateUI;

protected:
    //! This property stores the current segment.
    int _currentSegment;
    //! This property stores the current candidate.
    int _currentCandidate;
    //! This property stores the raw input data (UTF8).
    std::string _data;
    //! This property stores the preprocessed input data (UTF8).
    std::string _pdata;

    typedef std::vector<std::string> SegmentVector;
    typedef std::vector<std::string> CandidateVector;

    //! This vector stores conversion segments.
    SegmentVector _segments;
    //! This vector stores conversion candidates for current segment.
    CandidateVector _candidates;

    //! This method is called once input data is set.
    virtual void
    preProcessInputData();

};

} /* namespace ilixi */
#endif /* ILIXI_INPUTHELPER_H_ */
