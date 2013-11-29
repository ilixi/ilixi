using System;
using ilixi;

class MainClass
{
    public static void Main (string[] args)
    {
         Application app = new Application(null, null);
         app.setMargin(new Margin(10));
         app.setLayout(new VBoxLayout());
         app.addWidget(new CheckBox("Select"));
         app.addWidget(new PushButton("PushButton"));
         app.addWidget(new RadioButton("Option 1"));
         app.addWidget(new RadioButton("Option 2"));
         app.addWidget(new RadioButton("Option 3"));
         app.addWidget(new ToolButton("ToolButton"));
         app.exec();
    }
}
