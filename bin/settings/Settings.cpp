/*
 * Settings.cpp
 *
 *  Created on: Sep 6, 2012
 *      Author: tarik
 */

#include "Settings.h"
#include <ui/VBoxLayout.h>
#include <ui/GridLayout.h>
#include <ui/Spacer.h>
#include <core/ComponentData.h>

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <stdio.h>
#include <stdlib.h>

namespace ilixi
{

Settings::Settings(int argc, char* argv[])
        : Application(&argc, &argv, OptDale),
          _comp(NULL)
{
    setTitle("Compositor Settings");
    setBackgroundImage(ILIXI_DATADIR"compositor/bg.png");
    setMargin(20);
    setLayout(new VBoxLayout());

    _clickToFocus = new CheckBox("Click to focus");
    _enableAnimations = new CheckBox("Enable Animations");

    _showZoom = new CheckBox("Zoom");
    _showOpacity = new CheckBox("Opacity");
    _showDuration = new SpinBox(300);
    _showDuration->setPostfix(" ms");
    _showDuration->setRange(100, 1000);
    _showDuration->setStep(100);

    _hideZoom = new CheckBox("Zoom");
    _hideOpacity = new CheckBox("Opacity");
    _hideDuration = new SpinBox(300);
    _hideDuration->setPostfix(" ms");
    _hideDuration->setRange(100, 1000);
    _hideDuration->setStep(100);

    _showZoomTransition = new ComboBox("Zoom Transition");
    _showOpacityTransition = new ComboBox("Opacity Transition");

    _brightness = new Slider();

    HBoxLayout* buttons = new HBoxLayout();
    buttons->addWidget(new Spacer(Horizontal));
    _reset = new PushButton("Default");
    _reset->sigClicked.connect(sigc::mem_fun(this, &Settings::setDefaults));
    buttons->addWidget(_reset);
    _apply = new PushButton("Apply");
    _apply->sigClicked.connect(sigc::mem_fun(this, &Settings::sendSettings));
    buttons->addWidget(_apply);

    setDefaults();

    GroupBox* box = new GroupBox("Compositor Settings");
    addWidget(box);
    box->setLayout(new VBoxLayout());
    box->addWidget(_clickToFocus);
    box->addWidget(_enableAnimations);

    box->addWidget(new Label("Animation options for showing application"));
    box->addWidget(_showZoom);
    box->addWidget(_showOpacity);
    box->addWidget(_showDuration);

    box->addWidget(new Label("Animation options for hiding application"));
    box->addWidget(_hideZoom);
    box->addWidget(_hideOpacity);
    box->addWidget(_hideDuration);

    box->addWidget(_showZoomTransition);
    box->addWidget(_showOpacityTransition);
    box->addWidget(new Label("Brightness"));
    box->addWidget(_brightness);
    box->addWidget(new Spacer(Vertical));
    box->addWidget(buttons);

    DaleDFB::comaGetComponent("Compositor", &_comp);
}

Settings::~Settings()
{
    if (_comp)
        _comp->Release(_comp);
}

void
Settings::setDefaults()
{
    _clickToFocus->setChecked(true);
    _enableAnimations->setChecked(true);
    _showZoom->setChecked(false);
    _showOpacity->setChecked(true);
    _hideZoom->setChecked(true);
    _hideOpacity->setChecked(true);
    _brightness->setValue(100);
}

void
Settings::sendSettings()
{
//    if (_comp)
    {
        xmlChar* xmlbuff;
        int buffersize;
        char value[100];

        xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
        xmlNodePtr root_node = xmlNewNode(NULL, BAD_CAST "Compositor");
        xmlDocSetRootElement(doc, root_node);

        xmlNodePtr node = xmlNewChild(root_node, NULL, BAD_CAST "ClickToFocus",
                                      NULL);
        snprintf(value, 100, "%d", _clickToFocus->checked());
        xmlNewProp(node, BAD_CAST "enabled", BAD_CAST value);

        node = xmlNewChild(root_node, NULL, BAD_CAST "Animations", NULL);
        snprintf(value, 100, "%d", _enableAnimations->checked());
        xmlNewProp(node, BAD_CAST "enabled", BAD_CAST value);

        xmlNodePtr node1 = xmlNewChild(node, NULL, BAD_CAST "Showing", NULL);
        snprintf(value, 100, "%d", _showDuration->value());
        xmlNewProp(node1, BAD_CAST "duration", BAD_CAST value);
        snprintf(value, 100, "%d", _showZoom->checked());
        xmlNewProp(node1, BAD_CAST "zoom", BAD_CAST value);
        snprintf(value, 100, "%d", _showOpacity->checked());
        xmlNewProp(node1, BAD_CAST "opacity", BAD_CAST value);

        xmlNodePtr node2 = xmlNewChild(node, NULL, BAD_CAST "Hiding", NULL);
        snprintf(value, 100, "%d", _hideDuration->value());
        xmlNewProp(node2, BAD_CAST "duration", BAD_CAST value);
        snprintf(value, 100, "%d", _hideZoom->checked());
        xmlNewProp(node2, BAD_CAST "zoom", BAD_CAST value);
        snprintf(value, 100, "%d", _hideOpacity->checked());
        xmlNewProp(node2, BAD_CAST "opacity", BAD_CAST value);

        node = xmlNewChild(root_node, NULL, BAD_CAST "Brightness", NULL);
        snprintf(value, 100, "%d", int(_brightness->value()));
        xmlNewProp(node, BAD_CAST "value", BAD_CAST value);

        xmlDocDumpFormatMemory(doc, &xmlbuff, &buffersize, 1);

        void *ptr;
        DaleDFB::comaGetLocal(sizeof(char) * buffersize, &ptr);
        char* settings = (char*) ptr;
        snprintf(settings, buffersize, "%s", (char*) xmlbuff);
        DaleDFB::comaCallComponent(_comp, Compositor::SetOptions,
                                   (void*) settings);

        xmlFree(xmlbuff);
        xmlFreeDoc(doc);
    }
}

} /* namespace ilixi */

int
main(int argc, char* argv[])
{
    ilixi::Settings app(argc, argv);
    app.exec();
    return 0;
}
