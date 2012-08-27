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
    ExposedState = 0x01, //! Pointer is over widget. \sa Widget::enterEvent(), Widget::leaveEvent()
    FocusedState = 0x02,    //!< Widget has key input focus.
    PressedState = 0x04,    //!< Pointer is pressed over widget.
    DisabledState = 0x08,   //!< Widget is disabled.
    InvisibleState = 0x10   //!< Widget is invisible.
};

/*!
 * These flags specify a surface.
 */
enum SurfaceFlags
{
    InitialiseSurface = 0x01, //!< Set if widget should (re)initialise its surface.
    SurfaceModified = 0x02, //!< Set if widget's surface geometry is modified.
    DoZSort = 0x04,
    HasOwnSurface = 0x08, //!< Widget has an independent surface and its surface is not a sub-surface of any parent widget.
    RootSurface = 0x10 //!< Widget's surface is directly acquired from root Window (not a sub-surface).
};

/*!
 * These ORed values are used to describe the functionality of a widget's surface and geometry.
 */
enum SurfaceDescription
{
    DefaultDescription = 0x03, //!< (InitialiseSurface | SurfaceModified)
    BlitDescription = 0x0B, //!< Use if widget surface should be blitted onto another widget/surface, e.g. main widget inside a ScrollArea. (InitialiseSurface | SurfaceModified | HasOwnSurface)
    WindowDescription = 0x13 //!< Use if widget is a WindowWidget, e.g. Application or Dialog. (InitialiseSurface | SurfaceModified | RootWindow)
};

/*!
 * These values are used to define which input events can be consumed by a widget.
 */
enum WidgetInputMethod
{
    NoInput = 0x00, //!< Widget does not consume pointer or key events.
    KeyInput = 0x01, //!< Widget is only capable of consuming key events.
    PointerInput = 0x02, //!< Widget is only capable of consuming pointer events excluding motion.
    KeyAndPointerInput = KeyInput | PointerInput, //!< Widget is capable of consuming both key and pointer events. (KeyInput | PointerInput = 0x03)
    PointerTracking = 0x04, //!< Widget is only capable of consuming pointer motion events.
    PointerInputTracking = PointerInput | PointerTracking, //!< Widget is only capable of consuming pointer events including motion. (PointerInput | PointerTracking = 0x06)
    KeyAndPointerInputTracking = 0x07, //!< Widget is capable of consuming both key and pointer events. (KeyInput | PointerInput | PointerTracking)
    OSKInput = 0x08, //!< Widget can use OSK inputs.
    OSKInputEnabled = 0x0B, //!< (KeyInput | PointerInput | OSKInput)
    OSKInputTracking = 0x0F //!< (KeyInput | PointerInput | PointerTracking | OSKInput)
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
    FixedConstraint = 0x00, //!< Only widget's preferredSize() must be used. Widget can not grow or shrink.
    MinimumConstraint = GrowPolicy, //!< Widget's preferredSize() provides minimum. However, widget can grow if needed. (GrowPolicy)
    MaximumConstraint = ShrinkPolicy, //!< Widget's preferredSize() provides maximum. However, widget can shrink if needed. (ShrinkPolicy)
    NoConstraint = (GrowPolicy | ShrinkPolicy), //!< Widget's preferredSize() is used. However, widget can grow or shrink if needed. (GrowPolicy | ShrinkPolicy)
    MinimumExpandingConstraint = (GrowPolicy | ExpandPolicy), //!< Widget's preferredSize() is used. However, widget can not shrink. Widget should expand if possible. (GrowPolicy | ExpandPolicy)
    ExpandingConstraint = (GrowPolicy | ExpandPolicy | ShrinkPolicy), //!< Widget's preferredSize() is used. However, widget can grow or shrink if needed. Widget should expand if possible. (GrowPolicy | ShrinkPolicy | ExpandPolicy)
    IgnoredConstraint = 0xFF //!< Widget is ignored by layout. Its position or size is not modified.
};

namespace StyleHint
{
enum AnimatedStates
{
    FocusIn, FocusOut
};

enum Font
{
    DefaultFont, TitleFont, InputFont, ButtonFont
};

enum DefaultIcon
{
    Information, Question, Warning, Critical
};

enum Size
{
    Icon,
    PushButton,
    CheckBox,
    RadioButton,
    ProgressBar,
    Slider,
    ScrollBar,
    ScrollBarButton,
    TabPanel,
    TabPanelButton,
    TabPanelButtonIcon
};

enum Parameter
{
    ButtonOffset,
    PushButtonCornerLeft,
    PushButtonCornerRight,
    PushButtonCorners,
    PushButtonHeight,

    CheckBoxHeight,
    CheckBoxWidth,
    RadioHeight,
    RadioWidth,

    FrameOffsetLeft,
    FrameOffsetRight,
    FrameOffsetTop,
    FrameOffsetBottom,
    FrameOffsetLR,
    FrameOffsetTB,

    TabOffsetLeft,
    TabOffsetRight,
    TabOffsetTop,
    TabOffsetBottom,
    TabOffsetLR,
    TabOffsetTB,

    LineInputLeft,
    LineInputRight,
    LineInputLR,
    LineInputTop,
    LineInputBottom,
    LineInputTB,

    ToolButtonLeft,
    ToolButtonRight,
    ToolButtonLR,
    ToolButtonTop,
    ToolButtonBottom,
    ToolButtonTB,
    ToolButtonIndicator,

    SliderIndicatorWidth,
    SliderIndicatorHeight
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
    Left,   //!< Align left
    Center, //!< Align center
    Right   //!< Align top
};

//! Vertical alignment
enum Vertical
{
    Top,    //!< Align top
    Middle, //!< Align middle
    Bottom  //!< Align bottom
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
    BottomCorners = 0x0C,        //!< Bottom corners (BottomLeft | BottomRight).
    AllCorners = 0x0F, //!< All corners (TopLeft | TopRight | BottomLeft | BottomRight).
    AllExceptTopLeft = 0x0E,        //!< (TopRight | BottomLeft | BottomRight).
    AllExceptTopRight = 0x0D,       //!< (TopLeft | BottomLeft | BottomRight).
    AllExceptBottomLeft = 0x0B,     //!< (TopLeft | TopRight | BottomRight).
    AllExceptBottomRight = 0x07,    //!< (TopLeft | TopRight | BottomLeft).
};

enum AppOptions
{
    OptNone = 0x000, //!< Default application option.
    OptExclusive = 0x001, //!< Application will use layer exclusively.
    OptFullScreenUpdate = 0x002, //!< Windows will update whole surface each time.
    OptTripleAccelerated = 0x004,
    OptDale = 0x200,    //! Enable FusionDale interfaces for Application.
    OptSound = 0x400    //! Enable FusionSound interfaces for Application.
};
}

#endif /* ILIXI_ENUMS_H_ */
