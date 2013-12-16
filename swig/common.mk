SWIG_SOURCES 	= 	../api/Application.i \
					../api/Button.i \
					../api/CheckBox.i \
					../api/ComboBox.i \
					../api/ContainerBase.i \
					../api/Dialog.i \
					../api/Enums.i \
					../api/Font.i \
					../api/Frame.i \
					../api/GridLayout.i \
					../api/HBoxLayout.i \
					../api/Label.i \
					../api/LayoutBase.i \
					../api/LineInput.i \
					../api/Margin.i \
					../api/PushButton.i \
					../api/RadioButton.i \
					../api/Rectangle.i \
					../api/sigc_signal.i \
					../api/Size.i \
					../api/Spacer.i \
					../api/SpinBox.i \
					../api/TabPanel.i \
					../api/TextBase.i \
					../api/ToolButton.i \
					../api/VBoxLayout.i \
					../api/Widget.i \
					../api/WindowWidget.i
							
GEN_SOURCES		= 	./gen/ilixi_Application.cpp \
					./gen/ilixi_Button.cpp \
					./gen/ilixi_CheckBox.cpp \
					./gen/ilixi_ComboBox.cpp \
					./gen/ilixi_ContainerBase.cpp \
					./gen/ilixi_Dialog.cpp \
					./gen/ilixi_Enums.cpp \
					./gen/ilixi_Font.cpp \
					./gen/ilixi_Frame.cpp \
					./gen/ilixi_GridLayout.cpp \
					./gen/ilixi_HBoxLayout.cpp \
					./gen/ilixi_Label.cpp \
					./gen/ilixi_LayoutBase.cpp \
					./gen/ilixi_LineInput.cpp \
					./gen/ilixi_Margin.cpp \
					./gen/ilixi_PushButton.cpp \
					./gen/ilixi_RadioButton.cpp \
					./gen/ilixi_Rectangle.cpp \
					./gen/ilixi_sigc_signal.cpp \
					./gen/ilixi_Size.cpp \
					./gen/ilixi_Spacer.cpp \
					./gen/ilixi_SpinBox.cpp \
					./gen/ilixi_TabPanel.cpp \
					./gen/ilixi_TextBase.cpp \
					./gen/ilixi_ToolButton.cpp \
					./gen/ilixi_VBoxLayout.cpp \
					./gen/ilixi_Widget.cpp \
					./gen/ilixi_WindowWidget.cpp
					

#if WITH_COMPOSITOR
#SWIG_SOURCES 	+= 	../api/Notify.i
#GEN_SOURCES		+=	./gen/ilixi_Notify.cpp
#endif