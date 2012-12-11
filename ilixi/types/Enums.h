/*
 Copyright 2010-2012 Tarik Sekmen.

 All Rights Reserved.

 Written by Tarik Sekmen <tarik@ilixi.org>.

 This file is part of ilixi.

 ilixi is free software: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 ilixi is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with ilixi.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ILIXI_ENUMS_H_
#define ILIXI_ENUMS_H_

namespace ilixi
{
/*!
 * The state of a widget is mainly used for painting and it is usually changed upon receiving input events.
 */
enum WidgetState
{
    DefaultState = 0x00,    //!< Widget is visible and at its default state.
    ExposedState = 0x01,    //! Pointer is over widget. \sa Widget::enterEvent(), Widget::leaveEvent()
    FocusedState = 0x02,    //!< Widget has key input focus.
    PressedState = 0x04,    //!< Pointer is pressed over widget.
    DisabledState = 0x08,   //!< Widget is disabled.
    InvisibleState = 0x10,   //!< Widget is invisible.
    GrabbedState = 0x20
};

/*!
 * These values are used to define which input events can be consumed by a widget.
 */
enum WidgetInputMethod
{
    NoInput = 0x0000,                 //!< Widget does not consume pointer or key events.
    KeyInput = 0x0001,                //!< Widget is capable of consuming key events only.
    PointerInput = 0x0002,            //!< Widget is capable of consuming pointer events excluding motion.
    PointerInputTracking = 0x0004,    //!< Widget is capable of consuming pointer motion events only.
    PointerPassthrough = 0x0008,      //!< Widget passes all pointer events to children.
    PointerGrabbing = 0x0010,         //!< Widget can grab pointer events in certain cases.
    OSKInput = 0x0020,                //!< Widget is capable of consuming input from OSK component only.
    KeyPointer = KeyInput | PointerInput,
    PointerTracking = PointerInput | PointerInputTracking,
    KeyPointerTracking = KeyInput | PointerInput | PointerInputTracking,
    KeyPointerOSK = KeyInput | PointerInput | OSKInput,
    KeyPointerTrackingOSK = KeyInput | PointerInput | PointerInputTracking | OSKInput
};

/*!
 * This enum is used to set widget's resize constraints.
 */
enum WidgetResizePolicy
{
    GrowPolicy = 0x01,      //!< Widget is able to grow and function properly.
    ShrinkPolicy = 0x02,    //!< Widget is able to shrink and function properly.
    ExpandPolicy = 0x04,    //!< Widget should get as much space as possible.
    IgnorePolicy = 0x08,    //!< Widget is ignored by layout manager.
};

/*!
 * ResizeConstraint is used to specify how widgets should behave inside layouts.
 *
 * Each widget has horizontal and vertical constraints and they alter layout's
 * default resizing behaviour for width or height respectively.
 */
enum WidgetResizeConstraint
{
    FixedConstraint = 0x00,              //!< Only widget's preferredSize() must be used. Widget can not grow or shrink.
    MinimumConstraint = GrowPolicy, //!< Widget's preferredSize() provides minimum. However, widget can grow if needed. (GrowPolicy)
    MaximumConstraint = ShrinkPolicy, //!< Widget's preferredSize() provides maximum. However, widget can shrink if needed. (ShrinkPolicy)
    NoConstraint = (GrowPolicy | ShrinkPolicy), //!< Widget's preferredSize() is used. However, widget can grow or shrink if needed. (GrowPolicy | ShrinkPolicy)
    MinimumExpandingConstraint = (GrowPolicy | ExpandPolicy), //!< Widget's preferredSize() is used. However, widget can not shrink. Widget should expand if possible. (GrowPolicy | ExpandPolicy)
    ExpandingConstraint = (GrowPolicy | ExpandPolicy | ShrinkPolicy), //!< Widget's preferredSize() is used. However, widget can grow or shrink if needed. Widget should expand if possible. (GrowPolicy | ShrinkPolicy | ExpandPolicy)
    IgnoredConstraint = 0xFF                     //!< Widget is ignored by layout. Its position or size is not modified.
};

