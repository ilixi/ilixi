######################################## libxml2 #############################

LIBXML2_SRC_PATH:= libxml2-2.9.1
LIBXML2_INCLUDES:= -I ../$(LIBXML2_SRC_PATH) -I ../$(LIBXML2_SRC_PATH)/include

LIBXML2_SOURCES := \
	$(LIBXML2_SRC_PATH)/SAX.c	\
	$(LIBXML2_SRC_PATH)/entities.c	\
	$(LIBXML2_SRC_PATH)/encoding.c	\
	$(LIBXML2_SRC_PATH)/error.c	\
	$(LIBXML2_SRC_PATH)/parserInternals.c	\
	$(LIBXML2_SRC_PATH)/parser.c	\
	$(LIBXML2_SRC_PATH)/tree.c	\
	$(LIBXML2_SRC_PATH)/hash.c	\
	$(LIBXML2_SRC_PATH)/list.c	\
	$(LIBXML2_SRC_PATH)/xmlIO.c	\
	$(LIBXML2_SRC_PATH)/xmlmemory.c	\
	$(LIBXML2_SRC_PATH)/uri.c	\
	$(LIBXML2_SRC_PATH)/valid.c	\
	$(LIBXML2_SRC_PATH)/xlink.c	\
	$(LIBXML2_SRC_PATH)/HTMLparser.c	\
	$(LIBXML2_SRC_PATH)/HTMLtree.c	\
	$(LIBXML2_SRC_PATH)/debugXML.c	\
	$(LIBXML2_SRC_PATH)/xpath.c	\
	$(LIBXML2_SRC_PATH)/xpointer.c	\
	$(LIBXML2_SRC_PATH)/xinclude.c	\
	$(LIBXML2_SRC_PATH)/catalog.c	\
	$(LIBXML2_SRC_PATH)/globals.c	\
	$(LIBXML2_SRC_PATH)/threads.c	\
	$(LIBXML2_SRC_PATH)/c14n.c	\
	$(LIBXML2_SRC_PATH)/xmlstring.c	\
	$(LIBXML2_SRC_PATH)/buf.c	\
	$(LIBXML2_SRC_PATH)/xmlregexp.c	\
	$(LIBXML2_SRC_PATH)/xmlschemas.c	\
	$(LIBXML2_SRC_PATH)/xmlschemastypes.c	\
	$(LIBXML2_SRC_PATH)/xmlunicode.c	\
	$(LIBXML2_SRC_PATH)/xmlreader.c	\
	$(LIBXML2_SRC_PATH)/relaxng.c	\
	$(LIBXML2_SRC_PATH)/dict.c	\
	$(LIBXML2_SRC_PATH)/SAX2.c	\
	$(LIBXML2_SRC_PATH)/xmlwriter.c	\
	$(LIBXML2_SRC_PATH)/legacy.c	\
	$(LIBXML2_SRC_PATH)/chvalid.c	\
	$(LIBXML2_SRC_PATH)/pattern.c	\
	$(LIBXML2_SRC_PATH)/xmlsave.c	\
	$(LIBXML2_SRC_PATH)/schematron.c	\
	$(LIBXML2_SRC_PATH)/xzlib.c	\

######################################## libsigc++ ###########################

LIBSIGCXX_SRC_PATH:= libsigc++-2.2.10/sigc++
LIBSIGCXX_INCLUDES:= \
	-I ../$(LIBSIGCXX_SRC_PATH)	\
	-I ../$(LIBSIGCXX_SRC_PATH)/functors	\
	-I ../$(LIBSIGCXX_SRC_PATH)/functors/lamda	\
	-I ../$(LIBSIGCXX_SRC_PATH)/..

LIBSIGCXX_SOURCES := \
	$(LIBSIGCXX_SRC_PATH)/signal.cc \
	$(LIBSIGCXX_SRC_PATH)/signal_base.cc \
	$(LIBSIGCXX_SRC_PATH)/connection.cc \
	$(LIBSIGCXX_SRC_PATH)/trackable.cc \
	$(LIBSIGCXX_SRC_PATH)/functors/slot_base.cc \
	$(LIBSIGCXX_SRC_PATH)/functors/slot.cc \
	$(LIBSIGCXX_SRC_PATH)/adaptors/lambda/lambda.cc

################################ fontconfig ##################################

CFLAGS += -DFONTCONFIG_PATH=\"/sdcard/.fcconfig\" -DFC_CACHEDIR=\"/sdcard/.fccache\" -DFC_DEFAULT_FONTS=\"/system/fonts\" -DENABLE_LIBXML2

LIBFONTCONFIG_SRC_PATH:= fontconfig-android
LIBFONTCONFIG_INCLUDES:= \
        -I ../$(LIBFONTCONFIG_SRC_PATH)	\
        -I ../$(LIBFONTCONFIG_SRC_PATH)/src

