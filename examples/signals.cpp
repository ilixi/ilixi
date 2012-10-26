#include <ilixiGUI.h>

using namespace ilixi;

int
main(int argc, char* argv[])
{
    Application app(&argc, &argv);
    app.setMargin(10);
    app.setLayout(new VBoxLayout());

    CheckBox* check = new CheckBox("Show PushButton");
    check->setChecked();
    PushButton* button = new PushButton("PushButton");
    check->sigCheckChanged.connect(sigc::mem_fun(button, &PushButton::setVisible));
    app.addWidget(check);
    app.addWidget(button);

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
