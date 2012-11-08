#include <ilixiGUI.h>

using namespace ilixi;

int
main(int argc, char* argv[])
{
    Application app(&argc, &argv);
    app.setMargin(10);
    app.setLayout(new HBoxLayout());

    Dialog dialog1("Dialog 1", Dialog::NoButtonOption);
    ToolButton* button1 = new ToolButton("Show dialog 1");
    button1->sigPressed.connect(sigc::mem_fun(&dialog1, &Dialog::execute));
    app.addWidget(button1);

    Dialog dialog2("Dialog 2", Dialog::CancelButtonOption);
    ToolButton* button2 = new ToolButton("Show dialog 2");
    button2->sigClicked.connect(sigc::mem_fun(&dialog2, &Dialog::execute));
    app.addWidget(button2);

    Dialog dialog3("Dialog 3", Dialog::OKCancelButtonOption);
    ToolButton* button3 = new ToolButton("Show dialog 3");
    button3->sigClicked.connect(sigc::mem_fun(&dialog3, &Dialog::execute));
    app.addWidget(button3);

    Dialog dialog4("Dialog 4", Dialog::YesNoCancelButtonOption);
    ToolButton* button4 = new ToolButton("Show dialog 4");
    button4->sigPressed.connect(sigc::mem_fun(&dialog4, &Dialog::execute));
    app.addWidget(button4);

    app.exec();
    return 0;
}