LIBFONTCONFIG_SOURCES := \
	$(LIBFONTCONFIG_SRC_PATH)/src/fcatomic.c \
	$(LIBFONTCONFIG_SRC_PATH)/src/fcblanks.c \
	$(LIBFONTCONFIG_SRC_PATH)/src/fccache.c \
	$(LIBFONTCONFIG_SRC_PATH)/src/fccfg.c \
	$(LIBFONTCONFIG_SRC_PATH)/src/fccharset.c \
	$(LIBFONTCONFIG_SRC_PATH)/src/fcdbg.c \
	$(LIBFONTCONFIG_SRC_PATH)/src/fcdefault.c \
	$(LIBFONTCONFIG_SRC_PATH)/src/fcdir.c \
	$(LIBFONTCONFIG_SRC_PATH)/src/fcformat.c \
	$(LIBFONTCONFIG_SRC_PATH)/src/fcfreetype.c \
	$(LIBFONTCONFIG_SRC_PATH)/src/fcfs.c \
	$(LIBFONTCONFIG_SRC_PATH)/src/fcinit.c \
	$(LIBFONTCONFIG_SRC_PATH)/src/fclang.c \
	$(LIBFONTCONFIG_SRC_PATH)/src/fclist.c \
	$(LIBFONTCONFIG_SRC_PATH)/src/fcmatch.c \
	$(LIBFONTCONFIG_SRC_PATH)/src/fcmatrix.c \
	$(LIBFONTCONFIG_SRC_PATH)/src/fcname.c \
	$(LIBFONTCONFIG_SRC_PATH)/src/fcpat.c \
	$(LIBFONTCONFIG_SRC_PATH)/src/fcserialize.c \
	$(LIBFONTCONFIG_SRC_PATH)/src/fcstr.c \
	$(LIBFONTCONFIG_SRC_PATH)/src/fcxml.c \
	$(LIBFONTCONFIG_SRC_PATH)/src/ftglue.c

################################ ilixi #######################################

LOCAL_CPP_FEATURES += exceptions
INCLUDES +=  -I$(ILIXI_INCLUDE_PATH)/build-android/ilixi -I$(ILIXI_INCLUDE_PATH)/ilixi -I$(ILIXI_INCLUDE_PATH)/ilixi/ui -I$(ILIXI_INCLUDE_PATH)/ilixi/types $(LIBXML2_INCLUDES) $(LIBSIGCXX_INCLUDES) $(LIBFONTCONFIG_INCLUDES)

