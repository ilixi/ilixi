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

    app.addWidget(new Spacer(Vertical));

    GroupBox* group = new GroupBox("Options");
    VBoxLayout* vbox = new VBoxLayout();
    vbox->setSpacing(20);
    group->setLayout(vbox);
    group->addWidget(new RadioButton("Option 1"));
    group->addWidget(new RadioButton("Option 2"));
    group->addWidget(new RadioButton("Option 3"));
    app.addWidget(group);

    app.addWidget(new Spacer(Vertical));

    app.addWidget(new ToolButton("ToolButton"));
    app.exec();
    return 0;
}
