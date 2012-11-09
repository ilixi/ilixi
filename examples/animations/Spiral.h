/*
 * Spiral.h
 *
 *  Created on: Nov 8, 2012
 *      Author: tarik
 */

#ifndef SPIRAL_H_
#define SPIRAL_H_

#include <ui/Widget.h>

class Spiral : public ilixi::Widget
{
public:
    Spiral(Widget* parent = 0);

    virtual
    ~Spiral();

    void
    setTemp(int temp);

protected:
    virtual void
    compose(const ilixi::PaintEvent& event);

private:
    int _temp;
    ilixi::Image* _image;
};

#endif /* SPIRAL_H_ */