namespace StyleHint
{
enum AnimatedStates
{
    FocusIn,
    FocusOut
};

enum Font
{
    ButtonFont,
    DefaultFont,
    InputFont,
    TitleFont
};

enum PackedIcon
{
    ArrowNE,
    ArrowSW,
    Calendar,
    Clock,
    Comment,
    CPU,
    Cross,
    CrossCircle,
    Down,
    Eject,
    File,
    FileDelete,
    FileDownload,
    FileNew,
    FileText,
    FileUpload,
    Flag,
    Heart,
    Info,
    Left,
    Minus,
    Network,
    Next,
    Next2,
    Pause,
    Phone,
    Picture,
    Play,
    Plus,
    Power,
    Prev,
    Prev2,
    RAM,
    Restart,
    Ribbon,
    Right,
    Search,
    Settings,
    Share,
    Standby,
    Star,
    Stats,
    Stop,
    Tag,
    ThumbDown,
    ThumbUp,
    Tick,
    TickCircle,
    Up,
    ZoomIn,
    ZoomOut
};

enum Size
{
    CheckBox,
    Icon,
    ProgressBar,
    PushButton,
    RadioButton,
    ScrollBar,
    ScrollBarButton,
    Slider,
    SliderV,
    TabPanel,
    TabPanelButton,
    TabPanelButtonIcon
};

enum Parameter
{
    ButtonOffset,
    CheckBoxHeight,
    CheckBoxWidth,
    FrameOffsetBottom,
    FrameOffsetLeft,
    FrameOffsetLR,
    FrameOffsetRight,
    FrameOffsetTB,
    FrameOffsetTop,
    LineInputBottom,
    LineInputLeft,
    LineInputLR,
    LineInputRight,
    LineInputTB,
    LineInputTop,
    PushButtonLeft,
    PushButtonRight,
    PushButtonLR,
    PushButtonHeight,
    RadioHeight,
    RadioWidth,
    SliderIndicatorHeight,
    SliderIndicatorWidth,
    TabOffsetBottom,
    TabOffsetLeft,
    TabOffsetLR,
    TabOffsetRight,
    TabOffsetTB,
    TabOffsetTop,
    ToolButtonBottom,
    ToolButtonIndicator,
    ToolButtonLeft,
    ToolButtonLR,
    ToolButtonRight,
    ToolButtonTB,
    ToolButtonTop
};
} // end StyleHint

/*!
 * This enum is used to specify orientation of a widget, e.g. a horizontal slider control.
 */
enum Orientation
{
    Horizontal, //!< Horizontal orientation
    Vertical    //!< Vertical orientation
};

/*!
 * This enum specifies direction, e.g. an arrow direction inside a button.
 */
enum Direction
{
    Left,   //!< Left
    Right,  //!< Right
    Up,     //!< Up
    Down    //!< Down
};

namespace Alignment
{
//! Horizontal alignment
enum Horizontal
{
    Left = 0x01,    //!< Align left
    Center = 0x02,  //!< Align center
    Right = 0x04    //!< Align top
};

//! Vertical alignment
enum Vertical
{
    Top = 0x01,     //!< Align top
    Middle = 0x02,  //!< Align middle
    Bottom = 0x04   //!< Align bottom
};
}

/*!
 * This enum specifies a corner inside a rectangle.
 */
enum Corner
{
    TopLeft = 0x01,     //!< Top left corner.
    TopRight = 0x02,    //!< Top right corner.
    BottomLeft = 0x04,  //!< Bottom left corner.
    BottomRight = 0x08  //!< Bottom right corner.
};

/*!
 * This enum specifies groups of corners of a rectangle.
 */
enum Corners
{
    NoCorners = 0x00,               //!< No corners.
    LeftCorners = 0x05,             //!< Left corners (TopLeft | BottomLeft).
    RightCorners = 0x0A,            //!< Right corners (TopRight | BottomRight).
    TopCorners = 0x03,              //!< Top corners (TopLeft | TopRight).
    BottomCorners = 0x0C,           //!< Bottom corners (BottomLeft | BottomRight).
    AllCorners = 0x0F,              //!< All corners (TopLeft | TopRight | BottomLeft | BottomRight).
    AllExceptTopLeft = 0x0E,        //!< (TopRight | BottomLeft | BottomRight).
    AllExceptTopRight = 0x0D,       //!< (TopLeft | BottomLeft | BottomRight).
    AllExceptBottomLeft = 0x0B,     //!< (TopLeft | TopRight | BottomRight).
    AllExceptBottomRight = 0x07,    //!< (TopLeft | TopRight | BottomLeft).
};

enum AppOptions
{
    OptNone = 0x00000000,               //!< Default application option.
    OptExclusive = 0x00000001,          //!< Application will use layer exclusively.
    OptDale = 0x00000010,               //!< Enable FusionDale interfaces for Application.
    OptSound = 0x00000020,              //!< Enable FusionSound interfaces for Application.
    OptTripleAccelerated = 0x00000200
};

enum LayerFlipMode
{
    FlipNone,
    FlipOnSync,
    FlipWaitForSync,
    FlipNew
};

enum PushButtonStyle
{
    Default,
    OK,
    CANCEL
};

}

#endif /* ILIXI_ENUMS_H_ */
