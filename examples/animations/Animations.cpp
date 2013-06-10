#include "Animations.h"
#include <graphics/Painter.h>

using namespace ilixi;

Animations::Animations(int argc, char* argv[])
        : Application(&argc, &argv),
          _affine(false),
          _light(false),
          _fps(NULL)
{
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "--affine") == 0)
        {
            _affine = true;

            TweenAnimation* anim = new TweenAnimation();
            anim->setDuration(2000);
            anim->addTween(Tween::LINEAR, Tween::EASE_IN, 0, 360);
            anim->addTween(Tween::LINEAR, Tween::EASE_OUT, .8, 2);
            anim->sigExec.connect(sigc::mem_fun(this, &Animations::spiralAnimationSlot));
            _bgSeq.addAnimation(anim);

            anim = new TweenAnimation();
            anim->setDuration(2000);
            anim->addTween(Tween::LINEAR, Tween::EASE_IN, 0, 360);
            anim->addTween(Tween::LINEAR, Tween::EASE_OUT, 2, .8);
            anim->sigExec.connect(sigc::mem_fun(this, &Animations::spiralAnimationSlot));
            _bgSeq.addAnimation(anim);

            _bgSeq.setLooping(true);
            _bgSeq.start();
        }
        if (strcmp(argv[i], "--light") == 0)
        {
            _light = true;
        }
    }

    setBackgroundImage(ILIXI_DATADIR"images/ilixi_bg.jpg");
    appWindow()->setCustomCompose(true);

    _fps = new FPSCalculator();

    _frame = new Frame();
    _frame->setZ(5);
    addWidget(_frame);
    sigVisible.connect(sigc::mem_fun(this, &Animations::setFrameAnimation));

    // Add spirals
    _spiral1 = new Spiral();
    _spiral1->setZ(10);
    addWidget(_spiral1);
    if (!_light)
    {
        _spiral2 = new Spiral();
        _spiral2->setZ(10);
        addWidget(_spiral2);

        _spiral3 = new Spiral();
        _spiral3->setZ(10);
        addWidget(_spiral3);

        _spiral4 = new Spiral();
        _spiral4->setZ(10);
        addWidget(_spiral4);
    }
    sigVisible.connect(sigc::mem_fun(this, &Animations::setSpiralAnimation));

    // Add buttons
    _button1 = new PushButton("Button 1");
    _button1->setZ(15);
    addWidget(_button1);

    _button2 = new PushButton("Button 2");
    _button2->setZ(15);
    addWidget(_button2);

    _button3 = new PushButton("Button 3");
    _button3->setZ(15);
    addWidget(_button3);
    sigVisible.connect(sigc::mem_fun(this, &Animations::setButtonAnimation));

    appWindow()->setBackgroundClear(false);
}

Animations::~Animations()
{
    delete _fps;
}

void
Animations::compose(const PaintEvent& event)
{
    _fps->funck();

    Painter p(appWindow());
    p.begin(event);
    if (_affine)
    {
        TweenAnimation* anim = (TweenAnimation*) _bgSeq.currentAnimation();
        Affine2D affine;
        affine.translate(-width() / 2, -height() / 2);
        affine.rotate(anim->tweenValue(0));
        affine.scale(anim->tweenValue(1), anim->tweenValue(1));
        affine.translate(width() / 2, height() / 2);
        p.setAffine2D(affine);
        p.stretchImage(appWindow()->background(), appWindow()->frameGeometry());
        p.setAffine2D();
    } else
        p.stretchImage(appWindow()->background(), appWindow()->frameGeometry());

    p.setBrush(Color(255, 0, 0));
    std::string text = PrintF("FPS: %.1f", _fps->fps());
    Size s = appWindow()->stylist()->defaultFont()->extents(text);
    p.drawText(text, width() - s.width() - 10, 10);
    p.end();
}

void
Animations::setSpiralAnimation()
{
    _spiralW = width() / 3.0;
    _spiralH = height() / 2.5;

    _spiral1->setSize(_spiralW, _spiralH);
    if (!_light)
    {
        _spiral2->setSize(_spiralW, _spiralH);
        _spiral3->setSize(_spiralW, _spiralH);
        _spiral4->setSize(_spiralW, _spiralH);
    }

    TweenAnimation* anim1 = new TweenAnimation();
    anim1->setDuration(2000);
    anim1->addTween(Tween::ELASTIC, Tween::EASE_OUT, 0, _spiralW);
    anim1->addTween(Tween::ELASTIC, Tween::EASE_OUT, 0, _spiralH);
    anim1->addTween(Tween::SINE, Tween::EASE_OUT, 255, 0);
    anim1->sigExec.connect(sigc::mem_fun(this, &Animations::spiralAnimationSlot));
    _spiralSeq.addAnimation(anim1);

    TweenAnimation* anim2 = new TweenAnimation();
    anim2->setDuration(2000);
    anim2->addTween(Tween::BOUNCE, Tween::EASE_OUT, _spiralW, 0);
    anim2->addTween(Tween::BOUNCE, Tween::EASE_OUT, _spiralH, 0);
    anim2->addTween(Tween::BOUNCE, Tween::EASE_OUT, 0, 255);
    anim2->sigExec.connect(sigc::mem_fun(this, &Animations::spiralAnimationSlot));
    _spiralSeq.addAnimation(anim2);

    _spiralSeq.setLooping(true);
    _spiralSeq.start();
}

