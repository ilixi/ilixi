#ifndef ANIMATIONS_H_
#define ANIMATIONS_H_

#include <core/Application.h>
#include <ilixiGUI.h>
#include <lib/AnimationSequence.h>
#include <lib/TweenAnimation.h>
#include <lib/FPSCalculator.h>
#include "Spiral.h"

class Animations : public ilixi::Application
{
public:
    Animations(int argc, char* argv[]);

    virtual
    ~Animations();

protected:
    void
    compose(const ilixi::PaintEvent& event);

private:
    bool _affine;
    bool _light;
    ilixi::FPSCalculator* _fps;

    int _spiralW;
    int _spiralH;
    Spiral* _spiral1;
    Spiral* _spiral2;
    Spiral* _spiral3;
    Spiral* _spiral4;
    ilixi::AnimationSequence _spiralSeq;

    ilixi::PushButton* _button1;
    ilixi::PushButton* _button2;
    ilixi::PushButton* _button3;
    ilixi::PushButton* _button4;
    ilixi::AnimationSequence _buttonSeq;

    ilixi::AnimationSequence _bgSeq;

    ilixi::Frame* _frame;
    ilixi::AnimationSequence _frameSeq;

    void
    setSpiralAnimation();

    void
    spiralAnimationSlot();

    void
    setButtonAnimation();

    void
    buttonAnimationSlot();

    void
    setFrameAnimation();

    void
    frameAnimationSlot();
};

#endif /* ANIMATIONS_H_ */
