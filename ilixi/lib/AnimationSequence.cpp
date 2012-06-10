/*
 * AnimationSequence.cpp
 *
 *  Created on: Jun 10, 2012
 *      Author: tarik
 */

#include "AnimationSequence.h"
#include <algorithm>
#include <sigc++/bind.h>
#include "core/Logger.h"

namespace ilixi
{

  AnimationSequence::AnimationSequence() :
      _current(NULL)
  {
  }

  AnimationSequence::~AnimationSequence()
  {
    clear();
  }

  Animation*
  AnimationSequence::currentAnimation() const
  {
    return _current;
  }

  int
  AnimationSequence::duration() const
  {
    int d = 0;
    for (AnimationVector::const_iterator it = _animations.begin();
        it != _animations.end(); ++it)
      d += ((Animation*) *it)->duration();
    return d;
  }

  bool
  AnimationSequence::addAnimation(Animation* animation)
  {
    if (!animation)
      return false;

    AnimationVector::iterator it = std::find(_animations.begin(),
        _animations.end(), animation);

    if (it != _animations.end())
      {
        ILOG_WARNING(ILX_ANIMATIONSEQ,
            "Animation %p is already in sequence.", animation);
        return false;
      }

    animation->sigStarted.connect(
        sigc::bind<Animation*>(
            sigc::mem_fun(this, &AnimationSequence::setCurrentAnimation),
            animation));

    _animations.push_back(animation);

    if (_animations.size() > 1)
      _animations[_animations.size() - 2]->sigFinished.connect(
          sigc::mem_fun(_animations.back(), &Animation::start));
    return true;
  }

  bool
  AnimationSequence::removeAnimation(Animation* animation)
  {
    // TODO disconnect signals.
    if (!animation)
      return false;

    AnimationVector::iterator it = std::find(_animations.begin(),
        _animations.end(), animation);
    if (it != _animations.end())
      {
        delete *it;
        _animations.erase(it);
        ILOG_DEBUG(ILX_ANIMATIONSEQ, "Removed animation %p\n", animation);
        return true;
      }
    return false;
  }

  void
  AnimationSequence::clear()
  {
    for (AnimationVector::iterator it = _animations.begin();
        it != _animations.end(); ++it)
      delete *it;
  }

  void
  AnimationSequence::start()
  {
    if (_animations.size() && !_current)
      {
        _current = _animations[0];
        _current->start();
      }
  }

  void
  AnimationSequence::stop()
  {
    if (_current)
      {
        _current->stop();
        _current = NULL;
      }
  }

  void
  AnimationSequence::setCurrentAnimation(Animation* animation)
  {
    _current = animation;
  }

} /* namespace ilixi */