void
Animations::spiralAnimationSlot()
{
    TweenAnimation* anim = (TweenAnimation*) _spiralSeq.currentAnimation();

    _spiral1->moveTo(anim->tweenValue(0), anim->tweenValue(1));
    _spiral1->setTemp(anim->tweenValue(2));

    if (!_light)
    {
        _spiral2->moveTo(width() - _spiralW - anim->tweenValue(0), anim->tweenValue(1));
        _spiral2->setTemp(anim->tweenValue(2));

        _spiral3->moveTo(anim->tweenValue(0), height() - _spiralH - anim->tweenValue(1));
        _spiral3->setTemp(anim->tweenValue(2));

        _spiral4->moveTo(width() - _spiralW - anim->tweenValue(0), height() - _spiralH - anim->tweenValue(1));
        _spiral4->setTemp(anim->tweenValue(2));
    }

    update();
}

void
Animations::setButtonAnimation()
{
    int x = width() - _button1->width();
    int y = (height() - _button1->height()) / 2;
    int w = _button1->width() + 5;

    _button1->moveTo(0, y);
    _button2->moveTo(w, y);
    _button3->moveTo(2 * w, y);

    TweenAnimation* anim1 = new TweenAnimation();
    anim1->setDuration(1000);
    anim1->addTween(Tween::BACK, Tween::EASE_IN_OUT, 0, x);
    anim1->addTween(Tween::BACK, Tween::EASE_IN_OUT, w, x - w);
    anim1->addTween(Tween::BACK, Tween::EASE_IN_OUT, 2 * w, x - 2 * w);
    anim1->sigExec.connect(sigc::mem_fun(this, &Animations::buttonAnimationSlot));
    _buttonSeq.addAnimation(anim1);

    TweenAnimation* anim2 = new TweenAnimation();
    anim2->setDuration(1000);
    anim2->addTween(Tween::BACK, Tween::EASE_IN_OUT, x, 0);
    anim2->addTween(Tween::BACK, Tween::EASE_IN_OUT, x - w, w);
    anim2->addTween(Tween::BACK, Tween::EASE_IN_OUT, x - 2 * w, 2 * w);
    anim2->sigExec.connect(sigc::mem_fun(this, &Animations::buttonAnimationSlot));
    _buttonSeq.addAnimation(anim2);

    _buttonSeq.setLooping(true);
    _buttonSeq.start();
}

void
Animations::buttonAnimationSlot()
{
    TweenAnimation* anim = (TweenAnimation*) _buttonSeq.currentAnimation();

    _button1->setX(anim->tweenValue(0));
    _button2->setX(anim->tweenValue(1));
    _button3->setX(anim->tweenValue(2));

    update();
}

void
Animations::setFrameAnimation()
{

    int w = width() - 300;
    int h = height() - 200;
    int x = (width() - w) / 2;
    int y = (height() - h) / 2;

    _frame->setGeometry(x, y, w, h);
    _frame->setLayout(new GridLayout(3, _light ? 1 : 3));

    for (int i = 0; i < (_light ? 3 : 9); ++i)
    {
        if (i == 4)
        {
            ToolButton* button = new ToolButton("ToolButton");
            button->setDisabled();
            button->setConstraints(ExpandingConstraint, FixedConstraint);
            _frame->addWidget(button);
        } else
        {
            ToolButton* button = new ToolButton("ToolButton");
            button->setConstraints(ExpandingConstraint, ExpandingConstraint);
            _frame->addWidget(button);
        }
    }

    TweenAnimation* anim1 = new TweenAnimation();
    anim1->setDuration(800);
    anim1->addTween(Tween::BACK, Tween::EASE_IN_OUT, x, x - 50);
    anim1->addTween(Tween::BACK, Tween::EASE_IN_OUT, y, y - 50);
    anim1->addTween(Tween::BACK, Tween::EASE_IN_OUT, w, w + 100);
    anim1->addTween(Tween::BACK, Tween::EASE_IN_OUT, h, h + 100);
    anim1->sigExec.connect(sigc::mem_fun(this, &Animations::frameAnimationSlot));
    _frameSeq.addAnimation(anim1);

    TweenAnimation* anim2 = new TweenAnimation();
    anim2->setDuration(800);
    anim2->addTween(Tween::BACK, Tween::EASE_IN_OUT, x - 50, x);
    anim2->addTween(Tween::BACK, Tween::EASE_IN_OUT, y - 50, y);
    anim2->addTween(Tween::BACK, Tween::EASE_IN_OUT, w + 100, w);
    anim2->addTween(Tween::BACK, Tween::EASE_IN_OUT, h + 100, h);
    anim2->sigExec.connect(sigc::mem_fun(this, &Animations::frameAnimationSlot));
    _frameSeq.addAnimation(anim2);

    _frameSeq.setLooping(true);
    _frameSeq.start();
}

void
Animations::frameAnimationSlot()
{
    TweenAnimation* anim = (TweenAnimation*) _frameSeq.currentAnimation();
    _frame->setGeometry(anim->tweenValue(0), anim->tweenValue(1), anim->tweenValue(2), anim->tweenValue(3));
    _frame->layout()->doLayout();
}

int
main(int argc, char* argv[])
{
    Animations app(argc, argv);
    app.exec();
    return 0;
}
