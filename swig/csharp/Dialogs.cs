using System;
using ilixi;

class MainClass
{
    public static void Main (string[] args)
    {
        Application app = new Application(null, null);
        app.setMargin(new Margin(10));
        app.setLayout(new VBoxLayout());

        Dialog dialog1 = new Dialog("Dialog 1", Dialog.ButtonOption.NoButtonOption);
        ToolButton button1 = new ToolButton("Show dialog 1");
        button1.connectClick(new DelegateNotify(dialog1.execute));
        app.addWidget(button1);

        Dialog dialog2 = new Dialog("Dialog 2", Dialog.ButtonOption.CancelButtonOption);
        ToolButton button2 = new ToolButton("Show dialog 2");
        button2.connectClick(new DelegateNotify(dialog2.execute));
        app.addWidget(button2);

        Dialog dialog3 = new Dialog("Dialog 3", Dialog.ButtonOption.OKCancelButtonOption);
        ToolButton button3 = new ToolButton("Show dialog 3");
        button3.connectClick(new DelegateNotify(dialog3.execute));
        app.addWidget(button3);

        Dialog dialog4 = new Dialog("Dialog 4", Dialog.ButtonOption.YesNoCancelButtonOption);
        ToolButton button4 = new ToolButton("Show dialog 4");
        button4.connectClick(new DelegateNotify(dialog4.execute));
        app.addWidget(button4);

        app.exec();
    }
}
