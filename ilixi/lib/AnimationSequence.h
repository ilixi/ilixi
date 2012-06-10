/*
 * AnimationSequence.h
 *
 *  Created on: Jun 10, 2012
 *      Author: tarik
 */

#ifndef ILIXI_ANIMATIONSEQUENCE_H_
#define ILIXI_ANIMATIONSEQUENCE_H_

#include "lib/Animation.h"
#include <vector>

namespace ilixi
{
  /*!
   * Allows to run animations in sequential order.
   */
  class AnimationSequence
  {
  public:
    /*!
     * Constructor.
     */
    AnimationSequence();

    /*!
     * Destructor.
     */
    virtual
    ~AnimationSequence();

    /*!
     * Returns currently running animation, if any.
     */
    Animation*
    currentAnimation() const;

    /*!
     * Returns the total duration of sequence.
     */
    int
    duration() const;

    /*!
     * Adds animation to the end of sequence.
     */
    bool
    addAnimation(Animation* animation);

    /*!
     * Removes animation from sequence.
     */
    bool
    removeAnimation(Animation* animation);

    /*!
     * Removes all animations.
     */
    void
    clear();

    /*!
     * Starts running sequence.
     */
    void
    start();

    /*!
     * Stops sequence.
     */
    void
    stop();

  private:
    //! This property stores the currently running animation.
    Animation* _current;
    typedef std::vector<Animation*> AnimationVector;
    AnimationVector _animations;

    //! Sets current animation, used by signals.
    void
    setCurrentAnimation(Animation* animation);
  };

} /* namespace ilixi */
#endif /* ILIXI_ANIMATIONSEQUENCE_H_ */