DIRECTFB_APP_SOURCES+= \
	$(LIBFONTCONFIG_SOURCES)	\
	$(LIBSIGCXX_SOURCES)	\
	$(LIBXML2_SOURCES)	\
	$(ILIXI_SOURCE)/build-android/ilixi/compat/pthread_cancel.c	\
	$(ILIXI_SOURCE)/ilixi/graphics/Style.cpp	\
	$(ILIXI_SOURCE)/ilixi/graphics/Stylist.cpp	\
	$(ILIXI_SOURCE)/ilixi/graphics/IconPack.cpp	\
	$(ILIXI_SOURCE)/ilixi/graphics/Palette.cpp	\
	$(ILIXI_SOURCE)/ilixi/graphics/StylistBase.cpp	\
	$(ILIXI_SOURCE)/ilixi/graphics/Painter.cpp	\
	$(ILIXI_SOURCE)/ilixi/graphics/Surface.cpp	\
	$(ILIXI_SOURCE)/ilixi/graphics/FontPack.cpp	\
	$(ILIXI_SOURCE)/ilixi/ui/VideoPlayer.cpp	\
	$(ILIXI_SOURCE)/ilixi/ui/FileBrowser.cpp	\
	$(ILIXI_SOURCE)/ilixi/ui/SurfaceView.cpp	\
	$(ILIXI_SOURCE)/ilixi/ui/Spacer.cpp	\
	$(ILIXI_SOURCE)/ilixi/ui/ComboBox.cpp	\
	$(ILIXI_SOURCE)/ilixi/ui/LineInput.cpp	\
	$(ILIXI_SOURCE)/ilixi/ui/CheckBox.cpp	\
	$(ILIXI_SOURCE)/ilixi/ui/Button.cpp	\
	$(ILIXI_SOURCE)/ilixi/ui/ProgressBar.cpp	\
	$(ILIXI_SOURCE)/ilixi/ui/ToolButton.cpp	\
	$(ILIXI_SOURCE)/ilixi/ui/VBoxLayout.cpp	\
	$(ILIXI_SOURCE)/ilixi/ui/GroupBox.cpp	\
	$(ILIXI_SOURCE)/ilixi/ui/LineSeperator.cpp	\
	$(ILIXI_SOURCE)/ilixi/ui/AppWindow.cpp	\
	$(ILIXI_SOURCE)/ilixi/ui/Widget.cpp	\
	$(ILIXI_SOURCE)/ilixi/ui/Carousel.cpp	\
	$(ILIXI_SOURCE)/ilixi/ui/ScrollArea.cpp	\
	$(ILIXI_SOURCE)/ilixi/ui/HBoxLayout.cpp	\
	$(ILIXI_SOURCE)/ilixi/ui/Slider.cpp	\
	$(ILIXI_SOURCE)/ilixi/ui/ContainerBase.cpp	\
	$(ILIXI_SOURCE)/ilixi/ui/GridLayout.cpp	\
	$(ILIXI_SOURCE)/ilixi/ui/Label.cpp	\
	$(ILIXI_SOURCE)/ilixi/ui/ListBox.cpp	\
	$(ILIXI_SOURCE)/ilixi/ui/LayoutBase.cpp	\
	$(ILIXI_SOURCE)/ilixi/ui/GridView.cpp	\
	$(ILIXI_SOURCE)/ilixi/ui/Frame.cpp	\
	$(ILIXI_SOURCE)/ilixi/ui/TabPanel.cpp	\
	$(ILIXI_SOURCE)/ilixi/ui/Icon.cpp	\
	$(ILIXI_SOURCE)/ilixi/ui/Dialog.cpp	\
	$(ILIXI_SOURCE)/ilixi/ui/SpinBox.cpp	\
	$(ILIXI_SOURCE)/ilixi/ui/ButtonGroup.cpp	\
	$(ILIXI_SOURCE)/ilixi/ui/WindowWidget.cpp	\
	$(ILIXI_SOURCE)/ilixi/ui/DirectionalButton.cpp	\
	$(ILIXI_SOURCE)/ilixi/ui/TextBase.cpp	\
	$(ILIXI_SOURCE)/ilixi/ui/PushButton.cpp	\
	$(ILIXI_SOURCE)/ilixi/ui/ScrollBar.cpp	\
	$(ILIXI_SOURCE)/ilixi/ui/RadioButton.cpp	\
	$(ILIXI_SOURCE)/ilixi/ui/ToolBar.cpp	\
	$(ILIXI_SOURCE)/ilixi/lib/Animation.cpp	\
	$(ILIXI_SOURCE)/ilixi/lib/FileSystem.cpp	\
	$(ILIXI_SOURCE)/ilixi/lib/XMLReader.cpp	\
	$(ILIXI_SOURCE)/ilixi/lib/Easing.cpp	\
	$(ILIXI_SOURCE)/ilixi/lib/Thread.cpp	\
	$(ILIXI_SOURCE)/ilixi/lib/AnimationSequence.cpp	\
	$(ILIXI_SOURCE)/ilixi/lib/Tween.cpp	\
	$(ILIXI_SOURCE)/ilixi/lib/Util.cpp	\
	$(ILIXI_SOURCE)/ilixi/lib/FPSCalculator.cpp	\
	$(ILIXI_SOURCE)/ilixi/lib/Timer.cpp	\
	$(ILIXI_SOURCE)/ilixi/lib/InputHelper.cpp	\
	$(ILIXI_SOURCE)/ilixi/lib/TweenAnimation.cpp	\
	$(ILIXI_SOURCE)/ilixi/lib/FileInfo.cpp	\
	$(ILIXI_SOURCE)/ilixi/core/Window.cpp	\
	$(ILIXI_SOURCE)/ilixi/core/Application.cpp	\
	$(ILIXI_SOURCE)/ilixi/core/EventManager.cpp	\
	$(ILIXI_SOURCE)/ilixi/core/PlatformManager.cpp	\
	$(ILIXI_SOURCE)/ilixi/core/Callback.cpp	\
	$(ILIXI_SOURCE)/ilixi/core/SurfaceEventListener.cpp	\
	$(ILIXI_SOURCE)/ilixi/core/Logger.cpp	\
	$(ILIXI_SOURCE)/ilixi/types/RadioGroup.cpp	\
	$(ILIXI_SOURCE)/ilixi/types/Font.cpp	\
	$(ILIXI_SOURCE)/ilixi/types/Event.cpp	\
	$(ILIXI_SOURCE)/ilixi/types/Point.cpp	\
	$(ILIXI_SOURCE)/ilixi/types/Video.cpp	\
	$(ILIXI_SOURCE)/ilixi/types/TextLayout.cpp	\
	$(ILIXI_SOURCE)/ilixi/types/Size.cpp	\
	$(ILIXI_SOURCE)/ilixi/types/Brush.cpp	\
	$(ILIXI_SOURCE)/ilixi/types/Rectangle.cpp	\
	$(ILIXI_SOURCE)/ilixi/types/Image.cpp	\
	$(ILIXI_SOURCE)/ilixi/types/Color.cpp	\
	$(ILIXI_SOURCE)/ilixi/types/Pen.cpp	\
	$(ILIXI_SOURCE)/ilixi/types/Affine2D.cpp	\
	$(ILIXI_SOURCE)/ilixi/types/FontCache.cpp	\
	$(ILIXI_SOURCE)/ilixi/types/Margin.cpp	\
	$(ILIXI_SOURCE)/ilixi/lib/utf8.c

include $(DFB_INCLUDE_PATH)/build-android/directfb.mk
