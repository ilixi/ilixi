#include <ilixiGUI.h>

using namespace ilixi;

int
main(int argc, char* argv[])
{
    Application app(&argc, &argv);
    app.setMargin(10);
    app.setLayout(new VBoxLayout());

    app.addWidget(new CheckBox("Select"));
    app.addWidget(new PushButton("PushButton"));
    app.addWidget(new RadioButton("Option 1"));
    app.addWidget(new RadioButton("Option 2"));
    app.addWidget(new RadioButton("Option 3"));
    app.addWidget(new ToolButton("ToolButton"));
    app.exec();
    return 0;
}
