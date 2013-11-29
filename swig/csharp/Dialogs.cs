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
        DelegateNotify a = new DelegateNotify(dialog1.execute);
        button1.sigPressed.connect(button1, a);
        app.addWidget(button1);

        Dialog dialog2 = new Dialog("Dialog 2", Dialog.ButtonOption.CancelButtonOption);
        ToolButton button2 = new ToolButton("Show dialog 2");
        DelegateNotify b = new DelegateNotify(dialog2.execute);
        button2.sigClicked.connect(button2, b);
        app.addWidget(button2);

        Dialog dialog3 = new Dialog("Dialog 3", Dialog.ButtonOption.OKCancelButtonOption);
        ToolButton button3 = new ToolButton("Show dialog 3");
        button3.sigClicked.connect(button3, dialog3.execute);
        app.addWidget(button3);

        Dialog dialog4 = new Dialog("Dialog 4", Dialog.ButtonOption.YesNoCancelButtonOption);
        ToolButton button4 = new ToolButton("Show dialog 4");
        button4.sigPressed.connect(button4, dialog4.execute);
        app.addWidget(button4);

        app.exec();
    }
}
