## Makefile.am for /ilixi/ui
INCLUDES 				= 	-I$(top_srcdir)/ilixi -I$(top_builddir)/ilixi
noinst_LTLIBRARIES 		= 	libilixi_ui.la
libilixi_ui_la_CPPFLAGS	= 	$(AM_CPPFLAGS) @DEPS_CFLAGS@ 
libilixi_ui_la_CFLAGS	= 	$(AM_CFLAGS)
libilixi_ui_la_LIBADD 	= 	@DEPS_LIBS@				
libilixi_ui_la_SOURCES 	= 	AppWindow.cpp \
							Button.cpp \
							ButtonGroup.cpp \
							Carousel.cpp \
							CheckBox.cpp \
							ComboBox.cpp \
							ContainerBase.cpp \
							Dialog.cpp \
							DirectionalButton.cpp \
							FileBrowser.cpp \
							Frame.cpp \
							GridLayout.cpp \
							GridView.cpp \
							GroupBox.cpp \
							HBoxLayout.cpp \
							Icon.cpp \
							Label.cpp \
							LayoutBase.cpp \
							LineInput.cpp \
							LineSeperator.cpp \
							ListBox.cpp \
							ProgressBar.cpp \
							PushButton.cpp \
							RadioButton.cpp \
							ScrollArea.cpp \
							ScrollBar.cpp \
							Slider.cpp \
							Spacer.cpp \
							SpinBox.cpp \
							TabPanel.cpp \
							TextBase.cpp \
							ToolBar.cpp \
							ToolButton.cpp \
							VBoxLayout.cpp \
							VideoPlayer.cpp \
							Widget.cpp \
							WindowWidget.cpp
          				
ilixi_includedir 		= 	$(includedir)/$(PACKAGE)-$(VERSION)/ui
ilixi_include_HEADERS 	= 	AppWindow.h \
							Button.h \
							ButtonGroup.h \
							Carousel.h \
							CheckBox.h \
							ComboBox.h \
							ContainerBase.h \
							Dialog.h \
							DirectionalButton.h \
							FileBrowser.h \
							Frame.h \
							GridLayout.h \
							GridView.h \
							GroupBox.h \
							HBoxLayout.h \
							Icon.h \
							Label.h \
							LayoutBase.h \
							LineInput.h \
							LineSeperator.h \
							ListBox.h \
							ProgressBar.h \
							PushButton.h \
							RadioButton.h \
							ScrollArea.h \
							ScrollBar.h \
							Slider.h \
							Spacer.h \
							SpinBox.h \
							TabPanel.h \
							TextBase.h \
							ToolBar.h \
							ToolButton.h \
							VBoxLayout.h \
							VideoPlayer.h \
							Widget.h  \
							WindowWidget.h

if WITH_SURFACE_EVENTS
libilixi_ui_la_SOURCES 	+= 	SurfaceView.cpp
ilixi_include_HEADERS 	+= 	SurfaceView.h
endif
							
if WITH_REFLEX
UI_REFLEX				= 	Application_rflx.cpp \
							Button_rflx.cpp \
							Carousel_rflx.cpp \
							CheckBox_rflx.cpp \
							ComboBox_rflx.cpp \
							ContainerBase_rflx.cpp \
							Dialog_rflx.cpp \
							FileBrowser_rflx.cpp \
							Frame_rflx.cpp \
							GridLayout_rflx.cpp \
							GridView_rflx.cpp \
							GroupBox_rflx.cpp \
							HBoxLayout_rflx.cpp \
							Icon_rflx.cpp \
							Label_rflx.cpp \
							LayoutBase_rflx.cpp \
							LineInput_rflx.cpp \
							ProgressBar_rflx.cpp \
							PushButton_rflx.cpp \
							RadioButton_rflx.cpp \
							ScrollArea_rflx.cpp \
							Slider_rflx.cpp \
							Spacer_rflx.cpp \
							SpinBox_rflx.cpp \
							SurfaceView_rflx.cpp \
							TabPanel_rflx.cpp \
							TextBase_rflx.cpp \
							ToolButton_rflx.cpp \
							VBoxLayout_rflx.cpp \
							Widget_rflx.cpp \
							WindowWidget_rflx.cpp
							
nodist_libilixi_ui_la_SOURCES = $(UI_REFLEX)

include $(top_srcdir)/ilixi/gccxml.make

clean-local:
	-rm $(UI_REFLEX)
endif							