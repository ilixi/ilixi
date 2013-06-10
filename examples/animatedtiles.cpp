#include <math.h>

#include <core/Application.h>
#include <ui/ToolButton.h>
#include <ui/HBoxLayout.h>
#include <lib/FPSCalculator.h>
#include <lib/TweenAnimation.h>
#include <graphics/Painter.h>

using namespace ilixi;

class AnimatedTiles : public Application
{

public:
    AnimatedTiles(int *argc, char ***argv)
            : Application(argc, argv)
    {
        setBackgroundImage(ILIXI_DATADIR"images/ilixi_bg.jpg");
        appWindow()->setCustomCompose(true);

        _image = new Image(ILIXI_DATADIR"images/circle.png");

        _anim = new TweenAnimation();
        _anim->setDuration(2000);
        _anim->addTween(Tween::ELASTIC, Tween::EASE_OUT, 0, 1);
        _anim->sigExec.connect(sigc::mem_fun(this, &AnimatedTiles::animationSlot));

        for (int i = 0; i < 64; i++)
        {
            _pos[i].moveTo(-_image->width(), -_image->height());
        }

        sigVisible.connect(sigc::mem_fun(this, &AnimatedTiles::setStartPosition));

        _fps = new FPSCalculator();

        setMargin(10);
        setLayout(new HBoxLayout());

        ToolButton *b0 = new ToolButton("  [.]  ");
        b0->sigClicked.connect(sigc::mem_fun(this, &AnimatedTiles::button0Handler));
        addWidget(b0);

        ToolButton *b1 = new ToolButton("  [o]  ");
        b1->sigClicked.connect(sigc::mem_fun(this, &AnimatedTiles::button1Handler));
        addWidget(b1);

        ToolButton *b2 = new ToolButton("  [8]  ");
        b2->sigClicked.connect(sigc::mem_fun(this, &AnimatedTiles::button2Handler));
        addWidget(b2);
    }

    void
    button0Handler()
    {
        Size size = _image->size();     // FIXME: make width/height() validate the size as well

        Point dest[64];
        float x = (width() - size.width()) / 2.0f;
        float y = (height() - size.height()) / 2.0f;
        float w = (width() - 200) / 2.0f;
        float h = (height() - 200) / 2.0f;

        for (int i = 0; i < 64; i++)
        {
            dest[i].moveTo(x, y);
        }

        transitionTo(dest);
    }

    void
    button1Handler()
    {
        Size size = _image->size();     // FIXME: make width/height() validate the size as well

        Point dest[64];
        float x = (width() - size.width()) / 2.0f;
        float y = (height() - size.height()) / 2.0f;
        float w = (width() - 200) / 2.0f;
        float h = (height() - 200) / 2.0f;

        for (int i = 0; i < 64; i++)
        {
            dest[i].moveTo(x + cos((i / 63.0) * 6.28) * w, y + sin((i / 63.0) * 6.28) * h);
        }

        transitionTo(dest);
    }

    void
    button2Handler()
    {
        Size size = _image->size();     // FIXME: make width/height() validate the size as well

        Point dest[64];
        float x = (width() - size.width()) / 2.0f;
        float y = (height() - size.height()) / 2.0f;
        float w = (width() - 200) / 2.0f;
        float h = (height() - 200) / 2.0f;

        for (int i = 0; i < 64; i++)
        {
            dest[i].moveTo(x + sin((i / 63.0) * 6.28) * w, y + sin(((i * 2) / 63.0) * 6.28) * h);
        }

        transitionTo(dest);
    }

    void
    animationSlot()
    {
        float f = _anim->tweenValue(0);

        for (int i = 0; i < 64; i++)
        {
            _pos[i].moveTo(f * _dest[i].x() + (1.0f - f) * _last[i].x(), f * _dest[i].y() + (1.0f - f) * _last[i].y());
        }

        update();
    }

    void
    setStartPosition()
    {
        Size size = _image->size();     // FIXME: make width/height() validate the size as well

        for (int i = 0; i < 64; i++)
        {
            _pos[i].moveTo((width() - size.width()) / 2, (height() - size.height()) / 2);
        }

        update();
    }

    void
    transitionTo(const Point *newPos)
    {
        for (int i = 0; i < 64; i++)
        {
            _last[i] = _pos[i];
            _dest[i] = newPos[i];
        }

        _anim->start();
    }

    void
    compose(const ilixi::PaintEvent& event)
    {
        Size size = _image->size();     // FIXME: make width/height() validate the size as well

        Painter p(appWindow());
        p.begin(event);

        p.stretchImage(appWindow()->background(), 0, 0, width(), height());

        DFBRectangle rects[64];
        DFBPoint points[64];

        for (int i = 0; i < 64; i++)
        {
            rects[i].x = 0;
            rects[i].y = 0;
            rects[i].w = size.width();
            rects[i].h = size.height();

            points[i].x = _pos[i].x();
            points[i].y = _pos[i].y();
        }

        p.batchBlitImage(_image, rects, points, 64);

        _fps->funck();

        p.setBrush(Color(255, 0, 0));
        std::string text = PrintF("FPS: %.1f", _fps->fps());

        Size s = appWindow()->stylist()->defaultFont()->extents(text);
        p.drawText(text, width() - s.width() - 10, 10);

        p.end();
    }

    bool
    windowPreEventFilter(const DFBWindowEvent& event)
    {
        switch (event.type)
        {
        case DWET_KEYDOWN:
            switch (event.key_symbol)
            {
            case DIKS_0:
                button0Handler();
                return true;

            case DIKS_1:
                button1Handler();
                return true;

            case DIKS_2:
                button2Handler();
                return true;

            default:
                break;
            }
            break;

        default:
            break;
        }

        return false;
    }

private:
    ilixi::Image *_image;
    Point _last[64];
    Point _pos[64];
    Point _dest[64];
    TweenAnimation *_anim;
    ilixi::FPSCalculator* _fps;
};

int
main(int argc, char* argv[])
{
    AnimatedTiles app(&argc, &argv);
    app.exec();
    return 0;
}
