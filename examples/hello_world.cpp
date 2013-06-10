#include <core/Application.h>
#include <ui/Label.h>

int
main(int argc, char* argv[])
{
    ilixi::Application app(&argc, &argv);
    app.addWidget(new ilixi::Label("Hello World!"));
    app.exec();
    return 0;
}
