/*
 * Stacker.h
 *
 *  Created on: Apr 3, 2012
 *      Author: tarik
 */

#ifndef STACKER_H_
#define STACKER_H_

#include "ui/Application.h"
#include "ui/PushButton.h"

namespace ilixi
{

  class Stacker : public Application
  {
  public:
    Stacker(int argc, char* argv[]);

    virtual
    ~Stacker();

    void
    sendBack();

    void
    bringForward();

    void
    raise();

    void
    lower();
  private:
    PushButton* pb1;
  };

} /* namespace ilixi */
#endif /* STACKER_H_ */
